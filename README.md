# The FMI++ Library

**A High-level Utility Package for FMI-based Software Development**

Documentation is available [online](https://fmipp.readthedocs.io/en/latest).

## About

The [Functional Mock-up Interface][1] (FMI) specification intentionally provides only the most essential and fundamental functionalities in the form of a C interface.
On the one hand, this increases flexibility in use and portability to virtually any platform.
On the other hand, such a low-level approach implies several prerequisites a simulation tool has to fulfill in order to be able to utilize such an FMI component.

The FMI++ library addresses this problem for models and tools with interfaces according to the FMI specification by providing high-level functionalities, which ease the handling and manipulation of such models and tools.
Furthermore, the FMI++ library provides utilities to implement FMI-compliant interfaces for a large class of simulation tools.

The FMI++ library intends to bridge the gap between the basic FMI specifications and the typical requirements of simulation tools.
It is distributed using a [BSD-like license][2].

## Features

- provides functionalities to import FMUs (model description parser, numerical integrators, etc.)
- provides utilities to implement FMI-compliant interfaces for a large class of simulation tools
- interfaces for Python, Java, and other languages
- uses [Boost][3] and [SUNDIALS][4] (optional)
- compiles on many plattforms

[1]: https://fmi-standard.org/
[2]: ./FMIPP_LICENSE
[3]: https://www.boost.org/
[4]: https://computing.llnl.gov/projects/sundials