/*===
cexcept-example.c amc.0.3.0 (2000-Mar-05-Sun)
Adam M. Costello <amc@cs.berkeley.edu>

An example application that demonstrates how to use the cexcept.h
interface (version amc.0.3.*).

This application is single-threaded and uses a global exception context.

See cexcept-example2.c for an example of avoiding global variables by
passing the context in function arguments, and an example of using a
polymorphic exception type.

===*/


#include <stdio.h>
#include <stdlib.h>


/* The following declarations would normally go in a separate .h file: */

#include "cexcept.h"
define_exception_type(int);
extern struct exception_context exception_context[1];

/* End of separate .h file. */


void demo_throw(int fail)
{
  fprintf(stderr, "enter demo_throw(%d)\n", fail);
  if (fail) throw(42);
  fprintf(stderr, "return from demo_throw(%d)\n", fail);
}


void foo(int fail)
{
  fprintf(stderr, "enter foo(%d)\n", fail);
  demo_throw(fail);
  fprintf(stderr, "return from foo(%d)\n", fail);
}


/* Globally accessible storage for the exception context: */

struct exception_context exception_context[1];


int main()
{
  int e;

  init_exceptions();

  try {
    foo(0);
    foo(1);
    foo(2);
  }
  catch(e) fprintf(stderr, "exception %d\n", e);

  return EXIT_SUCCESS;
}
