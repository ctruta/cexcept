# cexcept

## Design Rationale

Questions answered below:

 * Why aren't multiple Catch clauses allowed?
 * Why can't I say `Catch(int e)` as in C++ or Java?
 * Why can't I jump into or out of a Try clause?
 * Why isn't a Finally clause supported?
 * Why can't I use Throw without an expression to re-throw an exception?
 * Why can't "real" exceptions be caught?
 * Why must the Catch expression have the same type that was passed to
   `define_exception_type()`?  I can assign a double value to an int,
   so why can't I catch a double exception in an int variable?
 * Why does the implementation copy the exception twice?
 * Why are the macros spelled Try, Catch, Throw rather than...?


### Why aren't multiple Catch clauses allowed?

The purpose of multiple catch clauses in C++ or Java is to allow you
to catch multiple exception types.  This is natural in a language
that supports polymorphism, where you can substitute objects of
different types into the same expression and cause different code to
be executed.  In such a language you can throw any type of object,
and the appropriate catch clause will be selected automatically.

C does not support polymorphism (except for unions and void
pointers).  Therefore it is natural in C to have just one exception
type, and therefore one Catch clause is sufficient.  The application
programmer is free to use any type for the exception type, and may
use a union to simulate polymorphism (see example2.c for an example
of this).  But for many applications an int, enum, or small struct
will be sufficient.


### But couldn't tricks be played to allow multiple exception types?

Yes, but cexcept.h is intended for C programs and C programmers.  If
the concept of polymorphism were incorporated into the interface,
the interface would become less intuitive to many of the people
it is designed for.  But if an application programmer uses unions
to simulate multiple exception types, the resulting code will be
perfectly understandable to other C programmers, even if they have
no experience with polymorphism.


### Why can't I say `Catch(int e)` as in C++ or Java?

Again, the interface tries to be intuitive to C programmers.  In C,
declarations go at the beginnings of blocks.  The cexcept syntax is
actually more flexible in a way, because any lvalue can go in the
parentheses, for example: `Catch(p->e)`.


### Why can't I jump into or out of a Try clause?

There is some hidden setup and cleanup that needs to be done
regardless of whether an exception is caught.  Jumping into a Try
clause would avoid doing the setup, and jumping out of it would
avoid doing the cleanup.


### Why isn't a Finally clause supported?

There's no way to implement it.  In Java, the main difference
between putting code in a finally clause versus putting the code
after the try/catch statement is that the finally clause will get
executed even if a return or throw occurs within the try clause or
catch clause.  But in C, a return statement returns immediately.

In Java one could make do without a finally clause by deferring
any return from the try clause (by storing the return value in a
variable), and moving some or all of the catch code outside the
try/catch statement, like so:

    Exception e = null;

    try { ... }
    catch (Exception tmp) { e = tmp; }

    // Do cleanup here.

    if (e instanceof Foo) { ... }
    else if (e instanceof Bar) { ... }
    ...
    else if (e != null) throw e;

A similar technique can be used with cexcept:

    my_exception_type e = some null value that will not be thrown;

    Try { ... }
    Catch (e) { }

    /* Do cleanup here. */

    if (e is not that null value) { ... }


### Why can't I use Throw without an expression to re-throw an exception?

The use of throw without an expression is needed in C++ because it's
the only way to re-throw an exception of unknown type:

    catch (...) {
        // do some stuff
        throw;
    }

This syntax is not provided in Java because the fact that all
exceptions are instances of Exception eliminates the problem:

    catch (Exception e) {
        // do some stuff
        throw e;
    }

Similarly, with cexcept, the type of exceptions is always known, so
an expressionless Throw is not needed.  Besides, the syntax would be
very difficult, if not impossible, to implement.


### Why can't "real" exceptions be caught?

"Real" exceptions (like floating-point exceptions and
addressing exceptions) are platform-dependent, but cexcept is a
platform-independent abstraction of setjmp/longjmp.  The only way
to jump to a `setjmp()` is from a `longjmp()`, so a Catch requires
an explicit Throw.

On POSIX systems, it is possible to call `setjmp()` from within a
signal handler, and hence possible to use Throw within a signal
handler, but this is very likely to cause subtle bugs and is not
recommended.  If a program uses both signals and cexcept, it may
wish to modify cexcept.h to use the POSIX functions `sigsetjmp()`
and `siglongjmp()` instead of the standard C library functions
`setjmp()` and `longjmp()`.


### Why must the Catch expression have the same type that was passed to `define_exception_type()`?

Although a double value can be assigned to an int, applications should
not throw a double exception expecting to catch it in an int variable.

The intuitive expectation is that `Throw v` and `Catch(e)` should
have an effect equivalent to `e = v`.  The latter converts `v` to the
type of `e`, but the Throw statement has no way of knowing the type of
`e`, so it converts `v` to the type passed to `define_exception_type()`.
Depending on how cexcept is implemented, unintuitive or even undefined
behavior could result if `e` has a different type.


### Why does the implementation copy the exception twice?

The Throw statement must store the exception value somewhere.  If it
stores directly into the object named by the Catch expression, then
that object must not be a non-volatile automatic object.  It was
deemed preferable to copy the exception twice rather than prohibit
application programmers from catching exceptions in non-volatile
automatic objects (which is what they would most often be inclined
to use).


### Why are the macros spelled Try, Catch, Throw rather than...?

We considered many other possibilities:

 * `try catch throw`
 * `TRY CATCH THROW`
 * `ctry ccatch cthrow`
 * `c_try c_catch c_throw`
 * `cex_try cex_catch cex_throw`
 * `CEX_TRY CEX_CATCH CEX_THROW`
 * `cexcept_try cexcept_catch cexcept_throw`
 * `try_until_exception catch_exception throw_exception`

We wanted names that would be easy to read, easy to type, intuitive,
and unlikely to conflict with existing names.  Ultimately we settled
on Try, Catch, and Throw as the best compromise among all these
criteria.
