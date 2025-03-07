#include <stdio.h>

extern "C" {
    #include "cc.h"
    #include "scan.h"
    #include "semutil.h"
    #include "sem.h"
    #include "sym.h"
}

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/Function.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include <utility>
#include <cstdlib>
#include <memory>
#include <cctype>
#include <cstdio>
#include <string>
#include <vector>
#include <list>
#include <map>

# define MAXLOOPNEST 50
# define MAXLABELS 50
# define MAXGOTOS 50

using std::map;
using std::string;
using std::vector;
using llvm::outs;
using llvm::Type;
using llvm::Value;
using llvm::Module;
using llvm::Function;
using llvm::Constant;
using llvm::IRBuilder;
using llvm::ArrayType;
using llvm::BasicBlock;
using llvm::AllocaInst;
using llvm::BranchInst;
using llvm::Instruction;
using llvm::LLVMContext;
using llvm::ConstantInt;
using llvm::GlobalValue;
using llvm::IntegerType;
using llvm::PointerType;
using llvm::FunctionType;
using llvm::GlobalVariable;
using llvm::ConstantAggregateZero;

extern int formalnum;                         /* number of formal arguments */
extern struct id_entry* formalvars[MAXLOCS];  /* entries for parameters */
extern int localnum;                          /* number of local variables  */
extern struct id_entry* localvars[MAXLOCS];   /* entries for local variables */

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

static int label_index = 0;
int relexpr = 0;

struct loopscope {
  struct sem_rec* breaks;
  struct sem_rec* conts;
} lscopes[MAXLOOPNEST];

static int looplevel = 0;
struct loopscope *looptop = (struct loopscope *) NULL;

struct labelnode {
   const char *id;    /* label string    */
   BasicBlock *bb;    /* basic block for label */
} labels[MAXLABELS];

struct gotonode {
   const char *id;     /* label string in goto statement */
   BranchInst *branch; /* branch to temporary label */
} gotos[MAXGOTOS];     /* list of gotos to be backpatched */

int numgotos = 0;    /* number of gotos to be backpatched */
int numlabelids = 0; /* total label ids in function */

std::string new_label()
{
  return ("L" + std::to_string(label_index++));
}

BasicBlock*
create_tmp_label()
{
  return BasicBlock::Create(TheContext);
}

BasicBlock*
create_named_label(std::string label)
{
  Function *curr_func = Builder.GetInsertBlock()->getParent();
  BasicBlock *new_block = BasicBlock::Create(TheContext, label, curr_func);
  return new_block;
}

/*
 * startloopscope - start the scope for a loop
 */
void
startloopscope()
{
   looptop = &lscopes[looplevel++];
   if (looptop > lscopes+MAXLOOPNEST) {
      fprintf(stderr, "loop nest too great\n");
      exit(1);
   }
   looptop->breaks = (struct sem_rec *) NULL;
   looptop->conts = (struct sem_rec *) NULL;
}

/*
 * endloopscope - end the scope for a loop
 */
void
endloopscope()
{
  looplevel--;
  looptop--;
}


/*
 * Global allocations. Globals are initialized to 0.
 */
void
global_alloc (struct id_entry *p, int width)
{
  string name(p->i_name);
  GlobalVariable *var;
  Type *type;
  Constant *init;

  if (p->i_type & T_INT) {
    if (p->i_type & T_ARRAY) {
      type = ArrayType::get(Type::getInt32Ty(TheContext), width);
      init = ConstantAggregateZero::get(type);
    }
    else {
      type = Type::getInt32Ty(TheContext);
      init = ConstantInt::get(Type::getInt32Ty(TheContext), 0);
    }
  }
  else {
    init = ConstantInt::get(Type::getDoubleTy(TheContext), 0);
    if (p->i_type & T_ARRAY) {
      type = ArrayType::get(Type::getDoubleTy(TheContext), width);
      init = ConstantAggregateZero::get(type);
    }
    else {
      type = Type::getDoubleTy(TheContext);
      init = ConstantInt::get(Type::getInt32Ty(TheContext), 0);
    }
  }

  TheModule->getOrInsertGlobal(name, type);
  var = TheModule->getNamedGlobal(name);
  var->setInitializer(init);
  p->i_value = (void*) var; 
}


/*
 * backpatch - set temporary labels in the sem_rec to real labels
 *
 * LLVM API calls:
 *
 * llvm::dyn_cast<BranchInst>(Value*)
 * BranchInst::getNumSuccessors()
 * BranchInst::getSuccessor(unsigned)
 * BranchInst::setSuccessor(unsigned, BasicBlock*)
 */
void backpatch(struct sem_rec *rec, void *bb)
{
  unsigned i;
  BranchInst *br_inst;
  struct sem_rec *currec = rec;

  while (currec != NULL)
  {
    if ( (br_inst = llvm::dyn_cast<BranchInst>((Value *) currec->s_value)) )
    {
      for (i = 0; i < br_inst->getNumSuccessors(); i++)
      {
        if (br_inst->getSuccessor(i) == ((BasicBlock *) currec->s_bb))
        {
          br_inst->setSuccessor(i, (BasicBlock *) bb);
        }
      }
    }
    else
    {
      fprintf(stderr, "error: backpatch with non-branch instruction\n");
      exit(1);
    }

    currec = currec->s_link;
  }

  // fprintf(stderr, "sem: backpatch not implemented\n");
  // return;
}


