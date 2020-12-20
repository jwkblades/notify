# Contributing Guide

The following is a set of guidelines to use when contributing to notify. These
guidelines are subject to change over time, though may go through a discussion
process before being finalized.

## Style Guidelines

Coding standards should follow the `.clang-format` file, located in the base
project directory. In short, what it says is:
* Spaces, always
  * 4 spaces for indentation
* lowerCamel case for function names
* UpperCamel case for class/struct names
* SCREAMING\_SNAKE case for consts
* Always use curlies
  * Curlies always go on a new line
* Try to limit code length to <120 characters per line.
* Do limit comments to <=80 characters per line
* Indentation is important

## Coding Standards

* We should always have unit tests for code that is checked in.
  * **NOTE**: Yes, I know that this isn't always possible, especially when
    interacting with a GUI environment, but you should do your best to unit test
    anything you add.
  * If you are fixing a bug, it likely means that there was a unit test failure,
    and you should add a new unit test that appropriately fails _prior_ to the
    fix, and successfully passes _after_ the fix.
* Classes should be named the same thing as the file they are contained in.
* Logically group classes into component directories
* Directories `i`, `s`, and `t` have special meanings.
  * `i` is an include directory, meant to be part of a public API.
  * `s` is a source directory, meant to house C++ files and implementation
    logic.
  * `t` is a test directory, where all of your unit tests go.

## Commits and Commit Messages

Try to make each commit a logically separate changeset which is easily
digestible. Large commits that change many different aspects of the application
at the same time are harder to understand and will increase review turnaround
time.

Commit titles should be concise and to the point. However, it can also be
extremely useful to have an involved description that explains the _why_ behind
the changes, not just the what.

When working on an issue, make sure to have the issue as the first thing in the
commit title so it is easy to determine _what_ you were working on. For
instance:  
* `[#9999999999] Fixed divide by zero errors`
* `#9999999999: Fixed divide by zero errors`
