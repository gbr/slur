#ifndef SEXPR_H
#define SEXPR_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "error_handling.h"

/* possible sval types */
enum { 	SVAL_ERR, SVAL_INT, SVAL_DEC, SVAL_BOOL,
		SVAL_SYM, SVAL_FUN, SVAL_SEXPR, SVAL_QEXPR };

struct sval;
struct senv;

typedef struct sval sval;
typedef struct senv senv;

typedef sval*(*sbuiltin)(senv*, sval*);

typedef struct _list_t_ {
    char* sym;
    sval* val;
    struct _list_t_ *next;
} list_t;

typedef struct _hash_table_t_ {
    int size;      
    list_t** table; 
} hash_table_t;

typedef union {
	long nt;
	double dec;
} val;

struct sval {
	int type;

	/* basic */
	// really, all of these could be in a union
	char* err;
	char* sym;
	val num;
	bool cond;

	/* function */
	sbuiltin builtin;
	senv* env;
	sval* formals;
	sval* body;

	/* expression */
	int count;
	sval** cell;
};

struct senv {
	senv* par;
	int count;
	hash_table_t vals;
};

char* stype_name(int sval_enum);
hash_table_t* hash_table(int size);
unsigned int hash(hash_table_t* ht, char* sym);
list_t* lookup_string(hash_table_t* hashtable, char* sym);
sval* sval_lambda(sval* formals, sval* body);
sval* sval_int(long x);
sval* sval_dec(double x);
sval* sval_err(char* fmt, ...);
sval* sval_sym(char *s);
sval* sval_sexpr();
sval* sval_qexpr();
sval* sval_fun(char* name, sbuiltin func);
sval* sval_bool(bool cond);
senv* senv_new();
void sval_del(sval *v);
void senv_del(senv* e);
sval* sval_pop(sval* v, int i);
sval* sval_take(sval* v, int i);
sval* sval_join(sval* x, sval* y);
sval* sval_add(sval* v, sval* x);
sval* sval_copy(sval* v);
sval* sval_copy(sval* v);
sval* senv_get(senv *e, sval* k);
void senv_put(senv *e, sval* k, sval* v);
senv* senv_copy(senv* e);
void senv_def(senv* e, sval* k, sval* v);

#endif