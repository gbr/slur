#include "evaluate.h"

sval* sval_eval_sexpr(senv* e, sval* v) {

    /* evaluate children */
    for (int i = 0; i < v->count; i++) {
        v->cell[i] = sval_eval(e, v->cell[i]);
        // error check
        if (v->cell[i]->type == SVAL_ERR) { return sval_take(v, i); }
    }

    if (v->count == 0) { return v; }
    if (v->count == 1) { return sval_take(v, 0); }

    /* ensure first element is a function after evaluation */
    sval* f = sval_pop(v, 0);
    if (f->type != SVAL_FUN) {
        sval* err = sval_err(
            "s-expression starts with incorrect type. "
            "got %s, expected %s.",
            stype_name(f->type), stype_name(SVAL_FUN));
        sval_del(f); sval_del(v);
        return err;
    }

    /* call builtin with operator */
    sval* result = sval_call(e, f, v);
    sval_del(f);
    return result;
}

sval* sval_eval(senv* e, sval* v) {
    /* evaluate symbols */
    if (v->type == SVAL_SYM) {
        sval* x = senv_get(e, v);
        sval_del(v);
        return x;
    }
    /* evaluate s-expressions */
    if (v->type == SVAL_SEXPR) { return sval_eval_sexpr(e, v); }
    /* other sval types remain the same */
    return v;
}

sval* sval_call(senv* e, sval* f, sval* a) {
    if (f->builtin) { return f->builtin(e, a); }

    int given = a->count;
    int total = f->formals->count;

    while (a->count) {
        if (f->formals->count == 0) {
            sval_del(a); return sval_err(
                "function passed too many arguments. "
                "got %i, expected %i", given, total);
        }

        /* pop the first symbol from the formals */
        sval* sym = sval_pop(f->formals, 0);

        /* special case to deal with '&' */
        if (strcmp(sym->sym, "&") == 0) {
            /* ensure '&' is followed by another symbol */
            if (f->formals->count != 1) {
                sval_del(a);
                return sval_err("function format invalid. "
                    "symbol '&' not followed by single symbol.");
            }

            /* next formal should be bound to remaining arguments */
            sval* nsym = sval_pop(f->formals, 0);
            senv_put(f->env, nsym, builtin_list(e, a));
            sval_del(sym); sval_del(nsym);
            break;
        }

        /* pop the next argument from the list */
        sval* val = sval_pop(a, 0);

        /* bind a copy into the function's environment */
        senv_put(f->env, sym, val);

        /* delete symbol and value */
        sval_del(sym); sval_del(val);
    }

    sval_del(a);

    /* If '&' remains in formal list bind to empty list */
    if (f->formals->count > 0 &&
        strcmp(f->formals->cell[0]->sym, "&") == 0) {

      /* Check to ensure that & is not passed invalidly. */
        if (f->formals->count != 2) {
            return sval_err("Function format invalid. "
                "Symbol '&' not followed by single symbol.");
        }

      /* Pop and delete '&' symbol */
        sval_del(sval_pop(f->formals, 0));

      /* Pop next symbol and create empty list */
        sval* sym = sval_pop(f->formals, 0);
        sval* val = sval_qexpr();

      /* Bind to environment and delete */
        senv_put(f->env, sym, val);
        sval_del(sym); sval_del(val);
    }

    /* if all formals have been bound, evaluate */
    if (f->formals->count == 0) {
        f->env->par = e;

        return builtin_eval(
            f->env, sval_add(sval_sexpr(), sval_copy(f->body)));
    } else {
        /* otherwise return partially evaluated function */
        return sval_copy(f);
    }
}

