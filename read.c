#include "read.h"

sval* sval_read_int(mpc_ast_t* t) {
    errno = 0;
    long x = strtol(t->contents, NULL, BASE);
    return errno != ERANGE
    ? sval_int(x)
    : sval_err("%s is an invalid number", t->contents);
}

sval* sval_read_dec(mpc_ast_t* t) {
    errno = 0;
    double x = strtod(t->contents, NULL);
    return errno != ERANGE
    ? sval_dec(x)
    : sval_err("%s is an invalid number", t->contents);
}

sval* sval_read(mpc_ast_t* t) {

    /* directly convert numbers and symbols */
    if (strstr(t->tag, "integer")) { return sval_read_int(t); }
    if (strstr(t->tag, "decimal")) { return sval_read_dec(t); }
    if (strstr(t->tag, "symbol")) { return sval_sym(t->contents); }

    /* if root (>) or sexpr then create empty list */
    sval* x = NULL;
    if (strcmp(t->tag, ">") == 0) { x = sval_sexpr(); }
    if (strstr(t->tag, "sexpr"))  { x = sval_sexpr(); }
    if (strstr(t->tag, "qexpr"))  { x = sval_qexpr(); }

    /* fill list with valid expressions */
    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
            if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
            if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
            if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
            if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
            x = sval_add(x, sval_read(t->children[i]));
        }

        return x;
    }

