#if 0
cexcept.h amc.0.1.1 (2000-Mar-04-Sat)
Adam M. Costello <amc@cs.berkeley.edu>

An interface for exception-handling in ANSI C.

#endif


#ifndef CEXCEPT_H
#define CEXCEPT_H


#ifndef CEXCEPT_CUSTOM_EXCEPTION
struct exception {
  int code;
  const char *msg;
  void *who;
  void *what;
};
#endif

    /* A struct exception is what gets copied from the exception */
    /* thrower to the exception catcher.  Applications are       */
    /* welcome to change this declaration, either by editing     */
    /* this file or by declaring struct exception and defining   */
    /* CEXCEPT_CUSTOM_EXCEPTION before including this file.      */
    /* There must be a member called "code", which must have an  */
    /* integral type or a pointer type.                          */


struct exception_context;

    /* A struct exception_context must be known to both the thrower   */
    /* and the catcher.  It is expected that there be one struct      */
    /* exception_context for each thread that uses exceptions.  It    */
    /* would certainly be dangerous for multiple threads to access    */
    /* the same context, and would probably not be useful for one     */
    /* thread to use multiple contexts.  The structure members will   */
    /* be declared below, so that the application can allocate this   */
    /* structure (in any way it pleases--automatic, static, or        */
    /* dynamic), but the application programmer should pretend not to */
    /* know the members, which are subject to change.                 */


#if 0

The rest of the interface consists of four macros:


USE_EXCEPTIONS(struct exception_context *ec)

    This macro is used like an internal declaration (it may appear only
    in the declaration sections of blocks).  The other macros
    may appear only within the scope of such a declaration.
    Typically, this declaration would go near the top of each function
    that either throws or catches exceptions, but it may appear in
    nested blocks instead (or in addition to).


void INIT_EXCEPTIONS(void)

    This macro is used like a function.  It must be called once
    after the struct exception_context is allocated, before the first
    BEGIN_CATCH is encountered.


BEGIN_CATCH
END_CATCH(struct exception e)

    These macros are used like braces--they begin and end a block,
    called a "catch block", which may begin with declarations just like
    a regular block.  Unlike the other macros, these two should not
    be followed by semicolons.  Any exception thrown within the block
    (usually by a function called from within the block) causes control
    to jump to the end of the block, and e will contain a copy of the
    exception.  If no exception is thrown, e.code will be 0.  IMPORTANT:
    return and goto must not be used to jump out of a catch block--the
    END_CATCH must be reached.  And the values of any non-volatile
    automatic variables changed within the catch block are undefined if
    an exception is caught.


void THROW(struct exception e)

    This macro is used like a function that does not return.  The
    thrower must assign a non-zero value to e.code if it wants to make
    sure that the catcher knows that an exception occurred.  Of course,
    the thrower may also assign values to the other members of e.  The
    exception must be caught, otherwise the program may crash.


Everything below this point is for the benefit of the compiler.  The
application programmer should pretend not to know any of it, because it
is subject to change.

#endif


#include <setjmp.h>

struct exception_context {
  struct exception_jmp_buf *last;
  struct exception tmp;
};

struct exception_jmp_buf {
  jmp_buf env;
};

#define USE_EXCEPTIONS(ec) struct exception_context *exception__c = (ec)

#define INIT_EXCEPTIONS() ((void)(exception__c->last = 0))

#define  BEGIN_CATCH { \
  struct exception_jmp_buf *exception__p = exception__c->last, exception__j; \
  exception__c->last = &exception__j; \
  if (setjmp(exception__j.env) == 0) {

#define END_CATCH(e) \
    (e).code = 0; \
  } else (e) = exception__c->tmp; \
  exception__c->last = exception__p; }

#define THROW(e) (exception__c->tmp = (e), longjmp(exception__c->last->env, 1))


#endif /* CEXCEPT_H */