/*
 * call - procedure invocation
 *
 * Grammar:
 * lval -> ID '(' ')'            { $$ = call($1, (struct sem_rec *) NULL); }
 * lval -> ID '(' exprs ')'      { $$ = call($1, $3); }
 *
 * LLVM API calls:
 * makeArrayRef(vector<Value*>)
 * IRBuilder::CreateCall(Function *, ArrayRef<Value*>)
 */
struct sem_rec*
call(char *f, struct sem_rec *args)
{
  struct sem_rec *rec = NULL;
  vector<Value *> *argvec = new vector<Value *>();
  llvm::ArrayRef<Value *> vref;
  id_entry *entry;

  if (args != NULL)
  {
    vector<Value *> *tmpvec = new vector<Value *>();
    sem_rec *tmprec = args;

    while (tmprec != NULL)
    {
      tmpvec->push_back((Value *)(tmprec->s_value));
      tmprec = tmprec->s_link;
    }

    for (int i = tmpvec->size() - 1; i >= 0; i--)
    {
      argvec->push_back(tmpvec->at(i));
    }
  }

  vref = llvm::makeArrayRef(*argvec);
  entry = lookup(f, 0);
  rec = s_node( Builder.CreateCall((Function *)(entry->i_value), vref), entry->i_type);

  return rec;

  fprintf(stderr, "sem: call not implemented\n");
  return ((struct sem_rec*) NULL);
}


/*
 * ccand - logical and
 *
 * Grammar:
 * cexpr -> cexpr AND m cexpr     { $$ = ccand($1, $3, $4); }
 *
 * LLVM API calls:
 * None
 */
struct sem_rec*
ccand(struct sem_rec *e1, void *m, struct sem_rec *e2)
{
  sem_rec *rec = NULL;
  sem_rec *truelist;
  sem_rec *falselist;

  backpatch(e1->s_true, m);
  truelist = e2->s_true;
  falselist = merge(e1->s_false, e2->s_false);

  rec = node( (void *) NULL, (void *) NULL, 0, (struct sem_rec *) NULL, truelist, falselist);
  return rec;

  // fprintf(stderr, "sem: ccand not implemented\n");
  // return ((struct sem_rec*) NULL);
}

/*
 * ccexpr - convert arithmetic expression to logical expression
 *
 * Grammar:
 * cexpr -> expr                  { $$ = ccexpr($1); }
 *
 * LLVM API calls:
 *
 * IRBuilder::CreateICmpNE(Value *, Value *)
 * IRBuilder::CreateFCmpONE(Value *, Value *)
 * IRBuilder::CreateCondBr(Value *, BasicBlock *, BasicBlock *)
 */
struct sem_rec*
ccexpr(struct sem_rec *e)
{
  BasicBlock *tmp_true, *tmp_false;
  Value *val;

  tmp_true = create_tmp_label();
  tmp_false = create_tmp_label();
  val = Builder.CreateCondBr( (Value *)e->s_value, tmp_true, tmp_false );

  return node( (void *) NULL, (void *) NULL, 0, (struct sem_rec *) NULL, 
              node(val, tmp_true, 0, (struct sem_rec *) NULL, (struct sem_rec *) NULL, (struct sem_rec *) NULL),
              node(val, tmp_false, 0, (struct sem_rec *) NULL, (struct sem_rec *) NULL, (struct sem_rec *) NULL) );

  // fprintf(stderr, "sem: ccexpr not implemented\n");
  // return ((struct sem_rec *) NULL);
}

/*
 * ccnot - logical not
 *
 * Grammar:
 * cexpr -> NOT cexpr             { $$ = ccnot($2); }
 *
 * LLVM API calls:
 * None
 */
struct sem_rec*
ccnot(struct sem_rec *e)
{
  return node( NULL, NULL, 0, NULL, e->s_false, e->s_true );

  fprintf(stderr, "sem: ccnot not implemented\n");
  return ((struct sem_rec *) NULL);
}

/*
 * ccor - logical or
 *
 * Grammar:
 * cexpr -> cexpr OR m cexpr      { $$ = ccor($1, $3, $4); }
 *
 * LLVM API calls:
 * None -- but uses backpatch
 */
struct sem_rec*
ccor(struct sem_rec *e1, void *m, struct sem_rec *e2)
{
  sem_rec *rec = NULL;
  sem_rec *truelist;
  sem_rec *falselist;

  backpatch(e1->s_false, m);
  truelist = merge(e1->s_true, e2->s_true);
  falselist = e2->s_false;

  rec = node( (void *) NULL, (void *) NULL, 0, (struct sem_rec *) NULL, truelist, falselist);
  return rec;

  fprintf(stderr, "sem: ccor not implemented\n");
  return NULL;
}

