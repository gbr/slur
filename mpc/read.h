#ifndef READ_H
#define READ_H

#include <errno.h>
#include <string.h>

#include "types.h"
#include "mpc.h"
#include "evaluate.h"

#define BASE 10

sval* sval_read_num(mpc_ast_t* t);
sval* sval_read(mpc_ast_t* t);
sval* sval_add(sval* v, sval* x);

#endif
