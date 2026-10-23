
#ifndef TABLA_SIMBOLOS_H
#define TABLA_SIMBOLOS_H

#define MAX_VARIABLES 100

typedef struct {
    char nombre[64];
    double valor;
} Variable;

void inicializar_tabla(void);
int guardar_variable(const char *nombre, double valor);
int obtener_variable(const char *nombre, double *valor);
void imprimir_tabla(void);

#endif
