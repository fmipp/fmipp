*************************
Data types and namespaces
*************************

The continuous development and improvement of the FMI specfication leads to conflicting defintions between different versions.
For instance, in FMI V1.0 the data type ``fmiBoolean`` was defined as ``char``, whereas FMI V2.0 defined ``fmi2Boolean`` as ``int``.

The FMI++ Library aims to use and provide common C++ interfaces and data types on top of these conflicting definitions.
To this end, just one set of data types is used consistently for handling FMUs of different FMI versions.
Wherever needed, the FMI types are cast to/from these FMI++ types.
Namespaces are used to separate and distinguish interfaces for different FMI versions.


.. topic:: FMI++ basic data types:

  :``fmippTime``: typedef of ``double``
  :``fmippReal``: typedef of ``double``
  :``fmippInteger``: typedef of ``int32_t``
  :``fmippBoolean``: typedef of ``bool``
  :``fmippString``: typedef of ``std::string``
  :``fmippTrue``: defined as ``true``
  :``fmippFalse``: defined as ``false``


.. seealso:: Refer to file :github_blob:`FMIPPTypes.h <common/FMIPPTypes.h>` for the full list of FMI++ data types.

.. topic:: FMI++ status flags (enum):

  :``fmippOK``: equivalent to ``fmiOK``, ``fmi2OK``, etc.
  :``fmippWarning``: equivalent to ``fmiWarning``, ``fmi2Warning``, etc.
  :``fmippDiscard``: equivalent to ``fmiDiscard``, ``fmi2Discard``, etc.
  :``fmippError``: equivalent to ``fmiError``, ``fmi2Error``, etc.
  :``fmippFatal``: equivalent to ``fmiFatal``, ``fmi2Fatal``, etc.


.. topic:: FMI++ Namespaces:

  :*fmi_1_0*: namespace for FMI V1.0
  :*fmi_2_0*: namespace for FMI V2.0
