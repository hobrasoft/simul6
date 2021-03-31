See [simul6.app](https://www.simul6.app) for an overview.

About Simul
======================

Simul is a series of simulation software for electrophoresis which has been
continuously developing.

It simulates the movement of ions in liquid solutions in an electric field. It
solves numerically a set of nonlinear partial differential equations and
nonlinear algebraic equations describing the continuity of ionic movement and
acid-base equilibria.

Simul 6.1 is a continuation of the previous version 5. It has been designed
using the latest programming tools. Its code was rewritten using Qt platform
with fully standard-conformant C++ and utilizes the MSVC compiler. The
computation engine was completely redesigned in order to take full advantage of
parallelization and multithreading computation. It is 5 – 15 times faster than
Simul 5.

The typical use of Simul 6.1
============================

-    to simulate experimental runs and optimize separation conditions (buffers, injection),
-    to inspect or explain unusual phenomena observed in experiments: system peaks (zones), overshoots, peak distortion,
-    to inspect stacking and preconcentrating analytes, electromigration dispersion,
-    to show beginners how electrophoresis works.

Features of Simul 6.1
=====================

-    A mathematical model of electromigration which accounts for any number of multivalent electrolytes or ampholytes, even in the hundreds.
-    Simul 6.1 includes a database of electrophoretic mobilities and pKa constants based on Takeshi Hirokawa's tables with the data of many ions.


Installation
============

Linux:

```
qmake
make -j
```




