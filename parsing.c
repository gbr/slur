#include "parsing.h"

int main(int argc, char** argv) {

    const char* SLUR        = "Slur";
    const char* EXIT_HINT   = "Press Ctrl+C to exit";
    const char* REPL_PROMPT = "slur> ";

  /* Parser definitions */
  mpc_parser_t* integer = mpc_new("integer");
  mpc_parser_t* decimal = mpc_new("decimal");
  mpc_parser_t* number  = mpc_new("number");
  mpc_parser_t* symbol  = mpc_new("symbol");
  mpc_parser_t* qexpr   = mpc_new("qexpr");
  mpc_parser_t* sexpr   = mpc_new("sexpr");
  mpc_parser_t* expr    = mpc_new("expr");
  mpc_parser_t* slur    = mpc_new("slur");

  /* Grammar definition */
  static const char* grammar = "                            \
      integer : /-?[0-9]+/ ;                                \
      decimal : /-?[0-9]*\\.[0-9]+/;                        \
      number  : <decimal> | <integer> ;                     \
      symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;          \
      qexpr   : '{' <expr>* '}' ;                           \
      sexpr   : '(' <expr>* ')' ;                           \
      expr    : <number> | <symbol> | <sexpr> | <qexpr> ;   \
      slur    : /^/ <expr>* /$/ ;                           \
      ";

  mpca_lang(MPCA_LANG_DEFAULT, grammar, integer, decimal,
    number, symbol, qexpr, sexpr, expr, slur);

  senv* e = senv_new();
  senv_add_builtins(e);

  printf("%s %s\n", SLUR, VERSION);
  printf("%s\n", EXIT_HINT);

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
 senv_del(e);

 /* undefine and delete parsers */
 mpc_cleanup(5, number, symbol, sexpr, expr, slur);

 return 0;
}