bool sval_eq(sval *x, sval *y) {
    if (x->type != y->type) { return false; }

    /* compare based on type */
    switch (x->type) {
        case SVAL_INT: return (x->num.nt == y->num.nt);
        case SVAL_DEC: return (x->num.dec == y->num.dec);

        case SVAL_ERR: return (strcmp(x->err, y->err) == 0);
        case SVAL_SYM: return (strcmp(x->sym, y->sym) == 0);

        /* if builtin, simple compare; othw, formals and body */
        case SVAL_FUN:
                       if (x->builtin || y->builtin) {
                           return x->builtin == y->builtin;
                       } else {
                           return sval_eq(x->formals, y->formals)
                               && sval_eq(x->body, y->body);
                       }

        case SVAL_QEXPR:
        case SVAL_SEXPR:
                       if (x->count != y->count) { return false; }
                       for (int i = 0; i < x->count; i++) {
                           /* if any element unequal, return false */
                           if (!sval_eq(x->cell[i], y->cell[i])) { return false; }
                       }
                       return true;
        break;
    }
    return false;
}

sval* builtin_add(senv* e, sval* a) {
    return builtin_op(e, a, "+");
}

sval* builtin_sub(senv* e, sval* a) {
    return builtin_op(e, a, "-");
}

sval* builtin_mul(senv* e, sval* a) {
    return builtin_op(e, a, "*");
}

sval* builtin_mod(senv* e, sval* a) {
    return builtin_op(e, a, "%");
}

sval* builtin_div(senv* e, sval* a) {
    return builtin_op(e, a, "/");
}

sval* builtin_op(senv* e, sval* a, char* op) {

    /* ensure all arguments are numbers */
    for (int i = 0; i < a->count; i++) {
        SASSERT(a, a->cell[i]->type == SVAL_INT
            || a->cell[i]->type == SVAL_DEC,
            "function '%s' passed the incorrect type for argument %i. "
            "got %s, expected number", op, i, stype_name(a->cell[i]->type),
            "number");
    }

    sval* x = sval_pop(a, 0);

    /* if no arguments and sub, negate */
    if (strcmp(op, "-") == 0 && a->count == 0) {
        if (x->type == SVAL_INT) { x->num.nt = -x->num.nt; }
        if (x->type == SVAL_DEC) { x->num.dec = -x->num.dec; }
    }

    while (a->count > 0) {
        sval* y = sval_pop(a, 0);

        if (x->type == SVAL_INT && y->type == SVAL_INT) {
            if (strcmp(op, "+") == 0) { x->num.nt += y->num.nt; }
            if (strcmp(op, "-") == 0) { x->num.nt -= y->num.nt; }
            if (strcmp(op, "*") == 0) { x->num.nt *= y->num.nt; }
            if (strcmp(op, "%") == 0) { x->num.nt %= y->num.nt; }
            if (strcmp(op, "/") == 0) {
                if (y->num.nt == 0) {
                    sval_del(x); sval_del(y);
                    x = sval_err("division by zero"); break;
                }
                x->num.nt /= y->num.nt;
            }
        }
        else {
            if (x->type == SVAL_INT) {
                x->type = SVAL_DEC;
                x->num.dec = (double)x->num.nt;
            }
            if (y->type == SVAL_INT) { y->num.dec = (double)y->num.nt; }

            if (strcmp(op, "+") == 0) { x->num.dec += y->num.dec; }
            if (strcmp(op, "-") == 0) { x->num.dec -= y->num.dec; }
            if (strcmp(op, "*") == 0) { x->num.dec *= y->num.dec; }
            if (strcmp(op, "%") == 0) {
                x = sval_err("the mod operator is for integers");
                break;
            }
            if (strcmp(op, "/") == 0) {
                if (y->num.dec == 0) {
                    sval_del(x); sval_del(y);
                    x = sval_err("division by zero"); break;
                }
                x->num.dec /= y->num.dec;
            }
        }
        sval_del(y);
    }

    sval_del(a); return x;
}