/*
 * con - constant reference in an expression
 *
 * Grammar:
 * expr -> CON                   { $$ = con($1); }
 *
 * LLVM API calls:
 * Type::getInt32Ty(LLVMContext&);
 * ConstantInt::get(Type*, uint64_t, bool)
 */
struct sem_rec*
con(const char *x)
{
  struct id_entry *entry;
  if ((entry = lookup(x, 0)) == NULL) {
    entry = install (x, 0);
    entry->i_type = T_INT;
    entry->i_scope = GLOBAL;
    entry->i_defined = 1;
  }

  entry->i_value = (void *) ConstantInt::get(Type::getInt32Ty(TheContext), std::stoi(x));
  return s_node( (void *) entry->i_value, entry->i_type );

  // fprintf(stderr, "sem: remainder of con not implemented\n");
  // return ((struct sem_rec *) NULL);
}

/*
 * dobreak - break statement
 *
 * Grammar:
 * stmt -> BREAK ';'                { dobreak(); }
 *
 * LLVM API calls:
 * None -- but uses n
 */
void
dobreak()
{
  struct sem_rec *new_break = n();

  new_break->s_link = looptop->breaks;
  looptop->breaks = new_break;

  // fprintf(stderr, "sem: dobreak not implemented\n");
  // return;
}

/*
 * docontinue - continue statement
 *
 * Grammar:
 * stmt -> CONTINUE ';'              { docontinue(); }
 *
 * LLVM API calls:
 * None -- but uses n
 */
void
docontinue()
{
  struct sem_rec *new_cont = n();

  new_cont->s_link = looptop->conts;
  looptop->conts = new_cont;

  // fprintf(stderr, "sem: docontinue not implemented\n");
  // return;
}

/*
 * dodo - do statement
 *
 * Grammar:
 * stmt -> DO m s lblstmt WHILE '(' m cexpr ')' ';' m
 *                { dodo($2, $7, $8, $11); }
 *
 * LLVM API calls:
 * None -- but uses backpatch
 */
void
dodo(void *m1, void *m2, struct sem_rec *cond, void *m3)
{
  backpatch(cond->s_true, m1);
  backpatch(cond->s_false, m3);

  struct sem_rec *rec = looptop->breaks;
  while (rec != NULL)
  {
    backpatch(rec, m3);
    rec = rec->s_link;
  }

  rec = looptop->conts;
  while (rec != NULL)
  {
    backpatch(rec, m2);
    rec = rec->s_link;
  }

  // fprintf(stderr, "sem: dodo not implemented\n");
  // return;
}

/*
 * dofor - for statement
 *
 * Grammar:
 * stmt -> FOR '(' expro ';' m cexpro ';' m expro n ')' m s lblstmt n m
 *               { dofor($5, $6, $8, $10, $12, $15, $16); }
 *
 * LLVM API calls:
 * None -- but uses backpatch
 */
void
dofor(void *m1, struct sem_rec *cond, void *m2, struct sem_rec *n1, void *m3,
  struct sem_rec *n2, void *m4)
{
  backpatch(cond->s_true, m3);
  backpatch(cond->s_false, m4);
  backpatch(n2, m2);
  backpatch(n1, m1);

  struct sem_rec *rec = looptop->breaks;
  while (rec != NULL)
  {
    backpatch(rec, m4);
    rec = rec->s_link;
  }

  rec = looptop->conts;
  while (rec != NULL)
  {
    backpatch(rec, m2);
    rec = rec->s_link;
  }

  // fprintf(stderr, "sem: dofor not implemented\n");
  // return;
}

/*
 * dogoto - goto statement
 *
 * Grammar:
 * stmt -> GOTO ID ';'              { dogoto($2); }
 *
 * LLVM API calls:
 * IRBuilder::CreateBr(BasicBlock *)
 */
void
dogoto(char *id)
{
  fprintf(stderr, "sem: dogoto not implemented\n");
  return;
}

/*
 * doif - one-arm if statement
 *
 * Grammar:
 * stmt -> IF '(' cexpr ')' m lblstmt m
 *         { doif($3, $5, $7); }
 *
 * LLVM API calls:
 * None -- but uses backpatch
 */
void
doif(struct sem_rec *cond, void *m1, void *m2)
{
  backpatch(cond->s_true, m1);
  backpatch(cond->s_false, m2);

  // fprintf(stderr, "sem: doif not implemented\n");
  // return;
}

/*
 * doifelse - if then else statement
 *
 * Grammar:
 * stmt -> IF '(' cexpr ')' m lblstmt ELSE n m lblstmt m
 *                { doifelse($3, $5, $8, $9, $11); }
 *
 * LLVM API calls:
 * None -- but uses backpatch
 */
void
doifelse(struct sem_rec *cond, void *m1, struct sem_rec *n,
  void *m2, void *m3)
{
  backpatch(cond->s_true, m1);
  backpatch(cond->s_false, m2);
  backpatch(n, m3);

  // fprintf(stderr, "sem: doifelse not implemented\n");
  // return;
}

