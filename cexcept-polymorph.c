/*===
cexcept-polymorph.c amc.0.2.0 (2000-Mar-05-Sun)
Adam M. Costello <amc@cs.berkeley.edu>

An example application that demonstrates how to use the cexcept.h
interface (version amc.0.2.*) to provide polymorphic exceptions.

See also cexcept-example.c.

===*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* The following declarations would normally go in a separate .h file: */

enum exception_flavor { okay, oops, screwup, barf };

struct exception {
  enum exception_flavor flavor;
  const char *msg;
  union {
    int oops;
    long screwup;
    char barf[8];
  } info;
};

#include "cexcept.h"
DEFINE_EXCEPTION_TYPE(struct exception);

/* End of separate .h file. */


void demo_throw(struct exception_context *ec)
{
  static int count = 0;
  USE_EXCEPTIONS(ec);
  struct exception e;

  fprintf(stderr, "enter demo_throw\n");

  ++count;

  if (count == 2) {
    e.flavor = oops;
    e.msg = "demo oops message";
    e.info.oops = 17;
    THROW(e);
  }
  else if (count == 3) {
    e.flavor = barf;
    e.msg = "demo barf message";
    strcpy(e.info.barf, "ABCDEFG");
    THROW(e);
  }
  else if (count == 4) {
    e.flavor = screwup;
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

  TRY foo(ec);
  CATCH(e) {
    switch (e.flavor) {
      case okay: break;
      case oops: fprintf(stderr, "bar caught oops (info == %d): %s\n",
                         e.info.oops, e.msg);
                 break;
        default: THROW(e);
    }
  }

  fprintf(stderr, "return from bar\n");
}


int main()
{
  struct exception_context ec[1];
  USE_EXCEPTIONS(ec);
  struct exception e;

  INIT_EXCEPTIONS();

  TRY {
    bar(ec);  /* no exceptions */
    bar(ec);  /* exception will be caught by bar(), looks okay to us */
    bar(ec);  /* bar() will rethrow the exception */
    fprintf(stderr, "we won't get here\n");
  }
  CATCH(e) {
    switch (e.flavor) {
         case okay: break;
         case barf: fprintf(stderr, "main caught barf (info == %s): %s\n",
                            e.info.barf, e.msg);
                    break;
      case screwup: fprintf(stderr, "main caught screwup (info == %ld): %s\n",
                            e.info.screwup, e.msg);
           default: fprintf(stderr, "main caught unknown exception\n");
    }
  }

  return EXIT_SUCCESS;
}