sval* builtin_var(senv* e, sval* a, char* func) {
    SASSERT_ARG_TYPE(a, 0, SVAL_QEXPR, func);

    sval* syms = a->cell[0];

    for (int i = 0; i < syms->count; i++) {
        SASSERT(a, syms->cell[i]->type == SVAL_SYM,
            "function '%s' cannot define non-symbol. "
            "got %s, expected %s.", func,
            stype_name(syms->cell[i]->type),
            stype_name(SVAL_SYM));
    }

    SASSERT(a, syms->count == a->count-1,
        "function '%s' cannot define the input. "
        "%i symbols to %i values", func, syms->count, a->count-1);

    /* assign copies of values to symbols */
    for (int i = 0; i < syms->count; i++) {
        if (strcmp(func, "def") == 0) {
            senv_def(e, syms->cell[i], a->cell[i+1]);
        }

        if (strcmp(func, "=") == 0) {
            senv_put(e, syms->cell[i], a->cell[i+1]);
        }
    }

    sval_del(a);
    return sval_sexpr();
}

sval* builtin_gt(senv* e, sval* a) {
    return builtin_ord(e, a, ">");
}

sval* builtin_lt(senv* e, sval* a) {
    return builtin_ord(e, a, "<");
}

sval* builtin_ge(senv* e, sval* a) {
    return builtin_ord(e, a, ">=");
}

sval* builtin_le(senv* e, sval* a) {
    return builtin_ord(e, a, "<=");
}
sval* builtin_eq(senv* e, sval* a) {
    return builtin_cmp(e, a, "==");
}

sval* builtin_ne(senv* e, sval* a) {
    return builtin_cmp(e, a, "/=");
}
sval* builtin_ord(senv* e, sval* a, char* op) {
    SASSERT_ARG_COUNT(a, 2, op);

   /* ensure all arguments are numbers */
    for (int i = 0; i < a->count; i++) {
        SASSERT(a, a->cell[i]->type == SVAL_INT
            || a->cell[i]->type == SVAL_DEC,
            "function '%s' passed the incorrect type for argument %i. "
            "got %s, expected number", op, i,
            stype_name(a->cell[i]->type));
    }

    int v;

    if (a->cell[0]->type == SVAL_INT && a->cell[1]->type == SVAL_INT) {
        if (strcmp(op, ">")  == 0) {
           v = a->cell[0]->num.nt > a->cell[1]->num.nt;
        }
        if (strcmp(op, "<")  == 0) {
            v = a->cell[0]->num.nt < a->cell[1]->num.nt;
        }
        if (strcmp(op, ">=") == 0) {
            v = a->cell[0]->num.nt >= a->cell[1]->num.nt;
        }
        if (strcmp(op, "<=") == 0) {
            v = a->cell[0]->num.nt <= a->cell[1]->num.nt;
        }
        if (strcmp(op, "==") == 0) {
            v = a->cell[0]->num.nt == a->cell[1]->num.nt;
        }
        if (strcmp(op, "/=") == 0) {
            v = a->cell[0]->num.nt != a->cell[1]->num.nt;
        }
    }
    else {
        if (a->cell[0]->type == SVAL_INT) {
            a->cell[0]->type = SVAL_DEC;
            a->cell[0]->num.dec = (double)a->cell[0]->num.nt;
        }
        if (a->cell[1]->type == SVAL_INT) {
            a->cell[1]->num.dec = (double)a->cell[1]->num.nt;
        }

        if (strcmp(op, ">")  == 0) {
            v = a->cell[0]->num.dec > a->cell[1]->num.dec;
        }
        if (strcmp(op, "<")  == 0) {
            v = a->cell[0]->num.dec < a->cell[1]->num.dec;
        }
        if (strcmp(op, ">=") == 0) {
            v = a->cell[0]->num.dec >= a->cell[1]->num.dec;
        }
        if (strcmp(op, "<=") == 0) {
            v = a->cell[0]->num.dec <= a->cell[1]->num.dec;
        }
        if (strcmp(op, "==") == 0) {
            v = a->cell[0]->num.dec == a->cell[1]->num.dec;
        }
        if (strcmp(op, "/=") == 0) {
            v = a->cell[0]->num.dec != a->cell[1]->num.dec;
        }
    }

    sval_del(a); return sval_bool(v);
}

