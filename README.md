Introduction
============

An assortment of C utilities and hardware abstractions that work on
multiple platforms including x86 PC\'s and microcontrollers. One day
this library could be more generically useful, but currently it is
developed according to the needs of the `omnid` robot project.

Main Features
=============

1.  Works on x86 Linux and tiva microcontrollers
2.  UART serial library with code to lower the default latency of FTDI
    USB to serial converters and to use RS-485 serial devices on linux.
    -   Only some fixed baud rates are supported, but they are added on
        an as-needed basis
    -   It is also possible to set the default FTDI latency with udev
        rules
3.  Protocol and serialization/de-serialization code for use over the
    uart
4.  Lock-free single-producer single-consumer queue
5.  CMake utilities
    -   Exposing git information at compile time via generated header
        files
    -   Compile flags that I like can (optionally) be carried over by
        linking with the library
        -   Extra warnings, strict standard compliance, and modern C and
            C++ standards
    -   Basic CMake options that I find useful
        -   Prevent in-source builds
        -   Set a default build type (for single-configuration
            generators)
6.  Flexible error handling code. Designed to immediately terminate
    program while preserving debugging information.
7.  A PID controller implementation
8.  Generic interface for doing math with multi-turn encoders
9.  Static assertions for C code
10. Serialization library
11. Functions for handling time.
12. Red/Green/Blue LED control that convert to print statements on a
    host pc
13. Redistribute catch.hpp unit testing framework for use in other
    projects

Installation
============
See [Installation](https://github.com/omnid/omnid_docs/blob/master/Installation.md)
for options and details, including how to install this project without ROS.

Usage
=====

-   Use `find_package(nuhal)` then add `nuhal::nuhal` to the
    `target_link_libraries`
-   The platform is auto-detected based on the toolchain and the appropriate libraries are used.
-   `tiva_cmake` is a dependency.

Structure
=========
- `src/common` contains the code common to all platforms. This does not make a library
- `src/linux` contains code that works on Linux only.
- `src/tiva` contains code that works on the Tiva microcontroller.
- `include/common` headers common to all platforms
- `include/linux` linux-specific headers
- `include/tiva` tiva-specific headers
- `CMakeLists.txt` orchestrates the overall build, ensures that hte library is built for all platforms with
   the appropriate toolchain

Documentation
=============
- Doxygen documentation is generated with each build and placed in `&lt build &gt/doc/index.html`.

Adding a Platform
-----------------

1.  Create a new subdirectory called `nuhal_platform`
2.  Use `nuhal/nuhal_linux/CMakeLists.txt` as a guide for the
    CMakeLists.txt file
3.  If you need to support cross-compiling, use
    `nuhal/nuhal_tiva/PreLoad.txt` as a guide for setting a default
    cross compiler
4.  Update `/nuhal/nuhal_all/nuhal-config.cmake.in` to make it aware of
    your new library. When a user does a `find_package(nuhal)` it is
    this file that is called and it is its responsibility to select the
    appropriate default `nuhal` library based on the platform and
    toolchain

Testing
=======
1. There are automated unit tests that are platform-independent and can be run with `make test`
2. There are integration tests that can be loaded on the microcontroller in `nuhal_tiva/test`. Comments
   at the start of each file describe the expected test behavior
