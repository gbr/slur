#ifndef EVALUATE_H
#define EVALUATE_H

#include <stdlib.h>
#include <string.h>

#include "types.h"

sval* sval_eval_sexpr(senv* e, sval* v);
sval* sval_eval(senv* e, sval* v);
sval* sval_call(senv* e, sval* f, sval* a);
sval* builtin_op(senv* e, sval* a, char* op);
sval* builtin_head(senv* e, sval* a);
sval* builtin_tail(senv* e, sval* a);
sval* builtin_list(senv* e, sval* a);
sval* builtin_eval(senv* e, sval* a);
sval* builtin_join(senv* e, sval* a);
sval* builtin_cons(senv* e, sval* a);
sval* builtin_len(senv* e, sval* a);
sval* builtin_init(senv* e, sval* a);
void senv_add_builtin(senv* e, char* name,
    sbuiltin func);
void senv_add_builtins(senv* e);
sval* builtin_def(senv* e, sval* a);
sval* builtin_put(senv* e, sval* a);
sval* builtin_lambda(senv* e, sval* a);
sval* builtin_env(senv* e, sval* a);

#endif 
