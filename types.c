#include "types.h"

/* slur value decode */
char* stype_name(int sval_type) {
	switch (sval_type) {
		case SVAL_ERR:   return "error";
		case SVAL_INT:   return "integer";
		case SVAL_DEC:   return "double";
		case SVAL_STR:   return "string";
        case SVAL_BOOL:  return "bool";
		case SVAL_SYM:   return "symbol";
		case SVAL_FUN:   return "function";
		case SVAL_SEXPR: return "s-expression";
		case SVAL_QEXPR: return "q-expression";
		default: return "unknown type";
	}
}

/* Slur value functions */

/* constructor for user-defined sval functions */
sval* sval_lambda(sval* formals, sval* body) {
	sval* v = malloc(sizeof(sval));
	SASSERT_ALLOC_MEM(v);
	v->type = SVAL_FUN;

	/* set builtin to null */
	v->builtin = NULL;

	v->env = senv_new();
	v->formals = formals;
	v->body = body;
	return v;
}

/* slur value integer constructor */
sval* sval_int(long x) {
	sval* v = malloc(sizeof(sval));
	SASSERT_ALLOC_MEM(v);
	v->type = SVAL_INT;
	v->num.nt = x;
	return v;
}

/* slur value decimal constructor */
sval* sval_dec(double x) {
	sval* v = malloc(sizeof(sval));
	SASSERT_ALLOC_MEM(v);
	v->type = SVAL_DEC;
	v->num.dec = x;
	return v;
}

/* slur value error constructor */
sval* sval_err(char* fmt, ...) {
	sval* v = malloc(sizeof(sval));
	SASSERT_ALLOC_MEM(v);
	v->type = SVAL_ERR;

	va_list va;
	va_start(va, fmt);
	/* determine buffer size necessary from va */
	// int size = vsnprintf(NULL, 0, fmt, va);

	/* allocate the error buffer */
	// v->err = malloc(size+1);
	v->err = malloc(512);
	SASSERT_ALLOC_MEM(v->err);

	/* write the formatted string to the error buffer */
	// sprintf(v->err, fmt, va);
	vsnprintf(v->err, 511, fmt, va);

	/* reallocate to the number of bytes actually used */
	v->err = realloc(v->err, strlen(v->err)+1);
	SASSERT_ALLOC_MEM(v->err);

	va_end(va);
	return v;
}

/* slur value symbol constructor */
sval* sval_sym(char *s) {
	sval* v = malloc(sizeof(sval));
	SASSERT_ALLOC_MEM(v);
	v->type = SVAL_SYM;
	v->sym = malloc(strlen(s) + 1);
	SASSERT_ALLOC_MEM(v->sym);
	strcpy(v->sym, s);
	return v;
}

