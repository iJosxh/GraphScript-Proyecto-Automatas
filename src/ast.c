
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ast.h"
#include "tabla_simbolos.h"
#include "graficos.h"

static int errores_semanticos = 0;
static int detener_ejecucion = 0;

static void error_semantico(const char *mensaje) {
    fprintf(stderr, "Error semantico: %s\n", mensaje);
    errores_semanticos++;
    detener_ejecucion = 1;
}

Expresion *crear_numero(double valor) {
    Expresion *e = calloc(1, sizeof(*e));

    if (e != NULL) {
        e->tipo = EXPR_NUMERO;
        e->numero = valor;
    }

    return e;
}

Expresion *crear_variable(char *nombre) {
    Expresion *e = calloc(1, sizeof(*e));

    if (e != NULL) {
        e->tipo = EXPR_VARIABLE;
        e->nombre = nombre;
    } else {
        free(nombre);
    }

    return e;
}

Expresion *crear_binaria(
    Operador op,
    Expresion *izq,
    Expresion *der
) {
    Expresion *e = calloc(1, sizeof(*e));

    if (e != NULL) {
        e->tipo = EXPR_BINARIA;
        e->operador = op;
        e->izquierda = izq;
        e->derecha = der;
    }

    return e;
}

Expresion *crear_negativa(Expresion *expr) {
    Expresion *e = calloc(1, sizeof(*e));

    if (e != NULL) {
        e->tipo = EXPR_NEGATIVA;
        e->izquierda = expr;
    }

    return e;
}

Instruccion *crear_instruccion(TipoInstruccion tipo) {
    Instruccion *i = calloc(1, sizeof(*i));

    if (i != NULL) {
        i->tipo = tipo;
    }

    return i;
}

Instruccion *agregar_instruccion(
    Instruccion *lista,
    Instruccion *nueva
) {
    if (nueva == NULL) {
        return lista;
    }

    if (lista == NULL) {
        return nueva;
    }

    Instruccion *actual = lista;

    while (actual->siguiente != NULL) {
        actual = actual->siguiente;
    }

    actual->siguiente = nueva;
    return lista;
}

static double evaluar(Expresion *e, int *valido) {
    if (e == NULL || detener_ejecucion) {
        *valido = 0;
        return 0;
    }

    if (e->tipo == EXPR_NUMERO) {
        return e->numero;
    }

    if (e->tipo == EXPR_VARIABLE) {
        double valor;

        if (!obtener_variable(e->nombre, &valor)) {
            char mensaje[160];

            snprintf(
                mensaje,
                sizeof(mensaje),
                "variable '%s' no declarada",
                e->nombre ? e->nombre : "(null)"
            );

            error_semantico(mensaje);
            *valido = 0;
            return 0;
        }

        return valor;
    }

    if (e->tipo == EXPR_NEGATIVA) {
        int ok = 1;
        double valor = evaluar(e->izquierda, &ok);

        if (!ok) {
            *valido = 0;
            return 0;
        }

        return -valor;
    }

    int ok_izq = 1;
    int ok_der = 1;

    double a = evaluar(e->izquierda, &ok_izq);

    if (!ok_izq) {
        *valido = 0;
        return 0;
    }

    double b = evaluar(e->derecha, &ok_der);

    if (!ok_der) {
        *valido = 0;
        return 0;
    }

    switch (e->operador) {
        case OP_SUMA:
            return a + b;

        case OP_RESTA:
            return a - b;

        case OP_MULTIPLICACION:
            return a * b;

        case OP_DIVISION:
            if (fabs(b) < 1e-12) {
                error_semantico("division entre cero");
                *valido = 0;
                return 0;
            }

            return a / b;

        case OP_MAYOR:
            return a > b ? 1 : 0;

        case OP_MENOR:
            return a < b ? 1 : 0;

        case OP_IGUAL:
            return fabs(a - b) < 1e-9 ? 1 : 0;
    }

    error_semantico("operador desconocido");
    *valido = 0;
    return 0;
}

static void ejecutar_lista(Instruccion *lista) {
    Instruccion *i = lista;

    while (i != NULL && !detener_ejecucion) {
        int valido = 1;
        double valor = 0;

        switch (i->tipo) {
            case STMT_DECLARACION:
                valor = evaluar(i->expresion, &valido);

                if (valido && !detener_ejecucion) {
                    if (!guardar_variable(i->nombre, valor)) {
                        error_semantico(
                            "no se pudo guardar la variable"
                        );
                    }
                }
                break;

            case STMT_FORWARD:
                valor = evaluar(i->expresion, &valido);

                if (valido && !detener_ejecucion) {
                    avanzar(valor);
                }
                break;

            case STMT_ROTATE:
                valor = evaluar(i->expresion, &valido);

                if (valido && !detener_ejecucion) {
                    girar(valor);
                }
                break;

            case STMT_COLOR:
                cambiar_color(i->cadena);
                break;

            case STMT_PEN_UP:
                levantar_pluma();
                break;

            case STMT_PEN_DOWN:
                bajar_pluma();
                break;

            case STMT_REPEAT:
                valor = evaluar(i->expresion, &valido);

                if (!valido || detener_ejecucion) {
                    break;
                }

                if (valor < 0 ||
                    valor > 10000 ||
                    floor(valor) != valor) {

                    error_semantico(
                        "repeat requiere un entero entre 0 y 10000"
                    );

                    break;
                }

                for (int n = 0;
                     n < (int)valor && !detener_ejecucion;
                     n++) {
                    ejecutar_lista(i->cuerpo);
                }
                break;

            case STMT_IF:
                valor = evaluar(i->condicion, &valido);

                if (valido &&
                    !detener_ejecucion &&
                    valor != 0) {
                    ejecutar_lista(i->cuerpo);
                }
                break;
        }

        i = i->siguiente;
    }
}

int ejecutar_programa(Instruccion *programa) {
    errores_semanticos = 0;
    detener_ejecucion = 0;

    ejecutar_lista(programa);

    if (errores_semanticos > 0) {
        fprintf(
            stderr,
            "La ejecucion termino con %d error(es) semantico(s)\n",
            errores_semanticos
        );

        return 0;
    }

    return 1;
}

void liberar_expresiones(Expresion *expr) {
    if (expr == NULL) {
        return;
    }

    liberar_expresiones(expr->izquierda);
    liberar_expresiones(expr->derecha);

    free(expr->nombre);
    free(expr);
}

void liberar_programa(Instruccion *programa) {
    while (programa != NULL) {
        Instruccion *siguiente = programa->siguiente;

        free(programa->nombre);
        free(programa->cadena);

        liberar_expresiones(programa->expresion);
        liberar_expresiones(programa->condicion);

        liberar_programa(programa->cuerpo);

        free(programa);
        programa = siguiente;
    }
}
