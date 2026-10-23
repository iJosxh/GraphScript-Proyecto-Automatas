
#ifndef AST_H
#define AST_H

typedef enum {
    EXPR_NUMERO,
    EXPR_VARIABLE,
    EXPR_BINARIA,
    EXPR_NEGATIVA
} TipoExpresion;

typedef enum {
    OP_SUMA,
    OP_RESTA,
    OP_MULTIPLICACION,
    OP_DIVISION,
    OP_MAYOR,
    OP_MENOR,
    OP_IGUAL
} Operador;

typedef struct Expresion {
    TipoExpresion tipo;
    double numero;
    char *nombre;
    Operador operador;
    struct Expresion *izquierda;
    struct Expresion *derecha;
} Expresion;

typedef enum {
    STMT_DECLARACION,
    STMT_FORWARD,
    STMT_ROTATE,
    STMT_COLOR,
    STMT_PEN_UP,
    STMT_PEN_DOWN,
    STMT_REPEAT,
    STMT_IF
} TipoInstruccion;

typedef struct Instruccion {
    TipoInstruccion tipo;
    char *nombre;
    char *cadena;
    Expresion *expresion;
    Expresion *condicion;
    struct Instruccion *cuerpo;
    struct Instruccion *siguiente;
} Instruccion;

Expresion *crear_numero(double valor);
Expresion *crear_variable(char *nombre);
Expresion *crear_binaria(Operador op, Expresion *izq, Expresion *der);
Expresion *crear_negativa(Expresion *expr);

Instruccion *crear_instruccion(TipoInstruccion tipo);
Instruccion *agregar_instruccion(
    Instruccion *lista,
    Instruccion *nueva
);

int ejecutar_programa(Instruccion *programa);

void liberar_expresiones(Expresion *expr);
void liberar_programa(Instruccion *programa);

#endif