/*
 * doret - return statement
 *
 * Grammar:
 * stmt -> RETURN ';'            { doret((struct sem_rec *) NULL); }
 * stmt -> RETURN expr ';'       { doret($2); }
 *
 * LLVM API calls:
 * IRBuilder::CreateRetVoid();
 * IRBuilder::CreateRet(Value *);
 */
void
doret(struct sem_rec *e)
{
  if (!e)
  {
    Builder.CreateRetVoid();
    return;
  }

  Builder.CreateRet( ((Value *)e->s_value) );

  // fprintf(stderr, "sem: doret not implemented\n");
  // return;
}

/*
 * dowhile - while statement
 *
 * Grammar:
 * stmt -> WHILE '(' m cexpr ')' m s lblstmt n m
 *                { dowhile($3, $4, $6, $9, $10); }
 *
 * LLVM API calls:
 * None -- but uses backpatch
 */
void
dowhile(void *m1, struct sem_rec *cond, void *m2,
  struct sem_rec *n, void *m3)
{
  backpatch(cond->s_true, m2);
  backpatch(cond->s_false, m3);
  backpatch(n, m1);

  struct sem_rec *rec = looptop->breaks;
  while (rec != NULL)
  {
    backpatch(rec, m3);
    rec = rec->s_link;
  }

  rec = looptop->conts;
  while (rec != NULL)
  {
    backpatch(rec, m1);
    rec = rec->s_link;
  }

  // fprintf(stderr, "sem: dowhile not implemented\n");
  // return;
}

/*
 * exprs - form a list of expressions
 *
 * Grammar:
 * exprs -> exprs ',' expr        { $$ = exprs($1, $3); }
 *
 * LLVM API calls:
 * None
 */
struct sem_rec*
exprs(struct sem_rec *l, struct sem_rec *e)
{
  struct sem_rec *rec = NULL;

  rec = node(e->s_value, e->s_bb, e->s_type, l, e->s_true, e->s_false);

  return rec;

  fprintf(stderr, "sem: exprs not implemented\n");
  return ((struct sem_rec *) NULL);
}

/*
 * fhead - beginning of function body
 *
 * Grammar:
 * fhead -> fname fargs '{' dcls  { fhead($1); }
 */
void
fhead(struct id_entry *p)
{
  Type *func_type, *var_type;
  Value *arr_size;
  vector<Type*> func_args;
  GlobalValue::LinkageTypes linkage;
  FunctionType *FT;
  Function *F;
  BasicBlock *B;
  int i;
  struct id_entry *v;

  /* get function return type */
  func_type = (p->i_type == T_DOUBLE)       ?
              Type::getDoubleTy(TheContext) :
              Type::getInt32Ty(TheContext)  ;

  /* get function argument types */
  for (i = 0; i < formalnum; i++) {
    if (formalvars[i]->i_type == T_DOUBLE) {
      func_args.push_back(Type::getDoubleTy(TheContext));
    } else {
      func_args.push_back(Type::getInt32Ty(TheContext));
    }
  }

  FT = FunctionType::get(func_type, makeArrayRef(func_args), false);

  /* linkage is external if function is main */
  linkage = (strcmp(p->i_name, "main") == 0) ?
            Function::ExternalLinkage :
            Function::InternalLinkage ;

  F = Function::Create(FT, linkage, p->i_name, TheModule.get());
  p->i_value = (void*) F;


  B = BasicBlock::Create(TheContext, "", F);
  Builder.SetInsertPoint(B);

  /* 
   * Allocate instances of each parameter and local so they can be referenced
   * and mutated.
   */
  i = 0;
  for (auto &arg : F->args()) {

    v = formalvars[i++];
    arg.setName(v->i_name);
    var_type = (v->i_type == T_DOUBLE) ?
                Type::getDoubleTy(TheContext) :
                Type::getInt32Ty(TheContext);
    arr_size = (v->i_width > 1) ? 
               (ConstantInt::get(Type::getInt32Ty(TheContext), v->i_width)) :
               NULL;

    v->i_value = Builder.CreateAlloca(var_type, arr_size, arg.getName());
    Builder.CreateStore(&arg, (Value*)v->i_value);
  }

  /* Create the instance of stack memory for each local variable */
  for (i = 0; i < localnum; i++) {
    v = localvars[i];
    var_type = (v->i_type & T_DOUBLE) ?
                Type::getDoubleTy(TheContext) :
                Type::getInt32Ty(TheContext);
    arr_size = (v->i_width > 1) ? 
               (ConstantInt::get(Type::getInt32Ty(TheContext), v->i_width)) :
               NULL;

    v->i_value = Builder.CreateAlloca(var_type, arr_size, std::string(v->i_name));
  }
}

/*
 * fname - function declaration
 *
 * Grammar:
 * fname -> type ID               { $$ = fname($1, $2); }
 * fname -> ID                    { $$ = fname(T_INT, $1); }
 */
struct id_entry*
fname(int t, char *id)
{
  struct id_entry *entry = lookup(id, 0);

  // add function to hash table if it doesn't exist 
  if (!entry) {
    entry = install(id, 0);
  }

  // cannot have two functions of the same name
  if (entry->i_defined) {
    yyerror("cannot declare function more than once");
  }

  entry->i_type = t;
  entry->i_scope = GLOBAL;
  entry->i_defined = true;

  // need to enter the block to let hash table do internal work
  enterblock();
  // then need to reset argument count variables

  formalnum = 0;
  localnum = 0;

  return entry;
}

