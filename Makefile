
CC = gcc
FLEX = flex
BISON = bison

CFLAGS = -Wall -Wextra -I src -I build
LDLIBS = -lm

TARGET = build/graphscript
PARSER_C = build/parser.tab.c
PARSER_H = build/parser.tab.h
LEXER_C = build/lex.yy.c

SOURCES = $(PARSER_C) $(LEXER_C) \
          src/tabla_simbolos.c \
          src/graficos.c \
          src/ast.c

.PHONY: all clean test

all: $(TARGET)

build:
	mkdir -p build

$(PARSER_C) $(PARSER_H) &: src/parser.y src/ast.h | build
	$(BISON) -d -o $(PARSER_C) src/parser.y

$(LEXER_C): src/lexer.l $(PARSER_H) | build
	$(FLEX) -o $(LEXER_C) src/lexer.l

$(TARGET): $(SOURCES) $(PARSER_H)
	$(CC) $(CFLAGS) $(SOURCES) $(LDLIBS) -o $(TARGET)

test: $(TARGET)
	@echo "== Prueba de dibujo =="
	./$(TARGET) < test/cuadrado.gs
	@echo "== Variable no declarada: debe fallar =="
	@if ./$(TARGET) < test/error_variable.gs; then echo "FALLO: se esperaba error"; exit 1; else echo "Correcto: error detectado"; fi
	@echo "== Division entre cero: debe fallar =="
	@if ./$(TARGET) < test/error_division.gs; then echo "FALLO: se esperaba error"; exit 1; else echo "Correcto: error detectado"; fi
	@echo "== Error de sintaxis: debe fallar =="
	@if ./$(TARGET) < test/error_sintaxis.gs; then echo "FALLO: se esperaba error"; exit 1; else echo "Correcto: error detectado"; fi

clean:
	rm -f $(PARSER_C) $(PARSER_H) $(LEXER_C) $(TARGET) salida.svg
