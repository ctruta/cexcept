/*===
cexcept-example.c amc.0.2.1 (2000-Mar-05-Sun)
Adam M. Costello <amc@cs.berkeley.edu>

An example application that demonstrates how to use the cexcept.h
interface (version amc.0.2.*).

This application is single-threaded and allocates the struct
exception_context on the stack in main(), then passes it as an
argument to functions.  Other options would be to allocate the storage
dynamically, or to use a global variable (only in single-threaded
applications), or to include the struct exception_context (or a pointer
to it) inside some larger thread state structure that gets passed to
functions.

===*/


#include <stdio.h>
#include <stdlib.h>


/* The following declarations would normally go in a separate .h file: */

#include "cexcept.h"
DEFINE_EXCEPTION_TYPE(int);

/* End of separate .h file. */


void demo_throw(struct exception_context *ec, int fail)
{
  USE_EXCEPTIONS(ec);

  fprintf(stderr, "enter demo_throw(%d)\n", fail);
  if (fail) THROW(42);
  fprintf(stderr, "return from demo_throw(%d)\n", fail);
}


void foo(struct exception_context *ec, int fail)
{
  fprintf(stderr, "enter foo(%d)\n", fail);
  demo_throw(ec,fail);
  fprintf(stderr, "return from foo(%d)\n", fail);
}


int main()
{
  struct exception_context ec[1];
  USE_EXCEPTIONS(ec);
  int e;

  INIT_EXCEPTIONS();

  TRY {
    foo(ec,0);
    foo(ec,1);
    foo(ec,2);
  }
  CATCH(e) fprintf(stderr, "exception %d\n", e);

  return EXIT_SUCCESS;
}
