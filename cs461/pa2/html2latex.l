%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * 1 - Ordered list
 * 2 - Unordered list
 */
int list_type;
%}

%option       noyywrap nounput yylineno
%x            COMMENT PARA PRE OL UL LI

%%

"<h1>"[^<]*"</h1>" {
    /**************************************************************************
     * Matched an HTML H1 header so print it out as a LateX section title.
     * The beginning and ending tags need to be stripped off before the
     * title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-5] = 0;
    printf( "\\section{%s}", yytext+4 );
}

"<h2>"[^<]*"</h2>" {
    /**************************************************************************
     * Matched an HTML H2 header so print it out as a LateX subsection title.
     * The beginning and ending tags need to be stripped off before the
     * title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-5] = 0;
    printf( "\\subsection{%s}", yytext+4 );
}

"<h3>"[^<]*"</h3>" {
    /**************************************************************************
     * Matched an HTML H3 header so print it out as a LateX subsubsection title.
     * The beginning and ending tags need to be stripped off before the
     * title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-5] = 0;
    printf( "\\subsubsection{%s}", yytext+4 );
}

"<!--" {
    /**************************************************************************
     * Matched an HTML comment so print it out as a LateX comment. The
     * beginning and ending tags are stripped off before the LateX comment
     * is emitted.
     *************************************************************************/
    printf( "%% " );
    BEGIN COMMENT;
}

"<pre>" {
    /**************************************************************************
     * Matched an HTML preformatted tag so print it out as a LateX verbatim
     * environment. The beginning and ending tags are stripped off before the 
     * LateX environment is emitted.
     *************************************************************************/
    printf( "\\begin{verbatim}" );
    BEGIN PRE;
}

"<p>" {
    /**************************************************************************
     * Matched an HTML paragraph tag so print it out as a LateX paragraph.
     * The beginning and ending tags are stripped off before the 
     * LateX paragraph is emitted.
     *************************************************************************/
    BEGIN PARA;
}

"<ol>" {
    /**************************************************************************
     * Matched an HTML ordered list tag so print it out as a LateX itemized
     * environment. The beginning and ending tags are stripped off before the 
     * LateX environment is emitted.
     *************************************************************************/
    printf( "\\begin{enumerate}" );
    list_type = 1;
    BEGIN OL;
}

"<ul>" {
    /**************************************************************************
     * Matched an HTML ordered list tag so print it out as a LateX itemized
     * environment. The beginning and ending tags are stripped off before the 
     * LateX environment is emitted.
     *************************************************************************/
    printf( "\\begin{itemize}" );
    list_type = 2;
    BEGIN UL;
}

[ \t\n\r]+ {
    /**************************************************************************
     * Matched some whitespace that is not inside of an HTML tag. We
     * just ignore the whitespace.
     *************************************************************************/
}

. {
    /**************************************************************************
     * Matched something that we did not expect to see. Issue an error and
     * exit the program with a failure code.
     *************************************************************************/
    fprintf( stderr, "Unexpected input at line %d: '%s'\n", yylineno, yytext );
    exit( 1 );
}

<COMMENT>"-->" {
    /**************************************************************************
     * Matched an HTML end comment while parsing an HTML comment. Print out
     * a new line and resume normal parsing.
     *************************************************************************/
    printf( "\n" );
    BEGIN 0;
}

<COMMENT>[\n](\r)? {
    /**************************************************************************
     * Matched a newline while parsing an HTML comment. Print out a new
     * line making sure to mark the next line as a comment as well.
     *************************************************************************/
    printf( "\n%% " );
}

<COMMENT>[ \t] {
    /**************************************************************************
     * Matched some white space while parsing an HTML comment. Print out
     * the white space as is.
     *************************************************************************/
    printf( "%s", yytext );
}

<COMMENT>. {
    /**************************************************************************
     * Matched any other text while parsing an HTML comment. Print out
     * the text as is.
     *************************************************************************/
    printf( "%s", yytext );
}

<PRE>"</pre>" {
    /**************************************************************************
     * Matched an HTML end preformatted tag while parsing preformatted text.
     * Print out a verbatim end command and resume normal parsing.
     *************************************************************************/
    printf( "\\end{verbatim}\n" );
    BEGIN 0;
}

<PRE>. {
    /**************************************************************************
     * Matched any other text while parsing an HTML preformatted tag. 
     * Print out the text as is.
     *************************************************************************/
    printf( "%s", yytext );
}

<PARA>"</p>" {
    /**************************************************************************
     * Matched an HTML end paragraph tag while parsing an HTML paragraph.
     * Print out double newline to end paragraph and resume normal parsing.
     *************************************************************************/
    printf("\n\n");
    BEGIN 0;
}

