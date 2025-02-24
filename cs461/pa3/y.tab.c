/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20170709

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#define YYPREFIX "yy"

#define YYPURE 0

#line 6 "cexpr.y"

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

#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 76 "cexpr.y"
typedef union
{
  long num;
  char var;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 103 "y.tab.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define NUM 257
#define DUMP 258
#define CLEAR 259
#define VAR 260
#define YYERRCODE 256
typedef short YYINT;
static const YYINT yylhs[] = {                           -1,
    0,    0,   13,   13,   13,    1,    2,    2,    2,    2,
    2,    2,    2,    2,    2,    2,    2,    2,    3,    3,
    4,    4,    5,    5,    6,    6,    6,    7,    7,    7,
    8,    8,    8,    8,    9,    9,   10,   10,   11,   11,
   12,   12,
};
static const YYINT yylen[] = {                            2,
    0,    2,    2,    2,    2,    1,    1,    3,    4,    4,
    4,    4,    4,    5,    5,    4,    4,    4,    1,    3,
    1,    3,    1,    3,    1,    4,    4,    1,    3,    3,
    1,    3,    3,    3,    1,    2,    1,    2,    1,    3,
    1,    1,
};
static const YYINT yydefred[] = {                         1,
    0,   41,    0,    0,    0,    0,    0,    0,    0,    6,
    0,    0,    0,    0,    0,    0,   31,   35,   37,   39,
    2,    3,    4,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   42,   36,   38,    0,    5,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    8,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   40,    0,    0,    0,    0,    0,    0,    0,   32,   33,
   34,    9,   10,   11,   12,   13,    0,    0,   16,   17,
   18,    0,    0,   14,   15,
};
static const YYINT yydgoto[] = {                          1,
    9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
   19,   20,   21,
};
static const YYINT yysindex[] = {                         0,
  -40,    0,  -57,  -52,  -25,  -36,  -32,  -34,  -49,    0,
 -105,  -71,  -13,  -59,  -29,   87,    0,    0,    0,    0,
    0,    0,    0,  -34,  -30,  -22,  -21,   -9,   -7,    6,
   11,   32,   34,   47,    0,    0,    0,   68,    0,  -36,
  -36,  -36,   50,   49,  -36,  -36,  -36,  -36,  -36,    0,
  -34,  -34,  -34,  -34,  -34,   54,   61,  -34,  -34,  -34,
    0,  -71,  -13,  -59,  -36,  -36,   87,   87,    0,    0,
    0,    0,    0,    0,    0,    0,  -34,  -34,    0,    0,
    0,  -29,  -29,    0,    0,
};
static const YYINT yyrindex[] = {                         0,
    0,    0,    0,    0,    5,    0,    0,    0,    0,    0,
   46,  -12,  -26,   37,    3,  -17,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   -3,   44,   43,    0,    0,  -11,   12,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,   20,   29,    0,    0,
};
static const YYINT yygindex[] = {                         0,
   96,    0,    0,   76,   84,   86,   -6,   55,   70,  119,
    0,    0,    0,
};
#define YYTABLESIZE 228
static const YYINT yytable[] = {                          8,
   43,   22,   44,    8,    6,    8,   23,    8,    6,   39,
    6,   29,   32,   45,   21,   46,   27,   25,   40,   26,
   28,   28,   41,   28,   42,   28,   29,   28,   19,   29,
   51,   29,   21,   29,   30,   24,   31,   20,   52,   53,
   25,   28,   28,   25,   28,   42,   19,   29,   29,   30,
   29,   54,   30,   55,   30,   20,   30,   26,   82,   83,
   26,   25,   25,   42,   25,   56,   27,   21,   33,   27,
   30,   30,   57,   30,   23,   36,   28,   23,   26,   26,
   24,   26,   29,   24,   22,    7,    7,   27,   27,    7,
   27,    7,   58,    7,   59,   23,   25,   21,   34,   67,
   68,   24,   22,   38,    7,   30,   28,   60,   61,   65,
   66,   19,   29,   26,   77,   62,   69,   70,   71,   50,
   20,   78,   27,   49,   63,   37,   25,   64,   47,    0,
   23,    0,    0,   48,    0,   30,   24,   22,    0,    0,
    0,    0,    0,   26,    0,    0,   72,   73,   74,   75,
   76,    0,   27,   79,   80,   81,    0,    0,    0,    0,
   23,    0,    0,    0,    0,    0,   24,   22,    0,    0,
    0,    0,   84,   85,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    2,    3,    4,    5,
    2,    0,    2,   35,    2,    5,    0,   35,
};
static const YYINT yycheck[] = {                         40,
   60,   59,   62,   40,   45,   40,   59,   40,   45,   59,
   45,   37,   38,   43,   41,   45,   42,   43,  124,   45,
   38,   47,   94,   41,   38,   43,   38,   45,   41,   41,
   61,   43,   59,   45,   60,   61,   62,   41,   61,   61,
   38,   59,   60,   41,   62,   41,   59,   59,   60,   38,
   62,   61,   41,   61,   43,   59,   45,   38,   65,   66,
   41,   59,   60,   59,   62,   60,   38,   94,   94,   41,
   59,   60,   62,   62,   38,    6,   94,   41,   59,   60,
   38,   62,   94,   41,   41,  126,   41,   59,   60,  126,
   62,  126,   61,  126,   61,   59,   94,  124,  124,   45,
   46,   59,   59,    8,   59,   94,  124,   61,   41,   60,
   62,  124,  124,   94,   61,   40,   47,   48,   49,   24,
  124,   61,   94,   37,   41,    7,  124,   42,   42,   -1,
   94,   -1,   -1,   47,   -1,  124,   94,   94,   -1,   -1,
   -1,   -1,   -1,  124,   -1,   -1,   51,   52,   53,   54,
   55,   -1,  124,   58,   59,   60,   -1,   -1,   -1,   -1,
  124,   -1,   -1,   -1,   -1,   -1,  124,  124,   -1,   -1,
   -1,   -1,   77,   78,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,  259,  260,
  257,   -1,  257,  260,  257,  260,   -1,  260,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 260
#define YYUNDFTOKEN 276
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,"'%'","'&'",0,"'('","')'","'*'","'+'",0,"'-'",0,"'/'",0,0,0,0,0,0,0,0,0,0,
0,"';'","'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,"'^'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'|'",0,
"'~'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,"NUM","DUMP","CLEAR","VAR",0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,"illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : commands",
"commands :",
"commands : commands command",
"command : DUMP ';'",
"command : CLEAR ';'",
"command : expr ';'",
"expr : assign_expr",
"assign_expr : or_expr",
"assign_expr : VAR '=' expr",
"assign_expr : VAR '+' '=' expr",
"assign_expr : VAR '-' '=' expr",
"assign_expr : VAR '*' '=' expr",
"assign_expr : VAR '/' '=' expr",
"assign_expr : VAR '%' '=' expr",
"assign_expr : VAR '<' '<' '=' expr",
"assign_expr : VAR '>' '>' '=' expr",
"assign_expr : VAR '&' '=' expr",
"assign_expr : VAR '^' '=' expr",
"assign_expr : VAR '|' '=' expr",
"or_expr : xor_expr",
"or_expr : or_expr '|' xor_expr",
"xor_expr : and_expr",
"xor_expr : xor_expr '^' and_expr",
"and_expr : shift_expr",
"and_expr : and_expr '&' shift_expr",
"shift_expr : addsub_expr",
"shift_expr : shift_expr '<' '<' addsub_expr",
"shift_expr : shift_expr '>' '>' addsub_expr",
"addsub_expr : mdr_expr",
"addsub_expr : addsub_expr '+' mdr_expr",
"addsub_expr : addsub_expr '-' mdr_expr",
"mdr_expr : neg_expr",
"mdr_expr : mdr_expr '*' neg_expr",
"mdr_expr : mdr_expr '/' neg_expr",
"mdr_expr : mdr_expr '%' neg_expr",
"neg_expr : not_expr",
"neg_expr : '-' neg_expr",
"not_expr : paren_expr",
"not_expr : '~' not_expr",
"paren_expr : val_expr",
"paren_expr : '(' expr ')'",
"val_expr : NUM",
"val_expr : VAR",

};
#endif

int      yydebug;
int      yynerrs;

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#define YYINITSTACKSIZE 200

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
/* variables for the parser stack */
static YYSTACKDATA yystack;
#line 172 "cexpr.y"

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
#line 364 "y.tab.c"

#if YYDEBUG
#include <stdio.h>	/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL()
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yym = 0;
    yyn = 0;
    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        yychar = YYLEX;
        if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if (((yyn = yysindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if (((yyn = yyrindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag != 0) goto yyinrecovery;

    YYERROR_CALL("syntax error");

    goto yyerrlab; /* redundant goto avoids 'unused label' warning */
yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if (((yyn = yysindex[*yystack.s_mark]) != 0) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym > 0)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);

    switch (yyn)
    {
case 3:
#line 94 "cexpr.y"
	{ vardump(); }
break;
case 4:
#line 95 "cexpr.y"
	{ varclear(); }
break;
case 5:
#line 96 "cexpr.y"
	{ printIntOrResetDBZ(yystack.l_mark[-1].num); }
break;
case 8:
#line 103 "cexpr.y"
	{ yyval.num = yystack.l_mark[0].num;
                              StoreIfIntCastable(yystack.l_mark[-2].var, yystack.l_mark[0].num); }
break;
case 9:
#line 105 "cexpr.y"
	{ yyval.num = varget(yystack.l_mark[-3].var) + yystack.l_mark[0].num;
                              StoreIfIntCastable(yystack.l_mark[-3].var, varget(yystack.l_mark[-3].var) + yystack.l_mark[0].num); }
break;
case 10:
#line 107 "cexpr.y"
	{ yyval.num = varget(yystack.l_mark[-3].var) - yystack.l_mark[0].num;
                              StoreIfIntCastable(yystack.l_mark[-3].var, varget(yystack.l_mark[-3].var) - yystack.l_mark[0].num); }
break;
case 11:
#line 109 "cexpr.y"
	{ yyval.num = varget(yystack.l_mark[-3].var) * yystack.l_mark[0].num;
                              StoreIfIntCastable(yystack.l_mark[-3].var, varget(yystack.l_mark[-3].var) * yystack.l_mark[0].num); }
break;
case 12:
#line 111 "cexpr.y"
	{ yyval.num = varget(yystack.l_mark[-3].var) / yystack.l_mark[0].num;
                              StoreIfIntCastable(yystack.l_mark[-3].var, varget(yystack.l_mark[-3].var) / yystack.l_mark[0].num); }
break;
case 13:
#line 113 "cexpr.y"
	{ yyval.num = varget(yystack.l_mark[-3].var) % yystack.l_mark[0].num;
                              StoreIfIntCastable(yystack.l_mark[-3].var, varget(yystack.l_mark[-3].var) % yystack.l_mark[0].num); }
break;
case 14:
#line 115 "cexpr.y"
	{ yyval.num = varget(yystack.l_mark[-4].var) << yystack.l_mark[0].num;
                              StoreIfIntCastable(yystack.l_mark[-4].var, varget(yystack.l_mark[-4].var) << yystack.l_mark[0].num); }
break;
case 15:
#line 117 "cexpr.y"
	{ yyval.num = varget(yystack.l_mark[-4].var) >> yystack.l_mark[0].num;
                              StoreIfIntCastable(yystack.l_mark[-4].var, varget(yystack.l_mark[-4].var) >> yystack.l_mark[0].num); }
break;
case 16:
#line 119 "cexpr.y"
	{ yyval.num = varget(yystack.l_mark[-3].var) & yystack.l_mark[0].num;
                              StoreIfIntCastable(yystack.l_mark[-3].var, varget(yystack.l_mark[-3].var) & yystack.l_mark[0].num); }
break;
case 17:
#line 121 "cexpr.y"
	{ yyval.num = varget(yystack.l_mark[-3].var) ^ yystack.l_mark[0].num;
                              StoreIfIntCastable(yystack.l_mark[-3].var, varget(yystack.l_mark[-3].var) ^ yystack.l_mark[0].num); }
break;
case 18:
#line 123 "cexpr.y"
	{ yyval.num = varget(yystack.l_mark[-3].var) | yystack.l_mark[0].num;
                              StoreIfIntCastable(yystack.l_mark[-3].var, varget(yystack.l_mark[-3].var) | yystack.l_mark[0].num); }
break;
case 20:
#line 128 "cexpr.y"
	{ yyval.num = yystack.l_mark[-2].num | yystack.l_mark[0].num; }
break;
case 22:
#line 132 "cexpr.y"
	{ yyval.num = yystack.l_mark[-2].num ^ yystack.l_mark[0].num; }
break;
case 24:
#line 136 "cexpr.y"
	{ yyval.num = yystack.l_mark[-2].num & yystack.l_mark[0].num; }
break;
case 26:
#line 140 "cexpr.y"
	{ yyval.num = yystack.l_mark[-3].num << yystack.l_mark[0].num; }
break;
case 27:
#line 141 "cexpr.y"
	{ yyval.num = yystack.l_mark[-3].num >> yystack.l_mark[0].num; }
break;
case 29:
#line 145 "cexpr.y"
	{ yyval.num = yystack.l_mark[-2].num + yystack.l_mark[0].num; }
break;
case 30:
#line 146 "cexpr.y"
	{ yyval.num = yystack.l_mark[-2].num - yystack.l_mark[0].num; }
break;
case 32:
#line 150 "cexpr.y"
	{ yyval.num = yystack.l_mark[-2].num * yystack.l_mark[0].num; }
break;
case 33:
#line 151 "cexpr.y"
	{ yyval.num = DivOrFlag(yystack.l_mark[-2].num, yystack.l_mark[0].num); }
break;
case 34:
#line 152 "cexpr.y"
	{ yyval.num = yystack.l_mark[-2].num % yystack.l_mark[0].num; }
break;
case 36:
#line 156 "cexpr.y"
	{ yyval.num = -yystack.l_mark[0].num; }
break;
case 38:
#line 160 "cexpr.y"
	{ yyval.num = ~yystack.l_mark[0].num; }
break;
case 40:
#line 164 "cexpr.y"
	{ yyval.num = yystack.l_mark[-1].num; }
break;
case 41:
#line 167 "cexpr.y"
	{ yyval.num = yystack.l_mark[0].num; }
break;
case 42:
#line 168 "cexpr.y"
	{ yyval.num = (long) vars[yystack.l_mark[0].var - 'a']; }
break;
#line 690 "y.tab.c"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            yychar = YYLEX;
            if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if (((yyn = yygindex[yym]) != 0) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    YYERROR_CALL("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}
