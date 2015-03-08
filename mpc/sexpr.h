#ifndef SEXPR_H
#define SEXPR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* possible sval types */
enum { SVAL_ERR, SVAL_INT, SVAL_DEC, SVAL_SYM, SVAL_SEXPR };

typedef struct sval sval;

typedef union {
	long nt;
	double dec;
} val;

struct sval {
	int type;
	char* err;
	char* sym;
	val num;
	int count;
	sval** cell;
};

sval* sval_int(long x);
sval* sval_dec(double x);
sval* sval_err(char* m);
sval* sval_sym(char *s);
sval* sval_sexpr();
void sval_del(sval *v);

#endif