sval* builtin_cmp(senv* e, sval* a, char* op) {
    SASSERT_ARG_COUNT(a, 2, op);

    int v;
    if (strcmp(op, "==") == 0) {
        v = sval_eq(a->cell[0], a->cell[1]);
    }
    if (strcmp(op, "/=") == 0) {
        v = !sval_eq(a->cell[0], a->cell[1]);
    }
    sval_del(a); return sval_bool(v);
}

sval* builtin_if(senv* e, sval* a) {
    SASSERT_ARG_COUNT(a, 3, "if");
    SASSERT_ARG_TYPE(a, 0, SVAL_BOOL, "if");
    SASSERT_ARG_TYPE(a, 1, SVAL_QEXPR, "if");
    SASSERT_ARG_TYPE(a, 2, SVAL_QEXPR, "if");

    /* mark expressions as evaluable */
    sval *x;
    a->cell[1]->type = SVAL_SEXPR;
    a->cell[2]->type = SVAL_SEXPR;

    /* if cond is true, eval first expr; othw second */
    if (a->cell[0]->cond) {
        x = sval_eval(e, sval_pop(a, 1));
    } else {
        x = sval_eval(e, sval_pop(a, 2));
    }

    sval_del(a); return x;
}

sval* builtin_head(senv* e, sval* a) {
    SASSERT_ARG_COUNT(a, 1, "head");
    SASSERT_ARG_TYPE(a, 0, SVAL_QEXPR, "head");
    SASSERT_QEXPR_NONEMPTY(a, 0, "head");

    sval* v = sval_take(a, 0);
    while(v->count > 1) { sval_del(sval_pop(v, 1)); }

    return v;
}

sval* builtin_tail(senv* e, sval* a) {
    SASSERT_ARG_COUNT(a, 1, "tail");
    SASSERT_ARG_TYPE(a, 0, SVAL_QEXPR, "tail");
    SASSERT_QEXPR_NONEMPTY(a, 0, "tail");

    sval *v = sval_take(a, 0);
    sval_del(sval_pop(v, 0));
    return v;
}

sval* builtin_list(senv* e, sval* a) {
    a->type = SVAL_QEXPR;
    return a;
}

sval* builtin_eval(senv* e, sval* a) {
    SASSERT_ARG_COUNT(a, 1, "eval");
    SASSERT_ARG_TYPE(a, 0, SVAL_QEXPR, "eval");

    sval* x = sval_take(a, 0);
    x->type = SVAL_SEXPR;
    return sval_eval(e, x);
}

sval* builtin_join(senv* e, sval* a) {
    for (int i = 0; i < a->count; i++) {
        SASSERT_ARG_TYPE(a, i, SVAL_QEXPR, "join");
    }

    sval* x = sval_pop(a, 0);

    while (a->count) {
        x = sval_join(x, sval_pop(a, 0));
    }

    sval_del(a);
    return x;
}

sval* builtin_cons(senv* e, sval* a) {
    SASSERT_ARG_COUNT(a, 2, "cons");
    SASSERT_ARG_TYPE(a, 1, SVAL_QEXPR, "cons");

    sval* x = sval_pop(a, 0);
    sval* v = sval_qexpr();
    sval_add(v, x);

    while (a->count) {
        v = sval_join(v, sval_pop(a, 0));
    }

    sval_del(a);
    return v;
}

sval* builtin_len(senv* e, sval* a) {
    SASSERT_ARG_COUNT(a, 1, "len");
    SASSERT_ARG_TYPE(a, 0, SVAL_QEXPR, "len");

    sval* v = sval_int(a->cell[0]->count);
    sval_del(a);
    return v;
}

