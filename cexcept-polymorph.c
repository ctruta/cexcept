#if 0
cexcept-polymorph.c amc.0.1.1 (2000-Mar-04-Sat)
Adam M. Costello <amc@cs.berkeley.edu>

An example application that demonstrates how to use the cexcept.h
interface (version amc.0.1.*) to provide polymorphic exceptions.

See also cexcept-example.c.

#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CEXCEPT_CUSTOM_EXCEPTION

enum exception_flavor { okay, oops, screwup, barf };

struct exception {
  enum exception_flavor code;
  const char *msg;
  union {
    int oops;
    long screwup;
    char barf[8];
  } info;
};

#include "cexcept.h"


void demo_throw(struct exception_context *ec)
{
  static int count = 0;
  USE_EXCEPTIONS(ec);
  struct exception e;

  fprintf(stderr, "enter demo_throw\n");

  ++count;

  if (count == 2) {
    e.code = oops;
    e.msg = "demo oops message";
    e.info.oops = 17;
    THROW(e);
  }
  else if (count == 3) {
    e.code = barf;
    e.msg = "demo barf message";
    strcpy(e.info.barf, "ABCDEFG");
    THROW(e);
  }
  else if (count == 4) {
    e.code = screwup;
    e.msg = "demo screwup message";
    e.info.screwup = 987654321;
    THROW(e);
  }

  fprintf(stderr, "return from demo_throw\n");
}


void foo(struct exception_context *ec)
{
  fprintf(stderr, "enter foo\n");
  demo_throw(ec);
  fprintf(stderr, "return from foo\n");
}


void bar(struct exception_context *ec)
{
  USE_EXCEPTIONS(ec);
  struct exception e;

  fprintf(stderr, "enter bar\n");

  BEGIN_CATCH
    foo(ec);
  END_CATCH(e)

  switch (e.code) {
    case okay: break;
    case oops: fprintf(stderr, "bar caught oops (info == %d): %s\n",
                       e.info.oops, e.msg);
               break;
      default: THROW(e);
  }

  fprintf(stderr, "return from bar\n");
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
    bar(ec);  /* no exceptions */
    bar(ec);  /* exception will be caught by bar(), looks okay to us */
    bar(ec);  /* bar() will rethrow the exception */
    fprintf(stderr, "we won't get here\n");
  END_CATCH(e)

  switch (e.code) {
       case okay: break;
       case barf: fprintf(stderr, "main caught barf (info == %s): %s\n",
                          e.info.barf, e.msg);
                  break;
    case screwup: fprintf(stderr, "main caught screwup (info == %ld): %s\n",
                          e.info.screwup, e.msg);
         default: fprintf(stderr, "main caught unknown exception\n");
  }

  return EXIT_SUCCESS;
}
