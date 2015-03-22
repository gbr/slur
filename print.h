#ifndef PRINT_H
#define PRINT_H

#include <stdio.h>

#include "mpc/mpc.h"
#include "types.h"

void sval_expr_print(sval* v, char open, char close);
void sval_print(sval* v);
void sval_println(sval* v);
void sval_print_str(sval* v);

#endif