sval* builtin_init(senv* e, sval* a) {
    SASSERT_ARG_COUNT(a, 1, "tail");
    SASSERT_ARG_TYPE(a, 0, SVAL_QEXPR, "tail");
    SASSERT_QEXPR_NONEMPTY(a, 0, "tail");

    sval *v = sval_take(a, 0);
    sval_del(sval_pop(v, v->count-1));

    return v;
}

void senv_add_builtin(senv* e, char* name, sbuiltin func) {
    sval* k = sval_sym(name);
    sval* v = sval_fun(name, func);
    senv_put(e, k, v);
    sval_del(v); sval_del(k);
}

void senv_add_builtins(senv* e) {
    /* list functions */
    senv_add_builtin(e, "list", builtin_list);
    senv_add_builtin(e, "head", builtin_head);
    senv_add_builtin(e, "tail", builtin_tail);
    senv_add_builtin(e, "eval", builtin_eval);
    senv_add_builtin(e, "join", builtin_join);
    senv_add_builtin(e, "cons", builtin_cons);
    senv_add_builtin(e, "len",  builtin_len);
    senv_add_builtin(e, "init", builtin_init);

    /* variable functions */
    senv_add_builtin(e, "\\",  builtin_lambda);
    senv_add_builtin(e, "def", builtin_def);
    senv_add_builtin(e, "env", builtin_env);
    senv_add_builtin(e, "=",   builtin_put);

    /* mathematical functions */
    senv_add_builtin(e, "+", builtin_add);
    senv_add_builtin(e, "-", builtin_sub);
    senv_add_builtin(e, "*", builtin_mul);
    senv_add_builtin(e, "/", builtin_div);

    /* comparator functions */
    senv_add_builtin(e, "if", builtin_if);
    senv_add_builtin(e, ">",  builtin_gt);
    senv_add_builtin(e, "gt", builtin_gt);
    senv_add_builtin(e, "<",  builtin_lt);
    senv_add_builtin(e, "lt", builtin_lt);
    senv_add_builtin(e, "<=", builtin_ge);
    senv_add_builtin(e, "ge", builtin_ge);
    senv_add_builtin(e, ">=", builtin_le);
    senv_add_builtin(e, "le", builtin_le);
    senv_add_builtin(e, "/=", builtin_ne);
    senv_add_builtin(e, "ne", builtin_ne);
    senv_add_builtin(e, "==", builtin_eq);
    senv_add_builtin(e, "eq", builtin_eq);

}

sval* builtin_def(senv* e, sval* a) {
    return builtin_var(e, a, "def");
}

sval* builtin_put(senv* e, sval* a) {
    return builtin_var(e, a, "=");
}

sval* builtin_lambda(senv* e, sval* a) {
    SASSERT_ARG_COUNT(a, 2, "\\");
    SASSERT_ARG_TYPE(a, 0, SVAL_QEXPR, "\\");
    SASSERT_ARG_TYPE(a, 1, SVAL_QEXPR, "\\");

    /* check first q-expression contains only symbols */
    for (int i = 0; i < a->cell[0]->count; i++) {
        SASSERT(a, (a->cell[0]->cell[i]->type == SVAL_SYM),
            "cannot define non-symbol. got %s, expected %s.",
            stype_name(a->cell[0]->cell[i]->type),stype_name(SVAL_SYM));
    }

    /* pop first two arguments and pass them to sval_lambda */
    sval* formals = sval_pop(a, 0);
    sval* body = sval_pop(a, 0);
    sval_del(a);

    return sval_lambda(formals, body);
}

sval* builtin_env(senv* e, sval* a) {
    SASSERT_ARG_COUNT(a, 0, "env");

    sval* v = sval_qexpr();
    for (int i = 0; i < e->count; i++) {
        sval_add(v, e->vals[i]);
    }
    return v;
}
