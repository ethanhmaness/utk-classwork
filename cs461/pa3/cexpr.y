/*
 * This file defines an example yacc grammar for simple C expressions.
 */

%{

#include <stdio.h>
#include <limits.h>
int vars[26] = { 0 };
int dbz_flag = 0;

int IsIntCastable(long l)
{
    return (l <= (long) INT_MAX) && (l >= (long) INT_MIN);
}

int DivOrFlag(long a, long b)
{
   if (b != 0) return a / b;
   else
   {
      dbz_flag = 1;
      return 0;
   }
}

void printIfIntCastable(long l)
{
    if (IsIntCastable(l)) printf("%d\n", (int) l);
    else printf("overflow\n");
}

void printIntOrResetDBZ(long l)
{
   if (dbz_flag)
   {
      printf("dividebyzero\n");
      dbz_flag = 0;
   }
   else printIfIntCastable(l);
}

void varstore(char v, int val)
{
    vars[v - 'a'] = val;
}

int varget(char v)
{
   return vars[v - 'a'];
}

void StoreIfIntCastable(char v, long val)
{
    if (IsIntCastable(val)) varstore(v, val);
}

void vardump()
{
   for (int i = 0; i < 26; i++)
   {
      printf("%c: %d\n", 'a' + i, vars[i]);
   }
}

void varclear()
{
   for (int i = 0; i < 26; i++)
   {
      vars[i] = 0;
   }
}

%}

%union
{
  long num;
  char var;
}

%token <num> NUM, DUMP, CLEAR

%token <var> VAR

%type <num> expr assign_expr or_expr xor_expr and_expr shift_expr addsub_expr mdr_expr neg_expr not_expr paren_expr val_expr

%%

commands  :
   |   commands command
   ;

command	:  DUMP ';'   { vardump(); }
   |  CLEAR ';'       { varclear(); }
   |  expr ';'        { printIntOrResetDBZ($1); }
   ;

expr  :  assign_expr
   ;

assign_expr  :  or_expr
   |  VAR '=' expr          { $$ = $3;
                              StoreIfIntCastable($1, $3); }
   |  VAR '+' '=' expr      { $$ = varget($1) + $4;
                              StoreIfIntCastable($1, varget($1) + $4); }
   |  VAR '-' '=' expr      { $$ = varget($1) - $4;
                              StoreIfIntCastable($1, varget($1) - $4); }
   |  VAR '*' '=' expr      { $$ = varget($1) * $4;
                              StoreIfIntCastable($1, varget($1) * $4); }
   |  VAR '/' '=' expr      { $$ = varget($1) / $4;
                              StoreIfIntCastable($1, varget($1) / $4); }
   |  VAR '%' '=' expr      { $$ = varget($1) % $4;
                              StoreIfIntCastable($1, varget($1) % $4); }
   |  VAR '<' '<' '=' expr  { $$ = varget($1) << $5;
                              StoreIfIntCastable($1, varget($1) << $5); }
   |  VAR '>' '>' '=' expr  { $$ = varget($1) >> $5;
                              StoreIfIntCastable($1, varget($1) >> $5); }
   |  VAR '&' '=' expr      { $$ = varget($1) & $4;
                              StoreIfIntCastable($1, varget($1) & $4); }
   |  VAR '^' '=' expr      { $$ = varget($1) ^ $4;
                              StoreIfIntCastable($1, varget($1) ^ $4); }
   |  VAR '|' '=' expr      { $$ = varget($1) | $4;
                              StoreIfIntCastable($1, varget($1) | $4); }
   ;

or_expr  :  xor_expr
   |  or_expr '|' xor_expr  { $$ = $1 | $3; }
   ;

xor_expr  :  and_expr
   |  xor_expr '^' and_expr  { $$ = $1 ^ $3; }
   ;

and_expr  :  shift_expr
   |  and_expr '&' shift_expr  { $$ = $1 & $3; }
   ;

shift_expr  :  addsub_expr
   |  shift_expr '<' '<' addsub_expr  { $$ = $1 << $4; }
   |  shift_expr '>' '>' addsub_expr  { $$ = $1 >> $4; }
   ;

addsub_expr  :  mdr_expr
   |  addsub_expr '+' mdr_expr  { $$ = $1 + $3; }
   |  addsub_expr '-' mdr_expr  { $$ = $1 - $3; }
   ;

mdr_expr  :  neg_expr
   |  mdr_expr '*' neg_expr  { $$ = $1 * $3; }
   |  mdr_expr '/' neg_expr  { $$ = DivOrFlag($1, $3); }
   |  mdr_expr '%' neg_expr  { $$ = $1 % $3; }
   ;

neg_expr  :  not_expr
   |  '-' neg_expr  { $$ = -$2; }
   ;

not_expr  :  paren_expr
   |  '~' not_expr  { $$ = ~$2; }
   ;

paren_expr  :  val_expr
   |  '(' expr ')'  { $$ = $2; }
   ;

val_expr  :  NUM  { $$ = $1; }
   |  VAR         { $$ = (long) vars[$1 - 'a']; }
   ;

%%

main()
{
   if (yyparse())
      printf("\nInvalid expression.\n");
   else
      printf("\nCalculator off.\n");
}

yyerror(s)
char *s;
{
   fprintf(stderr, "%s\n", s);
}
