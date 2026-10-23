
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"
%}

%option noyywrap yylineno noinput nounput

DIGITO      [0-9]
NUM         {DIGITO}+(\.{DIGITO}+)?
ID          [a-zA-Z_][a-zA-Z0-9_]*
CADENA      \"([^\"\n]|\\.)*\"

%%
"forward"       { return FORWARD; }
"rotate"        { return ROTATE; }
"penColor"      { return PENCOLOR; }
"penUp"         { return PENUP; }
"penDown"       { return PENDOWN; }
"repeat"        { return REPEAT; }
"if"            { return IF; }
"var"           { return VAR; }

"=="            { return IGUAL_IGUAL; }
">"             { return MAYOR; }
"<"             { return MENOR; }
"="             { return ASIGNACION; }

"+"             { return SUMA; }
"-"             { return RESTA; }
"*"             { return MULTIPLICACION; }
"/"             { return DIVISION; }

"("             { return PARENTESIS_IZQ; }
")"             { return PARENTESIS_DER; }
"{"             { return LLAVE_IZQ; }
"}"             { return LLAVE_DER; }
";"             { return PUNTO_COMA; }
","             { return COMA; }

{NUM} {
    yylval.numero = atof(yytext);
    return NUMERO;
}

{CADENA} {
    yylval.cadena = strdup(yytext);
    return CADENA;
}

{ID} {
    yylval.cadena = strdup(yytext);
    return IDENTIFICADOR;
}

"//"[^\n]*      { /* Ignorar comentarios */ }
[ \t\r]+        { /* Ignorar espacios */ }
\n              { /* yylineno cuenta las lineas */ }

. {
    fprintf(
        stderr,
        "Error lexico en linea %d: caracter no reconocido '%s'\n",
        yylineno,
        yytext
    );

    return CARACTER_INVALIDO;
}
%%
