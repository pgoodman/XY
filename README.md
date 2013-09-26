**Note: XY is incomplete, bug-ridden, and unmaintained!!**

XY Programming Language
=======================

XY is an experimental programming language. This repository is a C++11 implementation of an XY compiler. The code is not particularly pretty, but it gets the job done.

The syntax of XY is a mix between JavaScript, C, C++, Haskell, and Python.

Types in XY are structurally defined. I intend to have both inductive and co-inductive data types. The XY compiler will check the well-typedness of programs statically (i.e. at compile time).

Requirements
------------
A relatively recent version of the GCC, Clang, or the Intel&reg; C++ XE Studio. The compiler is implemented using some features of C99 and C++11, e.g. variadic templates, variadic macros, r-value references, etc.

Downloading the Coding
----------------------

    git clone git@github.com:pgoodman/XY.git
    cd XY


Compiling with GNU Toolchain
----------------------------
    
    make install
    make CXX="g++ -Dconstexpr=const -Dnullptr=0" CC="gcc"


Compiling with LLVM Toolchain
-----------------------------

    make install
    make CXX="clang++ -Dconstexpr=const -Dnullptr=0" CC="clang"

Compiling with the Intel&reg; C++ Studio
--------------------------------------

    make install
    make CXX="icpc -Dconstexpr=const -Dnullptr=0" CC="icc"

Reporting Bugs / Compiler Errors
--------------------------------

I usually compile the XY compiler on my computer and on a Linux box. This leaves much to be desired in terms of portability. If you run into a compiler error a runtime error, then please post them here or email them to i@petergoodman.me. Thank you!