<PARA>"<small>"[^<]*"</small>" {
    /**************************************************************************
     * Matched an HTML small tag so print it out as a LateX script size
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-8] = 0;
    printf( "{\\scriptsize %s}", yytext+7 );
}

<PARA>"<big>"[^<]*"</big>" {
    /**************************************************************************
     * Matched an HTML big tag so print it out as a LateX large
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-6] = 0;
    printf( "{\\Large %s}", yytext+5 );
}

<PARA>"<b>"[^<]*"</b>" {
    /**************************************************************************
     * Matched an HTML b tag so print it out as a LateX textbf
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-4] = 0;
    printf( "\\textbf{%s}", yytext+3 );
}

<PARA>"<i>"[^<]*"</i>" {
    /**************************************************************************
     * Matched an HTML i tag so print it out as a LateX textit
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-4] = 0;
    printf( "\\textit{%s}", yytext+3 );
}

<PARA>"<strong>"[^<]*"</strong>" {
    /**************************************************************************
     * Matched an HTML strong tag so print it out as a LateX textmd
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-9] = 0;
    printf( "\\textmd{%s}", yytext+8 );
}

<PARA>"<em>"[^<]*"</em>" {
    /**************************************************************************
     * Matched an HTML em tag so print it out as a LateX emph
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-5] = 0;
    printf( "\\emph{%s}", yytext+4 );
}

<PARA>"<sup>"[^<]*"</sup>" {
    /**************************************************************************
     * Matched an HTML sup tag so print it out as a LateX textsuperscript
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-6] = 0;
    printf( "\\textsuperscript{%s}", yytext+5 );
}

<PARA>"<sub>"[^<]*"</sub>" {
    /**************************************************************************
     * Matched an HTML sub tag so print it out as a LateX textsubscript
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-6] = 0;
    printf( "\\textsubscript{%s}", yytext+5 );
}

<PARA>. {
    /**************************************************************************
     * Matched any other text while parsing an HTML paragraph tag. 
     * Print out the text as is.
     *************************************************************************/
    printf( "%s", yytext );
}

<OL>"</ol>" {
    /**************************************************************************
     * Matched an HTML end ordered list tag while parsing unordered list.
     * Print out an itemize end command and resume normal parsing.
     *************************************************************************/
    printf( "\\end{enumerate}\n");
    BEGIN 0;
}

<OL>"<li>" {
    /**************************************************************************
     * Matched an HTML list item tag so print it out as a LateX item element.
     * The beginning and ending tags are stripped off before the LateX
     * element is emitted.
     *************************************************************************/
    printf( "\\item " );
    BEGIN LI;
}

<UL>"</ul>" {
    /**************************************************************************
     * Matched an HTML end ordered list tag while parsing unordered list.
     * Print out an itemize end command and resume normal parsing.
     *************************************************************************/
    printf( "\\end{itemize}\n");
    BEGIN 0;
}

<UL>"<li>" {
    /**************************************************************************
     * Matched an HTML list item tag so print it out as a LateX item element.
     * The beginning and ending tags are stripped off before the LateX
     * element is emitted.
     *************************************************************************/
    printf( "\\item " );
    BEGIN LI;
}

<LI>"</li>" {
    /**************************************************************************
     * Matched an HTML end list item tag while parsing a list item.
     * Print out newline to end item and resume list parsing.
     *************************************************************************/
    printf( "\n" );
    if (list_type == 1)
    {
        BEGIN OL;
    }
    else if (list_type == 2)
    {
        BEGIN UL;
    }
    else
    {
        fprintf( stderr, "Unexpected input at line %d: '%s'\n", yylineno, yytext );
        exit( 1 );
    }
}

<LI>"<small>"[^<]*"</small>" {
    /**************************************************************************
     * Matched an HTML small tag so print it out as a LateX script size
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-8] = 0;
    printf( "{\\scriptsize %s}", yytext+7 );
}

<LI>"<big>"[^<]*"</big>" {
    /**************************************************************************
     * Matched an HTML big tag so print it out as a LateX large
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-6] = 0;
    printf( "{\\Large %s}", yytext+5 );
}

<LI>"<b>"[^<]*"</b>" {
    /**************************************************************************
     * Matched an HTML b tag so print it out as a LateX textbf
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-4] = 0;
    printf( "\\textbf{%s}", yytext+3 );
}

<LI>"<i>"[^<]*"</i>" {
    /**************************************************************************
     * Matched an HTML i tag so print it out as a LateX textit
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-4] = 0;
    printf( "\\textit{%s}", yytext+3 );
}

<LI>"<strong>"[^<]*"</strong>" {
    /**************************************************************************
     * Matched an HTML strong tag so print it out as a LateX textmd
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-9] = 0;
    printf( "\\textmd{%s}", yytext+8 );
}

<LI>"<em>"[^<]*"</em>" {
    /**************************************************************************
     * Matched an HTML em tag so print it out as a LateX emph
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-5] = 0;
    printf( "\\emph{%s}", yytext+4 );
}

<LI>"<sup>"[^<]*"</sup>" {
    /**************************************************************************
     * Matched an HTML sup tag so print it out as a LateX textsuperscript
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-6] = 0;
    printf( "\\textsuperscript{%s}", yytext+5 );
}

<LI>"<sub>"[^<]*"</sub>" {
    /**************************************************************************
     * Matched an HTML sub tag so print it out as a LateX textsubscript
     * element. The beginning and ending tags need to be stripped off before
     * the title can be formatted in LateX.
     *************************************************************************/
    yytext[yyleng-6] = 0;
    printf( "\\textsubscript{%s}", yytext+5 );
}

<LI>. {
    /**************************************************************************
     * Matched any other text while parsing an HTML paragraph tag. 
     * Print out the text as is.
     *************************************************************************/
    printf( "%s", yytext );
}

%%

/**************************************************************************
 * The main function for the HTML to LateX converter. Here we do three
 * things:
 *  1. Print out the LateX document header.
 *  2. Invoke the lexer to print out the LateX document
 *  3. Print out the LateX footer.
 *************************************************************************/
int main( int argc, const char *argv[] ) {
    printf( "\\documentclass[12pt]{article}\n" );
    printf( "\\usepackage{fixltx2e}\n" );
    printf( "\n" );
    printf( "\\setlength{\\parindent}{0.0in}\n" );
    printf( "\\setlength{\\parskip}{0.125in}\n" );
    printf( "\n\n" );
    printf( "\\begin{document}\n" );
    yylex();
    printf( "\\end{document}\n" );
    return 0;
}
