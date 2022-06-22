*****************
The FMI++ Library
*****************

.. meta::
   :description lang=en: The FMI++ Library

Introduction
============

The `Functional Mock-up Interface (FMI) <https://fmi-standard.org/>`_ specification intentionally provides only the most essential and fundamental functionalities in the form of a C interface.
On the one hand, this increases flexibility in use and portability to virtually any platform.
On the other hand, such a low-level approach implies several prerequisites a simulation tool has to fulfil in order to be able to utilize such an FMI component.

The `FMI++ Library <https://github.com/fmipp/fmipp>`_ library addresses this problem for models and tools with interfaces according to the FMI specification by providing high-level functionalities, which ease the handling and manipulation of such models and tools (see chapter on :doc:`import functionalities <import>`).
Furthermore, the FMI++ library provides utilities to implement FMI-compliant interfaces for a large class of simulation tools (see chapter on :doc:`export functionalities <export>`).

The FMI++ library intends to bridge the gap between the basic FMI specifications and the typical requirements of simulation tools.
It is distributed using a :github_blob:`BSD-like license <FMIPP_LICENSE>` and suggestions or contributions are heartly welcome.

Implementation
==============

In order to provide a reliable, stable and portable solution, the FMI++ library relies on other state-of-the-art tools where necessary.
Especially, it depends upon many functionalities provided by the `Boost library <https://www.boost.org/>`_ – from numerical integration to XML parsing, shared memory access and testing.
Cross-language wrappers are realized with the versatile `SWIG <https://swig.org/>`_ tool, an open-source software tool for connecting computer programs or libraries written in C/C++ with other languages.
The building process is managed via `CMake <https://cmake.org/>`_ , a cross-platform free and open-source software for managing the build process of software using a compiler-independent method.
Numerical integrators are implemented using `Boost odeint <https://github.com/boostorg/odeint>`_ and `SUNDIALS <https://computing.llnl.gov/projects/sundials>`_.
These dependencies allow to deploy the FMI++ library on a combination of many platforms and compilers, making it a flexible multi-platform solution in the spirit of the FMI specification.

Data types and namespaces
=========================

The FMI++ Library aims at using common data types and interfaces for the different versions of the FMI specification (i.e., FMI V1.0, FMI V2.0, and FMI V3.0).
Find information on the common data types and namespaces :doc:`here <common>`.

Import utilities
================

The FMI++ library provides functionalities that ease the handling of FMUs.
The goal is to facilitate the **integration of FMU models into existing simulation environments**.
Find information on these import utilties :doc:`here <import>`.

Export utilities
================

The FMI++ library provides functionalities that ease the creation of FMUs.
The goal is to facilitate the **integration of existing software tools via FMI compliant interfaces and wrappers**.
Find information on these export utilties :doc:`here <export>`.

.. toctree::
   :maxdepth: 2
   :hidden:
   :caption: FMI++ Library Reference

   /common
   /import
   /export

Building and Installing
=======================

Find information on **building and installing** the FMI++ Library :doc:`here <installation>`.

Using the FMI++ Library in your own project
===========================================

Find information on using the FMI++ Library in your own C++, Java and Python :doc:`here <usage>`.

.. toctree::
   :maxdepth: 2
   :hidden:
   :caption: Usage

   /installation
   /usage

