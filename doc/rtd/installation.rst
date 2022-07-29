**************************
Building the FMI++ Library
**************************

General prerequisites
=====================

The following tools/libraries need to be installed properly:

* `CMake <https://cmake.org/>`_
* `Boost <https://www.boost.org/>`_ (version 1.72.0 or higher is recommended)

For installing the Java and Python bindings you also need the following:

* Java Software Development Kit (e.g. `Oracle’s JDK <https://www.oracle.com/java/technologies/downloads/>`_)
* `Python <https://www.python.org>`_ (make sure you install a Python version including shared libraries, e.g., *libpython3.8.so* on Linux)
* `SWIG <https://swig.org/>`_ (tested with version 4.0.2)

In case the `SUNDIALS <https://computing.llnl.gov/projects/sundials>`_ equation solver suite should be used for numerical integration, the according include files and libraries need to be installed.

To build the code reference, `Doxygen <https://www.doxygen.nl/index.html>`_ has to be installed.

Linux
=====

CMake, Boost (incl. odeint), JDK, Python and SWIG are usually available via the native package management (e.g., ``apt``).
To build FMI++, use CMake as command line tool.
In the terminal, go to the FMI++ source directory and type:

.. code-block:: bash

  mkdir build
  cd build
  cmake ..
  make

To run the tests, type:

.. code-block:: bash

  make test

To build the Doxygen documentation, type:

.. code-block:: bash

  make doc

Windows
=======

.. seealso:: **Windows only**: Be sure to add the Boost library directories (containing the DLLs) to the global ``PATH`` variable **BEFORE** you run CMake!!!

  #. open the system properties (``WinKey`` + ``Pause`` or go to *Settings* |arrow| *System* |arrow| *About* |arrow| *System Info* |arrow| *Advanced System Settings*)
  #. select the *Advanced* tab, then the *Environment Variables* button
  #. select and edit the ``PATH`` variable in the user variables, e.g., adding *C:\\Tools\\boost_1_72_0\\lib64-msvc-14.2*.

.. |arrow| unicode:: U+2192 .. rightwards arrow

Visual Studio 2019
------------------

Prerequisites:

* Install `Microsoft Visual Studio 2019 <https://visualstudio.microsoft.com/downloads/>`_
* Install Boost libraries:
  * download the Boost library compiled with Visual Studio 2019 from `here <https://sourceforge.net/projects/boost/files/boost-binaries/>`_
  * include the library directory in the ``PATH`` variable (see above)

Use the CMake GUI:

* Configure where to find the source code and where to build the binaries.
* Click *Configure*.

  * Select *Visual Studio 16 2019*.
  * In case of errors, set variables according to your system setup (``BOOST_INCLUDEDIR``, ``BOOST_LIBRARYDIR``, ``SWIG_EXECUTABLE``, ``PYTHON_EXECUTABLE``, etc.)
  * Set variables and click Configure until configuration is successful.

* Click Generate.
* Then go to the FMI++ build directory:

  * Open file *fmipp.sln* (double-click) to run Visual Studio 2019.
  * Press F7 to build the full solution.

Alternatively, you can run from the Windows Command Prompt.
Starting from the FMI++ source directory, type something like:

.. code-block:: doscon

  rmdir build /S /Q
  mkdir build
  cd build
  cmake .. -G "Visual Studio 16 2019" -DBOOST_INCLUDEDIR=C:/Tools/boost_1_72_0 \
    -DSWIG_EXECUTABLE=C:/Tools/swig-4.0.2/bin/swig.exe \
    -DPYTHON_EXECUTABLE=C:/Tools/Python38-x64/python.exe
  cmake --build . --target install --config Release

To build the Doxygen documentation either select and build target doc in Visual Studio, or enter at the command prompt (in the build directory):

.. code-block:: doscon

  cmake --build . --target doc

Build options
=============

Building language bindings and unit tests
-----------------------------------------

By default, the Java & Python bindings and unit tests are built.
However, there are special CMake flags available that control whether the Java & Python bindings (``BUILD_SWIG``, ``BUILD_SWIG_JAVA``, ``BUILD_SWIG_PYTHON``) and the unit tests (``BUILD_TESTS``) are build or not.
For instance, if you do not want to build the Java & Python bindings, you can

* either call CMake from the command line with an additional input option
  ::

    -DBUILD_SWIG=OFF

* or turn off the option by editing file *CMakeLists.txt* in the FMI++ source directory
  ::

    option( BUILD_SWIG "SWIG - Simplified Wrapper ..." OFF )

* or use the CMake GUI (if available) to uncheck the according tick box.

If you do not want to build the unit tests (not recommended) proceed accordingly.

Using the SUNDIALS numerical solver suite
-----------------------------------------

By default, the FMI++ library uses `Boost odeint <https://github.com/boostorg/odeint>`_ for numerical integration.
However, FMI++ also offers the possibility to use the CVODE solver provided by `SUNDIALS <https://computing.llnl.gov/projects/sundials>`_.

In order to do so, the following special flags for CMake have to be set:

* ``INCLUDE_SUNDIALS`` needs to be set to ``ON``
* ``SUNDIALS_INCLUDEDIR`` needs to point the directory containing the header files
* ``SUNDIALS_LIBRARYDIR`` needs to point to the directory containing the (dynamic or static) libraries

These values can be set in various ways (compare the instructions above for building language bindings and unit tests).
For example, on the Linux command line CMake should be called similar to the following:

.. code-block:: bash

  cmake <path/to/fmipp> -DINCLUDE_SUNDIALS=ON -DSUNDIALS_INCLUDEDIR=/usr/include/sundials -DSUNDIALS_LIBRARYDIR=/usr/lib/x86_64-linux-gnu/