/*
 * ftail - end of function body
 *
 * Grammar:
 * func -> fhead stmts '}'       { ftail(); }
 */
void
ftail()
{
  numgotos = 0;
  numlabelids = 0;
  leaveblock();
}

/*
 * id - variable reference
 *
 * Grammar:
 * lval -> ID                    { $$ = id($1); }
 * lval -> ID '[' expr ']'       { $$ = indx(id($1), $3); }
 *
 * LLVM API calls:
 * None
 */
struct sem_rec*
id(char *x)
{
  struct id_entry *entry;
  
  if ((entry = lookup(x, 0)) == NULL) {
    yyerror("undeclared identifier");
    entry = install(x, -1);
    entry->i_type = T_INT;
    entry->i_scope = LOCAL;
    entry->i_defined = 1;
  }

  return s_node( (void *) entry->i_value, entry->i_type | T_ADDR );

  // fprintf(stderr, "sem: remainder of id not implemented\n");
  // return ((struct sem_rec *) NULL);
}

/*
 * indx - subscript
 * 
 * Grammar:
 * lval -> ID '[' expr ']'       { $$ = indx(id($1), $3); }
 *
 * LLVM API calls:
 * ConstantInt::get(Type *, uint64_t, bool)
 * IntegerType::getInt32Ty(LLVMContext &)
 * makeArrayRef(vector<Value*>)
 * IRBuilder::CreateGEP(Type, Value *, ArrayRef<Value*>)
 */
struct sem_rec*
indx(struct sem_rec *x, struct sem_rec *i)
{
  vector<Value *> *vec = new vector<Value *>();

  vec->push_back(ConstantInt::get(IntegerType::getInt32Ty(TheContext), 0));
  vec->push_back((Value *) i->s_value);

  llvm::ArrayRef<Value *> arr = llvm::makeArrayRef(*vec);

  return s_node( Builder.CreateGEP((Value *) x->s_value, arr), x->s_type & (~T_ARRAY));

  fprintf(stderr, "sem: indx not implemented\n");
  return ((struct sem_rec *) NULL);
}

/*
 * labeldcl - process a label declaration
 *
 * Grammar:
 * labels -> ID ':'                { labeldcl($1); }
 * labels -> labels ID ':'         { labeldcl($2); }
 * 
 * NOTE: All blocks in LLVM must have a terminating instruction (i.e., branch
 * or return statement -- fall-throughs are not allowed). This code must
 * ensure that each block ends with a terminating instruction.
 *
 * LLVM API calls:
 * IRBuilder::GetInsertBlock()
 * BasicBlock::getTerminator()
 * IRBuilder::CreateBr(BasicBlock*)
 * IRBuilder::SetInsertPoint(BasicBlock*)
 * BranchInst::setSuccessor(unsigned, BasicBlock*)
 */
void
labeldcl(const char *id)
{
  fprintf(stderr, "sem: labeldcl not implemented\n"); 
  return;
}

/*
 * m - generate label and return next temporary number
 *
 * NOTE: All blocks in LLVM must have a terminating instruction (i.e., branch
 * or return statement -- fall-throughs are not allowed). This code must
 * ensure that each block ends with a terminating instruction.
 *
 * LLVM API calls:
 * IRBuilder::GetInsertBlock()
 * BasicBlock::getTerminator()
 * IRBuilder::CreateBr(BasicBlock*)
 * IRBuilder::SetInsertPoint(BasicBlock*)
 */
void*
m ()
{
  BasicBlock *bb;

  std::string label = new_label();
  bb = create_named_label(label);

  if (Builder.GetInsertBlock()->getTerminator() == NULL)
  {
    Builder.CreateBr(bb);
  }

  Builder.SetInsertPoint(bb);
  return (void *) bb;

  // fprintf(stderr, "sem: m not implemented\n");
  // return (void *) NULL;
}

/*
 * n - generate goto and return backpatch pointer
 *
 * LLVM API calls:
 * IRBuilder::CreateBr(BasicBlock *)
 */
struct sem_rec *n()
{
  BasicBlock *bb;

  bb = create_tmp_label();

  return node((void *) Builder.CreateBr(bb), (void *) bb, 0, (struct sem_rec *) NULL, (struct sem_rec *) NULL, (struct sem_rec *) NULL);

  fprintf(stderr, "sem: n not implemented\n");
  return NULL;
}

/*
 * op1 - unary operators
 *
 * LLVM API calls:
 * IRBuilder::CreateLoad(Type, Value *)
 * IRBuilder::CreateNot(Value *)
 * IRBuilder::CreateNeg(Value *)
 * IRBuilder::CreateFNeg(Value *)
 */
