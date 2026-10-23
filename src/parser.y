
%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "tabla_simbolos.h"
#include "graficos.h"

int yylex(void);
void yyerror(const char *s);
extern int yylineno;

static int error_sintactico = 0;
%}

%code requires {
    #include "ast.h"
}

%union {
    double numero;
    char *cadena;
    Expresion *expr;
    Instruccion *inst;
}

%token <numero> NUMERO
%token <cadena> IDENTIFICADOR CADENA

%token FORWARD ROTATE PENCOLOR PENUP PENDOWN REPEAT IF VAR
%token IGUAL_IGUAL MAYOR MENOR ASIGNACION
%token SUMA RESTA MULTIPLICACION DIVISION
%token PARENTESIS_IZQ PARENTESIS_DER
%token LLAVE_IZQ LLAVE_DER PUNTO_COMA COMA
%token CARACTER_INVALIDO

%type <expr> expresion condicion
%type <inst> instrucciones instruccion bloque

%left SUMA RESTA
%left MULTIPLICACION DIVISION
%right MENOS_UNARIO

%start programa

%%
programa:
    instrucciones {
        inicializar_tabla();

        if (!iniciar_grafico("salida.svg")) {
            liberar_programa($1);
            YYABORT;
        }

        int correcto = ejecutar_programa($1);

        finalizar_grafico();

        if (correcto) {
            imprimir_tabla();
            printf("Archivo generado: salida.svg\n");
        }

        liberar_programa($1);

        if (!correcto) {
            fprintf(stderr, "No se pudo ejecutar el programa\n");
            YYABORT;
        }
    }
    ;

instrucciones:
    %empty {
        $$ = NULL;
    }
    | instrucciones instruccion {
        $$ = agregar_instruccion($1, $2);
    }
    ;

instruccion:
    VAR IDENTIFICADOR ASIGNACION expresion {
        $$ = crear_instruccion(STMT_DECLARACION);
        $$->nombre = $2;
        $$->expresion = $4;
    }
    | VAR IDENTIFICADOR ASIGNACION expresion PUNTO_COMA {
        $$ = crear_instruccion(STMT_DECLARACION);
        $$->nombre = $2;
        $$->expresion = $4;
    }
    | FORWARD PARENTESIS_IZQ expresion PARENTESIS_DER {
        $$ = crear_instruccion(STMT_FORWARD);
        $$->expresion = $3;
    }
    | FORWARD PARENTESIS_IZQ expresion PARENTESIS_DER PUNTO_COMA {
        $$ = crear_instruccion(STMT_FORWARD);
        $$->expresion = $3;
    }
    | ROTATE PARENTESIS_IZQ expresion PARENTESIS_DER {
        $$ = crear_instruccion(STMT_ROTATE);
        $$->expresion = $3;
    }
    | ROTATE PARENTESIS_IZQ expresion PARENTESIS_DER PUNTO_COMA {
        $$ = crear_instruccion(STMT_ROTATE);
        $$->expresion = $3;
    }
    | PENCOLOR PARENTESIS_IZQ CADENA PARENTESIS_DER {
        $$ = crear_instruccion(STMT_COLOR);
        $$->cadena = $3;
    }
    | PENCOLOR PARENTESIS_IZQ CADENA PARENTESIS_DER PUNTO_COMA {
        $$ = crear_instruccion(STMT_COLOR);
        $$->cadena = $3;
    }
    | PENUP PARENTESIS_IZQ PARENTESIS_DER {
        $$ = crear_instruccion(STMT_PEN_UP);
    }
    | PENUP PARENTESIS_IZQ PARENTESIS_DER PUNTO_COMA {
        $$ = crear_instruccion(STMT_PEN_UP);
    }
    | PENDOWN PARENTESIS_IZQ PARENTESIS_DER {
        $$ = crear_instruccion(STMT_PEN_DOWN);
    }
    | PENDOWN PARENTESIS_IZQ PARENTESIS_DER PUNTO_COMA {
        $$ = crear_instruccion(STMT_PEN_DOWN);
    }
    | REPEAT PARENTESIS_IZQ expresion PARENTESIS_DER bloque {
        $$ = crear_instruccion(STMT_REPEAT);
        $$->expresion = $3;
        $$->cuerpo = $5;
    }
    | IF PARENTESIS_IZQ condicion PARENTESIS_DER bloque {
        $$ = crear_instruccion(STMT_IF);
        $$->condicion = $3;
        $$->cuerpo = $5;
    }
    | CARACTER_INVALIDO {
        yyerror("caracter no valido");
        YYABORT;
    }
    ;

bloque:
    LLAVE_IZQ instrucciones LLAVE_DER {
        $$ = $2;
    }
    ;

condicion:
    expresion MAYOR expresion {
        $$ = crear_binaria(OP_MAYOR, $1, $3);
    }
    | expresion MENOR expresion {
        $$ = crear_binaria(OP_MENOR, $1, $3);
    }
    | expresion IGUAL_IGUAL expresion {
        $$ = crear_binaria(OP_IGUAL, $1, $3);
    }
    ;

expresion:
    NUMERO {
        $$ = crear_numero($1);
    }
    | IDENTIFICADOR {
        $$ = crear_variable($1);
    }
    | expresion SUMA expresion {
        $$ = crear_binaria(OP_SUMA, $1, $3);
    }
    | expresion RESTA expresion {
        $$ = crear_binaria(OP_RESTA, $1, $3);
    }
    | expresion MULTIPLICACION expresion {
        $$ = crear_binaria(OP_MULTIPLICACION, $1, $3);
    }
    | expresion DIVISION expresion {
        $$ = crear_binaria(OP_DIVISION, $1, $3);
    }
    | RESTA expresion %prec MENOS_UNARIO {
        $$ = crear_negativa($2);
    }
    | PARENTESIS_IZQ expresion PARENTESIS_DER {
        $$ = $2;
    }
    ;
%%

void yyerror(const char *s) {
    error_sintactico = 1;

    fprintf(
        stderr,
        "Error sintactico en linea %d: %s\n",
        yylineno,
        s
    );
}

int main(void) {
    printf("GraphScript: analizando programa\n");

    if (yyparse() == 0 && !error_sintactico) {
        printf("Analisis finalizado correctamente\n");
        return 0;
    }

    fprintf(stderr, "No se pudo ejecutar el programa\n");
    return 1;
}
