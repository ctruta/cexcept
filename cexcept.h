/*===
cexcept.h amc.0.5.0 (2000-Mar-19-Sun)
Adam M. Costello <amc@cs.berkeley.edu>

An interface for exception-handling in ANSI C, based on the ideas of
Cosmin Truta <cosmin@cs.toronto.edu>.

You don't normally want .c files to include this header file directly.
Instead, create a wrapper header file that includes this header file and
then invokes the define_exception_type macro (see below), and let your
.c files include that header file.

The interface consists of one type, one well-known name, and five
macros.


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


struct exception_context *the_exception_context;

    It is the application's responsibility to make sure that the
    name the_exception_context refers to the address of a mutable
    (non-constant) struct exception_context before invoking any of the
    macros described below.  Subject to that constraint, the application
    may declare a variable of that name anywhere it likes (inside a
    function, in a parameter list, or externally), and may use whatever
    type qualifiers it likes (static, extern, const, etc).  Examples:

    static struct exception_context * const the_exception_context = &foo;
    { struct exception_context *the_exception_context = bar; ... }
    int blah(struct exception_context *the_exception_context, ...) ...
    extern struct exception_context the_exception_context[1];

    The last example illustrates a trick that avoids creating a pointer
    object separate from the structure object.

    The name could even be a macro, for example:

    struct exception_context *get_ec(void);
    #define the_exception_context get_ec()


void init_exception_context(void);

    This macro is used like a function.  It must be called once after
    the struct exception_context is allocated, before the first
    Try/Catch statement is encountered.


Try statement
Catch (expression) statement

    The Try/Catch/Throw macros are capitalized in order to avoid
    confusion with the C++ keywords, which have subtly different
    semantics.

    A Try/Catch statement has a syntax similar to an if/else statement,
    except that the parenthesized expression goes after the second
    keyword rather than the first.  As with if/else, there are two
    clauses, each of which may be a simple statement ending with a
    semicolon or a brace-enclosed compound statement.  But whereas
    the else clause is optional, the Catch clause is required.  The
    expression must be an lvalue (something capable of being assigned
    to) compatible with the type passed to define_exception_type().

    If a Throw that uses the same exception context as the Try/Catch is
    executed within the Try clause (typically within a function called
    by the Try clause), and the exception is not caught by a nested
    Try/Catch statement, then a copy of the exception will be assigned
    to the expression, and control will jump to the Catch clause.  If no
    such Throw is executed, expr is not modified, and the Catch clause
    is not executed.

    IMPORTANT: return and goto must not be used to jump out of a
    Try clause--the Catch must be reached.  Also, the values of any
    non-volatile automatic variables changed within the Try clause are
    undefined after an exception is caught.


Throw expression;

    A Throw statement is very much like a return statement, except that
    the expression is required.  Whereas return jumps back to the place
    where the current function was called, Throw jumps back to the Catch
    clause of the innermost enclosing Try clause.  The expression must
    be compatible with the type passed to define_exception_type().  The
    exception must be caught, otherwise the program may crash.

    Slight limitation:  The expression cannot be a comma-expression (but
    no one would want to use a comma-expression here anyway).


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

#define init_exception_context() ((void)(the_exception_context->last = 0))

#define Try \
  { \
    struct exception__jmp_buf *exception__p, exception__j; \
    exception__p = the_exception_context->last; \
    the_exception_context->last = &exception__j; \
    if (setjmp(exception__j.env) == 0) { \
      if (1)

#define Catch(e) \
      else { } \
      the_exception_context->caught = 0; \
    } \
    else { \
      (e) = the_exception_context->tmp; \
      the_exception_context->caught = 1; \
    } \
    the_exception_context->last = exception__p; \
  } \
  if (!the_exception_context->caught) { } \
  else

/* Try ends with if(), and Catch begins and ends with else.  */
/* This ensures that the Try/Catch syntax is really the same */
/* as the if/else syntax.                                    */

#define Throw \
  for (;; longjmp(the_exception_context->last->env, 1)) \
    the_exception_context->tmp =


#endif /* CEXCEPT_H */
