/*===
cexcept.h amc.0.3.0 (2000-Mar-05-Sun)
Adam M. Costello <amc@cs.berkeley.edu>

An interface for exception-handling in ANSI C.

You don't normally want .c files to include this header file directly.
Instead, create your own header file that includes this header file and
then invokes the define_exception_type macro (see below), and let your
.c files include that header file.

The interface consists of one type, one well-known variable name, and
five macros.


define_exception_type(type_name);

    This macro is used like an external declaration.  It specifies the
    type of object that gets copied from the exception thrower to the
    exception catcher.  The type_name must refer to a complete type (as
    opposed to an incomplete struct/union/array type).


struct exception_context;

    This type is declared by the define_exception_type() macro.  A
    struct exception_context must be known to both the thrower and the
    catcher.  It is expected that there be one struct exception_context
    for each thread that uses exceptions.  It would certainly be
    dangerous for multiple threads to access the same context, and
    would probably not be useful for one thread to use multiple
    contexts.  The application can allocate this structure in any way it
    pleases--automatic, static, or dynamic.  The application programmer
    should pretend not to know the structure members, which are subject
    to change.


struct exception_context *exception_context;

    It is the application's responsibility to make sure that the
    variable name exception_context refers to the address of a mutable
    (non-constant) struct exception_context before invoking any of the
    macros described below.  Subject to that constraint, the application
    may declare the variable anywhere it likes (inside a function, in a
    parameter list, or externally), and may use whatever type qualifiers
    it likes (static, extern, const, etc).  For example:

    static struct exception_context * const exception_context = &foo;
    { struct exception_context *exception_context = bar; ... }
    int blah(struct exception_context *exception_context, ...) ...
    extern struct exception_context exception_context[1];

    The last example illustrates a trick that avoids creating a pointer
    object separate from the structure object.


void init_exceptions(void);

    This macro is used like a function.  It must be called once after
    the struct exception_context is allocated, before the first try is
    encountered.


try statement
catch(e) statement

    These macros provide a new statement syntax.  As with if/else,
    each statement may be a simple statement ending with a semicolon
    or a brace-enclosed compound statement.  Unlike else, the catch
    is required.  The type of e must match the type passed to
    define_exception_type().

    If a throw() that uses the same exception context as the try/catch
    is executed within the try statement (typically within a function
    called by the statement), a copy of the exception will be assigned
    to e, and control will jump to the catch statement.  If no such
    throw() is executed, e is not modified, and the catch statement is
    not executed.

    IMPORTANT: return and goto must not be used to jump out of a try
    statement--the catch must be reached.  Also, the values of any
    non-volatile automatic variables changed within the try statement
    are undefined after an exception is caught.


void throw(e);

    This macro is used like a function that does not return.  The type
    of e must match the type passed to define_exception_type().  The
    exception must be caught, otherwise the program may crash.


Everything below this point is for the benefit of the compiler.  The
application programmer should pretend not to know any of it, because it
is subject to change.

===*/


#ifndef CEXCEPT_H
#define CEXCEPT_H


#include <setjmp.h>

#define define_exception_type(etype) \
struct exception_context { \
  struct exception__jmp_buf *last; \
  int caught; \
  etype tmp; \
}

struct exception__jmp_buf {
  jmp_buf env;
};

#define init_exceptions() ((void)(exception_context->last = 0))

#define try \
  { \
    struct exception__jmp_buf *exception__p, exception__j; \
    exception__p = exception_context->last; \
    exception_context->last = &exception__j; \
    if (setjmp(exception__j.env) == 0) { \
      if (1)

#define catch(e) \
      else { } \
      exception_context->caught = 0; \
    } \
    else { \
      (e) = exception_context->tmp; \
      exception_context->caught = 1; \
    } \
    exception_context->last = exception__p; \
  } \
  if (exception_context->caught)

#define throw(e) \
  (exception_context->tmp = (e), longjmp(exception_context->last->env, 1))


#endif /* CEXCEPT_H */
