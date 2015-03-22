#include "parsing.h"

int main(int argc, char** argv) {

    const char* SLUR        = "Slur";
    const char* EXIT_HINT   = "Press Ctrl+C to exit";
    const char* REPL_PROMPT = "slur> ";

    /* Parser definitions */
    integer = mpc_new("integer");
    decimal = mpc_new("decimal");
    number  = mpc_new("number");
    string  = mpc_new("string");
    comment = mpc_new("comment");
    symbol  = mpc_new("symbol");
    qexpr   = mpc_new("qexpr");
    sexpr   = mpc_new("sexpr");
    expr    = mpc_new("expr");
    slur    = mpc_new("slur");

    /* Grammar definition */
    static const char* grammar = "                            \
        integer : /-?[0-9]+/ ;                                \
        decimal : /-?[0-9]*\\.[0-9]+/;                        \
        number  : <decimal> | <integer> ;                     \
        symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;          \
        string  : /\"(\\\\.|[^\"])*\"/ ;                      \
        comment : /;[^\\r\\n]*/ ;                             \
        qexpr   : '{' <expr>* '}' ;                           \
        sexpr   : '(' <expr>* ')' ;                           \
        expr    : <number>  | <symbol> | <string>             \
                | <comment> | <sexpr>  | <qexpr> ;            \
        slur    : /^/ <expr>* /$/ ;                           \
        ";

    mpca_lang(MPCA_LANG_DEFAULT, grammar, integer, decimal,
      number, symbol, string, comment, qexpr, sexpr, expr, slur);

    senv* e = senv_new();
    senv_add_builtins(e);

    printf("%s version %s\n", SLUR, VERSION);
    printf("%s\n", EXIT_HINT);

    if (argc == 1) {
        for (;;) {
            char* input = readline(REPL_PROMPT);
            add_history(input);

            /* parse input */
            mpc_result_t res;
            if (mpc_parse("<stdin>", input, slur, &res)) {
                /* success */
                sval* x = sval_eval(e, sval_read(res.output));
                sval_println(x);
                sval_del(x);
                mpc_ast_delete(res.output);
            } else {
                /* failure */
               mpc_err_print(res.error);
               mpc_err_delete(res.error);
           }

           free(input);
       }
    } else if (argc >= 2) {

        /* loop through filename args */
        for (int i = 1; i < argc; i++) {

            sval* args = sval_add(sval_sexpr(), sval_str(argv[i]));
            sval* x = builtin_load(e, args);

            if (x->type == SVAL_ERR) { sval_println(x); }
            sval_del(x);
        }
    }

   senv_del(e);

   /* undefine and delete parsers */
   mpc_cleanup(10, integer, decimal, number, symbol,
           string, comment, qexpr, sexpr, expr, slur);

   return 0;
}
