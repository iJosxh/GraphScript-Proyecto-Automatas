
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "graficos.h"

#define ANCHO 800
#define ALTO 600

static FILE *svg = NULL;
static double x = ANCHO / 2.0;
static double y = ALTO / 2.0;
static double angulo = 0.0;
static int pluma_abajo = 1;
static char color[64] = "black";

int iniciar_grafico(const char *archivo)
{
    svg = fopen(archivo, "w");

    if (svg == NULL) {
        perror("No se pudo crear el archivo SVG");
        return 0;
    }

    x = ANCHO / 2.0;
    y = ALTO / 2.0;
    angulo = 0.0;
    pluma_abajo = 1;
    strcpy(color, "black");

    fprintf(svg,
        "<svg xmlns=\"http://www.w3.org/2000/svg\" "
        "width=\"%d\" height=\"%d\" viewBox=\"0 0 %d %d\">\n",
        ANCHO, ALTO, ANCHO, ALTO);

    fprintf(svg,
        "<rect width=\"100%%\" height=\"100%%\" fill=\"white\"/>\n");

    return 1;
}

void finalizar_grafico(void)
{
    if (svg != NULL) {
        fprintf(svg, "</svg>\n");
        fclose(svg);
        svg = NULL;
    }
}

void avanzar(double distancia)
{
    double radianes = angulo * 3.14159265358979323846 / 180.0;

    double nuevo_x = x + distancia * cos(radianes);
    double nuevo_y = y - distancia * sin(radianes);

    if (svg != NULL && pluma_abajo) {
        fprintf(svg,
            "<line x1=\"%.2f\" y1=\"%.2f\" "
            "x2=\"%.2f\" y2=\"%.2f\" "
            "stroke=\"%s\" stroke-width=\"2\"/>\n",
            x, y, nuevo_x, nuevo_y, color);
    }

    x = nuevo_x;
    y = nuevo_y;
}

void girar(double grados)
{
    angulo += grados;
}

void cambiar_color(const char *nuevo_color)
{
    size_t longitud = strlen(nuevo_color);

    /* Acepta cadenas con o sin comillas */
    if (longitud >= 2 &&
        nuevo_color[0] == '"' &&
        nuevo_color[longitud - 1] == '"') {

        size_t n = longitud - 2;

        if (n >= sizeof(color))
            n = sizeof(color) - 1;

        memcpy(color, nuevo_color + 1, n);
        color[n] = '\0';
    } else {
        snprintf(color, sizeof(color), "%s", nuevo_color);
    }
}

void levantar_pluma(void)
{
    pluma_abajo = 0;
}

void bajar_pluma(void)
{
    pluma_abajo = 1;
}