struct sem_rec*
op1(const char *op, struct sem_rec *y)
{
  Type *ty;
  struct sem_rec *rec = NULL;
  if (*op == '@')
  {
    if (!(y->s_type & T_ARRAY))
    {
      y->s_type &= ~T_ADDR;
      ty = llvm::dyn_cast<PointerType>(((Value*)y->s_value)->getType()->getScalarType())->getElementType();
      rec = s_node ( Builder.CreateLoad(ty, ((Value *)y->s_value)), y->s_type );
    }
  }
  else if (*op == '~')
  {
    if (y->s_type & T_INT)
    {
      rec = s_node ( Builder.CreateNot((Value *)y->s_value), y->s_type );
    }
  }
  else if (*op == '-')
  {
    if (y->s_type & T_DOUBLE)
    {
      rec = s_node ( Builder.CreateFNeg((Value *)y->s_value), y->s_type );
    }
    else if (y->s_type & T_INT)
    {
      rec = s_node ( Builder.CreateNeg((Value *)y->s_value), y->s_type );
    }
  }
  
  if (rec != NULL) return rec;

  fprintf(stderr, "sem: op1 not implemented\n");
  return ((struct sem_rec *) NULL);
}

Value *CreateOp (const char *op, struct sem_rec *x, struct sem_rec *y, int type)
{
  Value *val = NULL;

  if (type & T_DOUBLE)
  {
    switch (*op)
    {
      case '+':
        val = Builder.CreateFAdd((Value *) x->s_value, (Value *) y->s_value);
        break;
      case '-':
        val = Builder.CreateFSub((Value *) x->s_value, (Value *) y->s_value);
        break;
      case '*':
        val = Builder.CreateFMul((Value *) x->s_value, (Value *) y->s_value);
        break;
      case '/':
        val = Builder.CreateFDiv((Value *) x->s_value, (Value *) y->s_value);
        break;
    }
  }
  else if (type & T_INT)
  {
    switch (*op)
    {
      case '+':
        val = Builder.CreateAdd((Value *) x->s_value, (Value *) y->s_value);
        break;
      case '-':
        val = Builder.CreateSub((Value *) x->s_value, (Value *) y->s_value);
        break;
      case '*':
        val = Builder.CreateMul((Value *) x->s_value, (Value *) y->s_value);
        break;
      case '/':
        val = Builder.CreateSDiv((Value *) x->s_value, (Value *) y->s_value);
        break;
      case '%':
        val = Builder.CreateSRem((Value *) x->s_value, (Value *) y->s_value);
        break;
      case '&':
        val = Builder.CreateAnd((Value *) x->s_value, (Value *) y->s_value);
        break;
      case '|':
        val = Builder.CreateOr((Value *) x->s_value, (Value *) y->s_value);
        break;
      case '^':
        val = Builder.CreateXor((Value *) x->s_value, (Value *) y->s_value);
        break;
      case '<':
        val = Builder.CreateShl((Value *) x->s_value, (Value *) y->s_value);
        break;
      case '>':
        val = Builder.CreateAShr((Value *) x->s_value, (Value *) y->s_value);
        break;
    }
  }

  if (val != NULL) return val;

  fprintf(stderr, "sem: CreateOp error - type %x\n", type);
  return (Value *) NULL;
}

/*
 * op2 - arithmetic operators
 *
 * No LLVM API calls, but most functionality is abstracted to a separate
 * method used by op2, opb, and set.
 *
 * The separate method uses the following API calls:
 * IRBuilder::CreateAdd(Value *, Value *)
 * IRBuilder::CreateFAdd(Value *, Value *)
 * IRBuilder::CreateSub(Value *, Value *)
 * IRBuilder::CreateFSub(Value *, Value *)
 * IRBuilder::CreateMul(Value *, Value *)
 * IRBuilder::CreateFMul(Value *, Value *)
 * IRBuilder::CreateSDiv(Value *, Value *)
 * IRBuilder::CreateFDiv(Value *, Value *)
 * IRBuilder::CreateSRem(Value *, Value *)
 * IRBuilder::CreateAnd(Value *, Value *)
 * IRBuilder::CreateOr(Value *, Value *)
 * IRBuilder::CreateXOr(Value *, Value *)
 * IRBuilder::CreateShl(Value *, Value *)
 * IRBuilder::CreateAShr(Value *, Value *)
 */
struct sem_rec*
op2(const char *op, struct sem_rec *x, struct sem_rec *y)
{
  struct sem_rec *rec = NULL;
  struct sem_rec *a;
  struct sem_rec *b;
  Value *val;

  if (x->s_type & y->s_type)
  {
    a = x;
    b = y;
  }
  else if ((x->s_type & T_DOUBLE) && (y->s_type & T_INT))
  {
    a = x;
    b = cast(y, x->s_type);
  }
  else
  {
    fprintf(stderr, "sem: op2 - invalid argument types\n");
  }

  val = CreateOp(op, a, b, a->s_type);

  rec = s_node( val, x->s_type );
  
  if (rec != NULL) return rec;

  fprintf(stderr, "sem: op2 not implemented\n");
  return NULL;
}

/*
 * opb - bitwise operators
 *
 * No LLVM API calls, but most functionality is abstracted to a separate
 * method used by op2, opb, and set. The comment above op2 lists the LLVM API
 * calls for this method.
 */
