#ifndef PARSING_H
#define PARSING_H

#include "mpc/mpc.h"
#include "read.h"
#include "print.h"
#include "evaluate.h"
#include "types.h"

mpc_parser_t* integer;
mpc_parser_t* decimal;
mpc_parser_t* number;
mpc_parser_t* string;
mpc_parser_t* comment;
mpc_parser_t* symbol;
mpc_parser_t* qexpr;
mpc_parser_t* sexpr;
mpc_parser_t* expr;
mpc_parser_t* slur;

#ifdef _WIN32
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

/* Fake readline function */
char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, sizeof buffer, stdin);

    size_t bufsiz = strlen(buffer);
    char* cpy = malloc(bufsiz + 1);
    SASSERT_ALLOC_MEM(cpy);
    strcpy(cpy, buffer);
    cpy[bufsize-1] = '\0';
    return cpy;
}

/* Fake add_history function */
void add_history(char* unused) {}

#ifdef _linux
#include <editline/history.h>
#endif

#else
#include <editline/readline.h>
#endif

#endif
