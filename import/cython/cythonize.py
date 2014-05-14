import os
import numpy
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext


#if ( os.uname()[0] == "Linux" ) : # <-- Python2.7
if ( os.uname().sysname == "Linux" ) : # <-- Python3.3
    fmipp_module = [ Extension( "fmipp", [ "fmipp.pyx" ],
                                include_dirs=[ os.path.join( "..", "include" ),
                                               numpy.get_include() ],
                                library_dirs = [ os.path.join( "..", "lib" ) ],
                                libraries = [ "fmipp" ],
                                language="c++" ) ]

    setup(
        cmdclass = {'build_ext': build_ext},
        ext_modules = fmipp_module	
        )
else:
    # The next line does some magic trick that allows to build properly under MINGW
    #from numpy.distutils.misc_util import Configuration

    fmipp_module = [ Extension( "fmipp", [ "fmipp.pyx" ],
                                include_dirs=[ os.path.join( "..", "include" ),
                                               os.path.join( "c:\\", "Tools", "expat-2.1.0", "include" ),
                                               numpy.get_include() ],
                                library_dirs = [ os.path.join( "..", "lib" ) ],
                                libraries = [ "libfmipp" ],
                                extra_compile_args = [ "-v -DMINGW" ],
                                language="c++" ) ]

    setup(
        cmdclass = {'build_ext': build_ext},
        ext_modules = fmipp_module	
        )
