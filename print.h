#ifndef PRINT_H
#define PRINT_H

#include <stdio.h>

#include "types.h"

void sval_expr_print(sval* v, char open, char close);
void sval_print(sval* v);
void sval_println(sval* v);

#endif

