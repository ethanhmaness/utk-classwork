%{

#include "y.tab.h"

%}

%%

[0-9]+      { sscanf(yytext, "%ld", &yylval.num);
            return NUM; }

[a-z]       { sscanf(yytext, "%c", &yylval.var);
            return VAR; }

[ \n\t\r]+  { }

"dump"      { return DUMP; }

"clear"     { return CLEAR; }

.           { return yytext[0]; }
