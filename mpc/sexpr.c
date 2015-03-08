#include "sexpr.h"


/* Slur value functions */

/* slur value number constructor */
sval* sval_int(long x) {
	sval* v = malloc(sizeof(sval));
	v->type = SVAL_INT;
	v->num.nt = x;
	return v;
}

sval* sval_dec(double x) {
	sval* v = malloc(sizeof(sval));
	v->type = SVAL_DEC;
	v->num.dec = x;
	return v;
}

/* slur value error constructor */
sval* sval_err(char* m) {
	sval* v = malloc(sizeof(sval));
	v->type = SVAL_ERR;
	v->err = malloc(strlen(m) + 	1);
	strcpy(v->err, m);
	return v;
}

/* slur value symbol constructor */
sval* sval_sym(char *s) {
	sval* v = malloc(sizeof(sval));
	v->type = SVAL_SYM;
	v->sym = malloc(strlen(s) + 1);
	strcpy(v->sym, s);
	return v;
}

/* slur value s-expression constructor */
sval* sval_sexpr() {
	sval* v = malloc(sizeof(sval));
	v->type = SVAL_SEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

void sval_del(sval *v) {

	switch(v->type) {
		case SVAL_INT: break;
		case SVAL_DEC: break;

		case SVAL_ERR: free(v->err); break;
		case SVAL_SYM: free(v->sym); break;

		case SVAL_SEXPR:
     for (int i = 0; i < v->count; i++) {
        sval_del(v->cell[i]);
    }
    free(v->cell);
    break;
}
free(v);
}

