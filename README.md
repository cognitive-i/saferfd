SaferFD
==========================
SaferFD is a thin C++ 11 type for making working with POSIX file descriptors
safer.

It attempts to make it hard to make the following mistakes with file
descriptors:

* forgoting to close (handled when FD goes out of scope)
* performing operations on a dead file descriptor
* race conditions from language copying of file descriptor int rather than OS dup call 

**GitHub Repository**
https://github.com/cognitive-i/saferfd

Installation
============
The project uses CMake.

Usage
=====
See example folder.

Author
======
Atish Nazir, Cognitive-i Ltd saferfd@cognitive-i.com
