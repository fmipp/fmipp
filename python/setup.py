import os
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

# The next line does some magic trick that allows to build properly under MINGW
from numpy.distutils.misc_util import Configuration


fmipp_module = [ Extension( "fmipp", ["fmipp.pyx"],
                 include_dirs=[ os.path.join( "..", "include" ), os.path.join( "c:\\", "Tools", "expat-2.1.0", "include" ) ],
                 library_dirs = [ os.path.join( "..", "lib" ) ],
                 libraries = [ "libfmipp" ],
                 extra_compile_args = [ "-v -DMINGW" ],
                 language="c++" ) ]


setup(
    cmdclass = {'build_ext': build_ext},
    ext_modules = fmipp_module
)
