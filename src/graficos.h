
#ifndef GRAFICOS_H
#define GRAFICOS_H

int iniciar_grafico(const char *archivo);
void finalizar_grafico(void);

void avanzar(double distancia);
void girar(double grados);
void cambiar_color(const char *color);
void levantar_pluma(void);
void bajar_pluma(void);

#endif