struct sem_rec*
opb(const char *op, struct sem_rec *x, struct sem_rec *y)
{
  struct sem_rec *rec = NULL;
  Value *val;

  val = CreateOp(op, x, y, x->s_type);

  rec = s_node(val, x->s_type);

  if (rec != NULL) return rec;

  fprintf(stderr, "sem: opb not implemented\n");
  return ((struct sem_rec *) NULL);
}

/*
 * rel - relational operators
 *
 * Grammar:
 * cexpr -> expr EQ expr          { $$ = rel((char*) "==", $1, $3); }
 * cexpr -> expr NE expr          { $$ = rel((char*) "!=", $1, $3); }
 * cexpr -> expr LE expr          { $$ = rel((char*) "<=", $1, $3); }
 * cexpr -> expr GE expr          { $$ = rel((char*) ">=", $1, $3); }
 * cexpr -> expr LT expr          { $$ = rel((char*) "<",  $1, $3); }
 * cexpr -> expr GT expr          { $$ = rel((char*) ">",  $1, $3); }
 *
 * LLVM API calls:
 * IRBuilder::CreateICmpEq(Value *, Value *)
 * IRBuilder::CreateFCmpOEq(Value *, Value *)
 * IRBuilder::CreateICmpNE(Value *, Value *)
 * IRBuilder::CreateFCmpONE(Value *, Value *)
 * IRBuilder::CreateICmpSLT(Value *, Value *)
 * IRBuilder::CreateFCmpOLT(Value *, Value *)
 * IRBuilder::CreateICmpSLE(Value *, Value *)
 * IRBuilder::CreateFCmpOLE(Value *, Value *)
 * IRBuilder::CreateICmpSGT(Value *, Value *)
 * IRBuilder::CreateFCmpOGT(Value *, Value *)
 * IRBuilder::CreateICmpSGE(Value *, Value *)
 * IRBuilder::CreateFCmpOGE(Value *, Value *)
 */
struct sem_rec*
rel(const char *op, struct sem_rec *x, struct sem_rec *y)
{
  Value *val = NULL;

  int T_RETURN = ((x->s_type & T_INT) && (y->s_type & T_INT)) ? T_INT : T_DOUBLE;

  if (T_RETURN == T_INT)
  {
    if (strcmp(op, "<") == 0)
    {
      val = Builder.CreateICmpSLT( (Value *)x->s_value, (Value *)y->s_value);
    }
    else if (strcmp(op, ">") == 0)
    {
      val = Builder.CreateICmpSGT( (Value *)x->s_value, (Value *)y->s_value);
    }
    else if (strcmp(op, "==") == 0)
    {
      val = Builder.CreateICmpEQ( (Value *)x->s_value, (Value *)y->s_value);
    }
    else if (strcmp(op, ">=") == 0)
    {
      val = Builder.CreateICmpSGE( (Value *)x->s_value, (Value *)y->s_value);
    }
    else if (strcmp(op, "<=") == 0)
    {
      val = Builder.CreateICmpSLE( (Value *)x->s_value, (Value *)y->s_value);
    }
    else if (strcmp(op, "!=") == 0)
    {
      val = Builder.CreateICmpNE( (Value *)x->s_value, (Value *)y->s_value);
    }
  }
  else if (T_RETURN == T_DOUBLE)
  {
    sem_rec *xd;
    sem_rec *yd;
    if (!(x->s_type & T_DOUBLE) && (x->s_type & T_INT))
    {
      xd = cast(x, T_DOUBLE);
      yd = y;
    }
    else if (!(y->s_type & T_DOUBLE) && (y->s_type & T_INT))
    {
      yd = cast(y, T_DOUBLE);
      xd = x;
    }
    else
    {
      xd = x;
      yd = y;
    }

    if (strcmp(op, "<") == 0)
    {
      val = Builder.CreateFCmpOLT( (Value *)xd->s_value, (Value *)yd->s_value);
    }
    else if (strcmp(op, ">") == 0)
    {
      val = Builder.CreateFCmpOGT( (Value *)xd->s_value, (Value *)yd->s_value);
    }
    else if (strcmp(op, "==") == 0)
    {
      val = Builder.CreateFCmpOEQ( (Value *)xd->s_value, (Value *)yd->s_value);
    }
    else if (strcmp(op, ">=") == 0)
    {
      val = Builder.CreateFCmpOGE( (Value *)x->s_value, (Value *)y->s_value);
    }
    else if (strcmp(op, ">=") == 0)
    {
      val = Builder.CreateFCmpOLE( (Value *)x->s_value, (Value *)y->s_value);
    }
    else if (strcmp(op, "!=") == 0)
    {
      val = Builder.CreateFCmpONE( (Value *)x->s_value, (Value *)y->s_value);
    }
  }

  if (val != NULL) return ccexpr( s_node( (void *)val, T_RETURN ) );

  fprintf(stderr, "sem: rel not implemented\n");
  return ((struct sem_rec *) NULL);
}

