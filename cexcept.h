/*===
cexcept.h amc.0.2.1 (2000-Mar-05-Sun)
Adam M. Costello <amc@cs.berkeley.edu>

An interface for exception-handling in ANSI C.

You don't normally want .c files to include this header file directly.
Instead, create your own header file that includes this header file and
then invokes the DEFINE_EXCEPTION_TYPE macro (see below), and let your
.c files include that header file.

The interface consists of one type and five macros.

struct exception_context;

    A struct exception_context must be known to both the thrower and the
    catcher.  It is expected that there be one struct exception_context
    for each thread that uses exceptions.  It would certainly be
    dangerous for multiple threads to access the same context, and
    would probably not be useful for one thread to use multiple
    contexts.  The application can allocate this structure (in any way
    it pleases--automatic, static, or dynamic), but the application
    programmer should pretend not to know the members, which are
    subject to change.  A convenient way to define the storage is as a
    one-element array:

    struct exception_context foo[1];

    This way, foo is a pointer to the structure, which is what you need
    to pass to USE_EXCEPTIONS().


DEFINE_EXCEPTION_TYPE(type_name);

    This macro is used like an external declaration.  It specifies the
    type of object that gets copied from the exception thrower to the
    exception catcher.  The type_name must refer to a complete type (as
    opposed to an incomplete struct/union/array type).


USE_EXCEPTIONS(struct exception_context *ec);

    This macro is used like an internal declaration (it may appear only
    in the declaration sections of blocks).  The macros described below
    may appear only within the scope of such a declaration.  Typically,
    this declaration would go near the top of each function that either
    throws or catches exceptions, but it may appear in nested blocks
    instead (or in addition to).


void INIT_EXCEPTIONS(void);

    This macro is used like a function.  It must be called once after
    the struct exception_context is allocated, before the first TRY is
    encountered.


TRY statement
CATCH(e) statement

    These macros provide a new statement syntax.  As with if/else,
    each statement may be a simple statement ending with a semicolon
    or a brace-enclosed compound statement.  Unlike else, the CATCH
    is required.  The type of e must match the type passed to
    DEFINE_EXCEPTION_TYPE().

    If an exception is thrown within the TRY statement (typically
    by a function called from within the statement), a copy of the
    exception will be assigned to e, and control will jump to the CATCH
    statement.  If no exception is thrown, e is not modified, and the
    CATCH statement is not executed.

    IMPORTANT: return and goto must not be used to jump out of a TRY
    statement--the CATCH must be reached.  Also, the values of any
    non-volatile automatic variables changed within the TRY statement
    are undefined after an exception is caught.


void THROW(e);

    This macro is used like a function that does not return.  The type
    of e must match the type passed to DEFINE_EXCEPTION_TYPE().  The
    exception must be caught, otherwise the program may crash.


Everything below this point is for the benefit of the compiler.  The
application programmer should pretend not to know any of it, because it
is subject to change.

===*/


#ifndef CEXCEPT_H
#define CEXCEPT_H


#include <setjmp.h>

#define DEFINE_EXCEPTION_TYPE(etype) \
struct exception_context { \
  struct exception__jmp_buf *last; \
  int caught; \
  etype tmp; \
}

struct exception__jmp_buf {
  jmp_buf env;
};

#define USE_EXCEPTIONS(ec) struct exception_context *exception__c = (ec)

#define INIT_EXCEPTIONS() ((void)(exception__c->last = 0))

#define TRY \
  { \
    struct exception__jmp_buf *exception__p, exception__j; \
    exception__p = exception__c->last; \
    exception__c->last = &exception__j; \
    if (setjmp(exception__j.env) == 0) { \
      if (1)

#define CATCH(e) \
      else { } \
      exception__c->caught = 0; \
    } \
    else { \
      (e) = exception__c->tmp; \
      exception__c->caught = 1; \
    } \
    exception__c->last = exception__p; \
  } \
  if (exception__c->caught)

#define THROW(e) (exception__c->tmp = (e), longjmp(exception__c->last->env, 1))


#endif /* CEXCEPT_H */
