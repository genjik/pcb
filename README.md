Synchronization: Semaphores and Producer-Consumer Queues
===

You should have received a detailed handout for this project.  It
contains the project requirements and guidelines for implementing the
project.

Repository Layout
---

Your implementation should go in `src/csesem.c` and `src/pcq.c`.  The
header files `src/csesem.h` and `src/pcq.h` contain detailed information
about the functions that you will implement, but should not be changed.
The given code in this directory _does not work_, but will compile and
might pass some tests under the right circumstances.

There are five given tests in the `tests/` directory.  Each of them has
extensive comments.  You should read these tests as you are implementing
your code, to understand what they do and how you might write additional
tests to check your code.  They may also help you to understand how the
functions that you are implementing are used.

Building the Project
---

The default make target does nothing but compile your code.  You may use
it as a convenient way to syntax check.

Testing
---

As mentioned above, there are five tests provided.  You can run them
with `make test`.  You should plan to write additional tests for your
code.  The given tests will exercise many aspects of your code, but may
fail even if some parts of your code are already working; in particular,
the call the destroy functions, which may prove problematic.

Submission
---

Use `make submission` to build the file `fifo.tar`, which you will
upload to Autograder.
