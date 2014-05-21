import os
import numpy
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

# change the following variables according to your setup
fmipp_source_dir = os.path.join( "..", ".." ) # relative path to FMI++ root source directory
fmippim_lib_dir = os.path.join( "..", "..", "build", "import" ) # relative path to FMI++ import library (libfmippim)


#if ( os.uname()[0] == "Linux" ) : # <-- Python2.7
if ( os.uname().sysname == "Linux" ) : # <-- Python3.3
    fmippim_module = [ Extension( "fmippim", [ "fmippim.pyx" ],
                                  include_dirs=[ fmipp_source_dir,
                                                 numpy.get_include() ],
                                  library_dirs = [ fmippim_lib_dir ],
                                  libraries = [ "fmippim" ],
                                  language="c++" ) ]

    setup(
        cmdclass = {'build_ext': build_ext},
        ext_modules = fmippim_module	
        )
else:
    # The next line does some magic trick that allows to build properly under MINGW
    #from numpy.distutils.misc_util import Configuration

    fmippim_module = [ Extension( "fmippim", [ "fmippim.pyx" ],
                                  include_dirs=[ fmipp_source_dir,
                                                 numpy.get_include() ],
                                  library_dirs = [ fmippim_lib_dir ],
                                  libraries = [ "libfmippim" ],
                                  extra_compile_args = [ "-v -DMINGW" ],
                                  language="c++" ) ]

    setup(
        cmdclass = {'build_ext': build_ext},
        ext_modules = fmippim_module	
        )
