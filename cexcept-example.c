#if 0
cexcept-example.c amc.0.1.1 (2000-Mar-03-Fri)
Adam M. Costello <amc@cs.berkeley.edu>

An example application that demonstrates how to use the cexcept.h
interface (version amc.0.1.*).

This application is single-threaded and allocates the struct
exception_context on the stack in main(), then passes it as an
argument to functions.  Other options would be to allocate the storage
dynamically, or to use a global variable (only in single-threaded
applications), or to include the struct exception_context (or a pointer
to it) inside some larger thread state structure that gets passed to
functions.

#endif


#include <stdio.h>
#include <stdlib.h>

#include "cexcept.h"


void demo_throw(struct exception_context *ec, int fail)
{
  USE_EXCEPTIONS(ec);
  struct exception e;

  fprintf(stderr, "enter demo_throw(%d)\n", fail);

  if (fail) {
    e.code = 42;
    e.msg = "demo message";
    THROW(e);
  }

  fprintf(stderr, "return from demo_throw(%d)\n", fail);
}


void foo(struct exception_context *ec, int fail)
{
  fprintf(stderr, "enter foo(%d)\n", fail);
  demo_throw(ec,fail);
  fprintf(stderr, "return from foo(%d)\n", fail);
}


main()
{
  struct exception_context ec[1];

  /* That's a cute way to allocate space and create a convenient */
  /* name for its address, all in one step.  It can also be done */
  /* inside structures.                                          */

  USE_EXCEPTIONS(ec);
  struct exception e;

  INIT_EXCEPTIONS();

  BEGIN_CATCH
    foo(ec,0);
    foo(ec,1);
    foo(ec,2);
  END_CATCH(e)

  if (e.code) fprintf(stderr, "exception %d: %s\n", e.code, e.msg);
  return EXIT_SUCCESS;
}
