cmake_minimum_required( VERSION 2.8.12 )

project( fmipp_import_cython )

add_custom_target( fmippim.cpp ALL
                   COMMAND python cythonize.py build_ext -i
                   DEPENDS fmippim.pyx cythonize.py
                   SOURCES fmippim.pyx cythonize.py )
