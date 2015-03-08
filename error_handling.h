#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <stdlib.h>
#include <string.h>

#include "types.h"

#define SASSERT(args, cond, fmt, ...) \
  do { \
    if (!(cond)) { \
      sval* err = sval_err(fmt, ##__VA_ARGS__); \
      sval_del(args); \
      return err; \
    } \
  } while (0)

#define SASSERT_ARG_COUNT(args, num, func) \
  do { \
	  SASSERT(args, args->count == num, \
	  	"function '%s' passed incorrect number of arguments. got %i, expected %i.", \
      func, args->count, num); \
  } while (0)

#define SASSERT_ARG_TYPE(args, argi, expect, func) \
  do { \
    SASSERT(args, args->cell[argi]->type == expect, \
      "function '%s' passed the incorrect type for argument %i. got %s, expected %s", \
      func, argi, stype_name(args->cell[argi]->type), stype_name(expect)); \
  } while (0)

#define SASSERT_QEXPR_NONEMPTY(args, argi, func) \
  do { \
    SASSERT(args, args->cell[argi]->count != 0, \
      "function '%s' passed {} for argument %i", func, argi); \
  } while (0)

#define SASSERT_ALLOC_MEM(ptr) \
  do { \
  	if (!ptr) { \
      fprintf(stderr, "memory failure\n"); \
      exit(EXIT_FAILURE); \
    } \
  } while (0)

#endif
