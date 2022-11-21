********************************************
Using the FMI++ Library in your own projects
********************************************

C++ projects
============

After successfully building FMI++, two shared libraries are available in the build directory (or a subdirectory called *Release*, *Debug*, etc.).
They provide the full functionality of package ``import`` and ``export``, respectively.
When including FMI++ library header files, it is necessary to define a few preprocessor macros and compiler flags.
Which macros and flags to use depends on the plattform and the compiler, please refer to the list below for further details (compare also with :github_blob:`CMakeLists.txt <CMakeLists.txt>` in the root source directory).
Please refer to the documentation of the respective compiler to learn how to set macros and flags.

* ``WIN32``:
  This preprocessor macro has to be set whenever working on Windows.
* ``BOOST ALL NO LIB``:
  This preprocessor macro is recommended when working with Visual Studio (disables Boost auto-linking).
* ``-std=c++11``:
  This compler flag has to be set when using GCC version 4.7.0 or later.
* ``-std=c++0x``:
  This compler flag has to be set when using GCC version older than 4.7.0.

.. seealso:: How to define macros and flags depends on the compiler.
  Using CMake helps a lot in this regard, as it provides compiler- and plattform-independent functions for this purpose.
  For instance, for GCC a preprocessor macro can be defined with the ``-D`` option, for Visual C++ this can be done using the ``/D`` option, while with CMake for both cases the ``add_definitions`` function can be used.

Java projects
=============

After successfully building the FMI++ Java language bindings, all necessary java, class and jar files and shared libraries are available in directory *import/swig* (or a sub-directory called *Release*, *Debug*, etc.).
Make sure that ``java.library.path`` points to this directory and that the directory containing the FMI++ libraries is found in the system path!

.. seealso:: Some FMI++ artifacts for Java are also available in the `Maven Central repository <https://mvnrepository.com/repos/central>`_ (see `here <https://search.maven.org/search?q=fmipp>`__).

Python projects
===============

After successfully building the FMI++ Python language bindings, the files *fmippim.py* and *fmippim.pyd* are available in directory *import/swig* (or a sub-directory called *Release*, *Debug*, etc.).
Together they comprise the Python module for wrapping the FMI++ library.
Make sure that environment variable ``PYTHONPATH`` points to this directory and that the directory containing the FMI++ libraries is found in the system path!

.. seealso:: The `FMI++ Python Interface <https://github.com/fmipp/py-fmipp>`_ is also available via the `Python Package Index <https://pypi.org/>`_ (see `here <https://pypi.org/project/fmipp/>`__)!
