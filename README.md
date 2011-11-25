XY Programming Language
=======================

XY is an experimental programming language. This repository is a C++11 implementation of an XY compiler. The code is not particularly pretty, but it gets the job done.

The syntax of XY is a mix between JavaScript, C, C++, Haskell, and Python.

Types in XY are structurally defined. I intend to have both inductive and co-inductive data types. The XY compiler will check the well-typedness of programs statically (i.e. at compile time).

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

Reporting Bugs / Compiler Errors
--------------------------------

Either post them here or email them to i@petergoodman.me. Thank you!
