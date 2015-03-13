#include "print.h"

void sval_expr_print(sval* v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->count; i++) {

        /* print cell within */
        sval_print(v->cell[i]);

        /* space after all but last element */
        if (i != (v->count-1)) {
            putchar(' ');
        }
    }
    putchar(close);
}
void sval_print(sval* v) {
    switch (v->type) {
        case SVAL_INT:   printf("%li", v->num.nt); break;
        case SVAL_DEC:   printf("%f", v->num.dec); break;
        case SVAL_ERR:   fprintf(stderr, "Error: %s", v->err); break;
        case SVAL_SYM:   printf("%s", v->sym); break;
        case SVAL_SEXPR: sval_expr_print(v, '(', ')'); break;
        case SVAL_QEXPR: sval_expr_print(v, '{', '}'); break;
        case SVAL_FUN:
        if (v->builtin) {
            printf("<builtin>");
        } else {
            printf("(\\ "); sval_print(v->formals);
            putchar(' '); sval_print(v->body); putchar(')');
        }
        break;
    }
}

void sval_println(sval* v) { sval_print(v); putchar('\n'); }