/*
 * cast - cast value to a different type
 *
 * LLVM API calls:
 * IRBuilder::CreateSIToFP(Value *, Type *)
 * IRBuilder::CreateFPToSI(Value *, Type *)
 * Type::getInt32Ty(LLVMContext &)
 * Type::getDoubleTy(LLVMContext &)
 */
struct sem_rec*
cast (struct sem_rec *y, int t)
{
  sem_rec *rec = NULL;

  if ((t & T_INT) && (y->s_type & T_DOUBLE))
  {
    rec = s_node( Builder.CreateFPToSI((Value *) y->s_value, Type::getInt32Ty(TheContext)), t );
  }
  else if ((t & T_DOUBLE) && (y->s_type & T_INT))
  {
    rec = s_node( Builder.CreateSIToFP((Value *) y->s_value, Type::getDoubleTy(TheContext)), t );
  }

  if (rec != NULL) return rec;

  fprintf(stderr, "sem: cast not implemented\n");
  return ((struct sem_rec *) NULL);
}

/*
 * set - assignment operators
 *
 * Grammar:
 * expr -> lval SET expr         { $$ = set((char*) "",   $1, $3); }
 * expr -> lval SETOR expr       { $$ = set((char*) "|",  $1, $3); }
 * expr -> lval SETXOR expr      { $$ = set((char*) "^",  $1, $3); }
 * expr -> lval SETAND expr      { $$ = set((char*) "&",  $1, $3); }
 * expr -> lval SETLSH expr      { $$ = set((char*) "<<", $1, $3); }
 * expr -> lval SETRSH expr      { $$ = set((char*) ">>", $1, $3); }
 * expr -> lval SETADD expr      { $$ = set((char*) "+",  $1, $3); }
 * expr -> lval SETSUB expr      { $$ = set((char*) "-",  $1, $3); }
 * expr -> lval SETMUL expr      { $$ = set((char*) "*",  $1, $3); }
 * expr -> lval SETDIV expr      { $$ = set((char*) "/",  $1, $3); }
 * expr -> lval SETMOD expr      { $$ = set((char*) "%",  $1, $3); }
 *
 * Much of the functionality in this method is abstracted to a separate method
 * used by op2, opb, and set. The comment above op2 lists the LLVM API calls
 * for this method.
 *
 * Additional LLVM API calls:
 * IRBuilder::CreateLoad(Type, Value *)
 * IRBuilder::CreateStore(Value *, Value *)
 */
struct sem_rec*
set(const char *op, struct sem_rec *x, struct sem_rec *y)
{
  struct sem_rec *rec = NULL;
  struct sem_rec *x_tmp;
  struct sem_rec *a;
  struct sem_rec *b;
  Value *val;

  if (*op != '\0')
  {
    Type *ty = llvm::dyn_cast<PointerType>(((Value*)x->s_value)->getType()->getScalarType())->getElementType();
    x_tmp = s_node( Builder.CreateLoad(ty, (Value *) x->s_value), x->s_type);
  }
  else
  {
    x_tmp = x;
  }


  if (x_tmp->s_type & y->s_type)
  {
    a = x_tmp;
    b = y;
  }
  else if ((x_tmp->s_type & T_DOUBLE) && (y->s_type & T_INT))
  {
    a = x_tmp;
    b = cast(y, x_tmp->s_type);
  }
  else
  {
    fprintf(stderr, "sem: Calc - invalid argument types\n");
  }

  if (*op == '\0')
  {
    val = (Value *) b->s_value;
  }
  else
  {
    val = CreateOp(op, a, b, a->s_type);
  }

  rec = s_node( Builder.CreateStore(val, (Value *)x->s_value), x->s_type );
  
  if (rec != NULL) return rec;

  fprintf(stderr, "sem: set not implemented\n");
  return ((struct sem_rec *) NULL);
}

/*
 * genstring - generate code for a string
 *
 * Grammar:
 * expr ->  STR                   { $$ = genstring($1); }
 *
 * Use parse_escape_chars (in semutil.c) to handle escape characters
 *
 * LLVM API calls:
 * IRBuilder::CreateGlobalStringPtr(char *)
 */
struct sem_rec*
genstring(char *s)
{
  struct sem_rec *rec = NULL;

  rec = s_node( Builder.CreateGlobalStringPtr(parse_escape_chars(s)), T_STR);

  if (rec != NULL) return rec;

  fprintf(stderr, "sem: genstring not implemented\n");
  return (struct sem_rec *) NULL;
}

void
declare_print ()
{
  struct id_entry *entry;
  FunctionType *var_arg;
  Value *F;
  std::string fname = "print";

  /* Add print to our internal data structure */
  var_arg = FunctionType::get(IntegerType::getInt32Ty(TheContext),
                              PointerType::get(Type::getInt8Ty(TheContext), 0), true);
  F = TheModule->getOrInsertFunction(fname, var_arg).getCallee();

  entry = install( slookup(fname.c_str()), 0 );
  entry->i_type = T_INT | T_PROC;
  entry->i_value = (void*) F;
}

void
init_IR ()
{
  TheModule = make_unique<Module>("<stdin>", TheContext);
  declare_print();
}


void
emit_IR ()
{
  TheModule->print(outs(), nullptr);
}
