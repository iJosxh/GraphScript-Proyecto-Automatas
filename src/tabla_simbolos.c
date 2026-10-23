
#include <stdio.h>
#include <string.h>
#include "tabla_simbolos.h"

static Variable tabla[MAX_VARIABLES];
static int cantidad = 0;

void inicializar_tabla(void) {
    cantidad = 0;
    memset(tabla, 0, sizeof(tabla));
}

int guardar_variable(const char *nombre, double valor) {
    if (nombre == NULL ||
        strlen(nombre) >= sizeof(tabla[0].nombre)) {
        fprintf(stderr, "Error semantico: nombre de variable invalido\n");
        return 0;
    }

    for (int i = 0; i < cantidad; i++) {
        if (strcmp(tabla[i].nombre, nombre) == 0) {
            tabla[i].valor = valor;
            printf(
                "Variable actualizada: %s = %.2f\n",
                nombre,
                valor
            );
            return 1;
        }
    }

    if (cantidad >= MAX_VARIABLES) {
        fprintf(
            stderr,
            "Error semantico: limite de %d variables alcanzado\n",
            MAX_VARIABLES
        );
        return 0;
    }

    strcpy(tabla[cantidad].nombre, nombre);
    tabla[cantidad].valor = valor;
    cantidad++;

    printf("Variable guardada: %s = %.2f\n", nombre, valor);
    return 1;
}

int obtener_variable(const char *nombre, double *valor) {
    if (nombre == NULL || valor == NULL) {
        return 0;
    }

    for (int i = 0; i < cantidad; i++) {
        if (strcmp(tabla[i].nombre, nombre) == 0) {
            *valor = tabla[i].valor;
            return 1;
        }
    }

    return 0;
}

void imprimir_tabla(void) {
    printf("\n--- TABLA DE SIMBOLOS ---\n");

    if (cantidad == 0) {
        printf("(vacia)\n");
    }

    for (int i = 0; i < cantidad; i++) {
        printf(
            "%-20s %.2f\n",
            tabla[i].nombre,
            tabla[i].valor
        );
    }

    printf("-------------------------\n");
}