/* slur value s-expression constructor */
sval* sval_sexpr() {
	sval* v = malloc(sizeof(sval));
	SASSERT_ALLOC_MEM(v);
	v->type = SVAL_SEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

/* slur value s-expression constructor */
sval* sval_qexpr() {
	sval* v = malloc(sizeof(sval));
	SASSERT_ALLOC_MEM(v);
	v->type = SVAL_QEXPR;
	v->count = 0;
	v->cell = NULL;
	return v;
}

/* slur value function constructor */
sval* sval_fun(char* name, sbuiltin func) {
	sval* v = malloc(sizeof(sval));
	SASSERT_ALLOC_MEM(v);
	v->type = SVAL_FUN;
	v->builtin = func;
	v->sym = name;
	return v;
}

/* slur value boolean constructor */
sval* sval_bool(int x) {
	sval* v = malloc(sizeof(sval));
	SASSERT_ALLOC_MEM(v);
	v->type = SVAL_BOOL;
	v->cond = x == 0 ? false : true;
	return v;
}

sval* sval_str(char *s) {
    sval* v = malloc(sizeof(sval));
    SASSERT_ALLOC_MEM(v);
    v->type = SVAL_STR;
    v->str = malloc(strlen(s)+1);
    strcpy(v->str, s);
    return v;
}

senv* senv_new() {
	senv* e = malloc(sizeof(senv));
	SASSERT_ALLOC_MEM(e);
	e->par = NULL;
	e->count = 0;
	e->syms = NULL;
	e->vals = NULL;
	return e;
}

void sval_del(sval *v) {

	switch(v->type) {
		case SVAL_INT:  break;
		case SVAL_DEC:  break;
        case SVAL_BOOL: break;

        case SVAL_STR:  free(v->str); break;

		case SVAL_FUN:
			if (!v->builtin) {
				senv_del(v->env);
				sval_del(v->formals);
				sval_del(v->body);
			}
			break;

		case SVAL_ERR: free(v->err); break;
		case SVAL_SYM: free(v->sym); break;

		case SVAL_QEXPR:
		case SVAL_SEXPR:
                         for (int i = 0; i < v->count; i++) {
                             sval_del(v->cell[i]);
                         }
                         free(v->cell);
                         break;
	}
	free(v);
}

void senv_del(senv* e) {
	for (int i = 0; i < e->count; i++) {
		free(e->syms[i]);
		sval_del(e->vals[i]);
	}
	free(e->syms);
	free(e->vals);
	free(e);
}

sval* sval_join(sval* x, sval* y) {
	while(y->count) {
		x = sval_add(x, sval_pop(y, 0));
	}

	sval_del(y);
	return x;
}


sval* sval_add(sval* v, sval* x) {
	v->count++;
	v->cell = realloc(v->cell, sizeof(sval*) * v->count);
	SASSERT_ALLOC_MEM(v->cell);
	v->cell[v->count-1] = x;
	return v;
}

sval* senv_get(senv *e, sval* k) {
	/* return a match if it exists */
	for (int i = 0; i < e->count; i++) {
		if (strcmp(e->syms[i], k->sym) == 0) {
			return sval_copy(e->vals[i]);
		}
	}
	if (e->par) {
		return senv_get(e->par, k);
	} else {
		return sval_err("unbound symbol '%s'", k->sym);
	}
}


void senv_put(senv* e, sval* k, sval* v) {
	/* pre-check if variable already exists */
	for (int i = 0; i < e->count; i++) {

		/* if found, replace with new var */
		if (strcmp(e->syms[i], k->sym) == 0) {
			sval_del(e->vals[i]);
			e->vals[i] = sval_copy(v);
			return;
		}
	}

	/* allocate space for new entry */
	e->count++;
	e->vals = realloc(e->vals, sizeof(sval*) * e->count);
	SASSERT_ALLOC_MEM(e->vals);
	e->syms = realloc(e->syms, sizeof(sval*) * e->count);
	SASSERT_ALLOC_MEM(e->syms);

	/* copy new contents into new location */
	e->vals[e->count-1] = sval_copy(v);
	e->syms[e->count-1] = malloc(strlen(k->sym)+1);
	SASSERT_ALLOC_MEM(e->syms[e->count-1]);
	strcpy(e->syms[e->count-1], k->sym);
}

sval* sval_pop(sval* v, int i) {
	sval *x  = v->cell[i];

	/* shift memory after the item at i over the top */
	memmove(&v->cell[i], &v->cell[i+1],
		sizeof(sval*) * (v->count-i-1));

	v->count--;
	v->cell = realloc(v->cell, sizeof(sval*) * v->count);
	return x;
}

sval* sval_take(sval* v, int i) {
	sval* x = sval_pop(v, i);
	sval_del(v);
	return x;
}

sval* sval_copy(sval* v) {
	sval* x = malloc(sizeof(sval));
	SASSERT_ALLOC_MEM(x);
	x->type = v->type;

	switch (v->type) {
		/* copy functions and numbers directly */
		case SVAL_FUN:
		if (v->builtin) {
			x->builtin = v->builtin;
		} else {
			x->builtin = NULL;
			x->env = senv_copy(v->env);
			x->formals = sval_copy(v->formals);
			x->body = sval_copy(v->body);
		}
		break;

		case SVAL_INT: x->num.nt = v->num.nt; break;
		case SVAL_DEC: x->num.dec = v->num.dec; break;

		/* copy strings using strcpy and malloc */
		case SVAL_STR:
			x->str = malloc(strlen(v->str)+1);
			SASSERT_ALLOC_MEM(x->str);
			strcpy(x->str, v->str); break;
		case SVAL_ERR:
			x->err = malloc(strlen(v->err)+1);
			SASSERT_ALLOC_MEM(x->err);
			strcpy(x->err, v->err); break;

		case SVAL_SYM:
			x->sym = malloc(strlen(v->sym)+1);
			SASSERT_ALLOC_MEM(x->sym);
			if (!x->sym) {

			}
			strcpy(x->sym, v->sym); break;

		/* copy lists by each sub-expression */
		case SVAL_SEXPR:
		case SVAL_QEXPR:
			x->count = v->count;
			x->cell = malloc(sizeof(sval*) * x->count);
			for (int i = 0; i < x->count; i++) {
				x->cell[i] = sval_copy(v->cell[i]);
			}
			break;
	}
	return x;
}

senv* senv_copy(senv* e) {
	senv* n = malloc(sizeof(senv));
	SASSERT_ALLOC_MEM(n);
	n->par = e->par;
	n->count = e->count;
	n->syms = malloc(sizeof(char*) * n->count);
	SASSERT_ALLOC_MEM(n->syms);
	n->vals = malloc(sizeof(sval*) * n->count);
	SASSERT_ALLOC_MEM(n->vals);
	for (int i = 0; i < e->count; i++) {
		n->syms[i] = malloc(strlen(e->syms[i]) + 1);
		SASSERT_ALLOC_MEM(n->syms[i]);
		strcpy(n->syms[i], e->syms[i]);
		n->vals[i] = sval_copy(e->vals[i]);
	}
	return n;
}

void senv_def(senv* e, sval* k, sval* v) {
	/* find top level env */
	while (e->par) { e = e->par; }
	/* put value in e */
	senv_put(e, k, v);
}

