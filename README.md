# misclib

A library of various useful C routines, mostly for embedded use.


# Requirements
The following are required to build:

- (GNU) make
- gcc or a C90 compiler
- doxygen for documentation

It is possible to replace *make* with something else such as Eclipse, Microsoft VisualStudio, or the Keil µVision IDE. You can be creative here.

It is possible to build individual files, the dependencies are clear from the #include statements. If it does not #include another module, there should not be a dependency.


## Note
make misclib will build to a point, but fail to link. This will be fixed later.

make doc generates full documentation. Should be O.K. but still work in progress.
