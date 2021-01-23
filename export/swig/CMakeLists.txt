# -------------------------------------------------------------------
# Copyright (c) 2013-2020, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# -------------------------------------------------------------------

cmake_minimum_required( VERSION 3.8 )

project( fmippex_wrap )

if ( SWIG_FOUND )
   if (${CMAKE_VERSION} VERSION_GREATER "3.13")
      cmake_policy(SET CMP0078 OLD) # https://cmake.org/cmake/help/git-stage/policy/CMP0078.html
      cmake_policy(SET CMP0086 OLD) # https://cmake.org/cmake/help/git-stage/policy/CMP0086.html
   endif()

   include( UseSWIG )

   set_source_files_properties( libfmippex.i PROPERTIES CPLUSPLUS ON )

   # swig does not yet support matlab, so the following was tested with swig based on https://github.com/jaeandersson/swig
   if ( BUILD_SWIG_MATLAB )
      include_directories( ${Matlab_INCLUDE_DIRS} )

      set( FMIPPEX_WRAP_MATLAB_MODULE "fmippex_wrap_matlab" )
      # the output name of the matlab mex-file has to be the same as the module with MEX added, since the swig implementation i'm using wants it that way...

      swig_add_library( ${FMIPPEX_WRAP_MATLAB_MODULE} LANGUAGE matlab SOURCES libfmippex.i )

      if ( WIN32 )
        if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	      set_target_properties( ${SWIG_MODULE_${FMIPPEX_WRAP_MATLAB_MODULE}_REAL_NAME} PROPERTIES OUTPUT_NAME "fmippexMEX" PREFIX "" SUFFIX ".mexw64" COMPILE_FLAGS "-DMATLAB_MEX_FILE" )
	    else ()
	      set_target_properties( ${SWIG_MODULE_${FMIPPEX_WRAP_MATLAB_MODULE}_REAL_NAME} PROPERTIES OUTPUT_NAME "fmippexMEX" PREFIX "" SUFFIX ".mexw32" COMPILE_FLAGS "-DMATLAB_MEX_FILE" )
	    endif ()
		if ( MSVC ) # Add additional linker option, in order to make MSVC export the "mexFunction(...)" function.
		  # The following adds "-export:mexFunction" to the linker options. NOTE: Actually it
		  # should add "/export:mexFunction", but this work as well (at least with VS 2010).
		  target_link_libraries( ${SWIG_MODULE_${FMIPPEX_WRAP_MATLAB_MODULE}_REAL_NAME} "-export:mexFunction" )
        endif ()
      elseif( APPLE )
      	 set_target_properties( ${SWIG_MODULE_${FMIPPEX_WRAP_MATLAB_MODULE}_REAL_NAME} PROPERTIES OUTPUT_NAME "fmippexMEX" PREFIX "" SUFFIX ".mexmaci64" )
      elseif( UNIX )
         set_target_properties( ${SWIG_MODULE_${FMIPPEX_WRAP_MATLAB_MODULE}_REAL_NAME} PROPERTIES OUTPUT_NAME "fmippexMEX" PREFIX "" SUFFIX ".mexa64" )
      endif ()

      swig_link_libraries( ${FMIPPEX_WRAP_MATLAB_MODULE} ${Matlab_LIBRARIES} )
      swig_link_libraries( ${FMIPPEX_WRAP_MATLAB_MODULE} fmippex )
   endif ()

   if ( BUILD_SWIG_OCTAVE )
      if ( NOT DEFINED OCTAVE_INCLUDE_DIRS )
         message( FATAL_ERROR "Variable OCTAVE_INCLUDE_DIRS is not defined!" )
      endif()

      include_directories( ${OCTAVE_INCLUDE_DIRS} )
      include_directories( ${OCTAVE_INCLUDE_DIRS}/octave )
      include_directories( ${HDF5_INCLUDE_DIRS} )

      set( FMIPPEX_WRAP_OCTAVE_MODULE "fmippex_wrap_octave" )
      # the output name of the octave .oct-file has to be the same as the module name, otherwise octave does not find it when importing...

      swig_add_library( ${FMIPPEX_WRAP_OCTAVE_MODULE} LANGUAGE octave SOURCES libfmippex.i )
      set_target_properties( ${SWIG_MODULE_${FMIPPEX_WRAP_OCTAVE_MODULE}_REAL_NAME} PROPERTIES OUTPUT_NAME "fmippex" )
      swig_link_libraries( fmippex_wrap_octave fmippex )
      if( APPLE )
            swig_link_libraries( fmippex_wrap_octave ${OCTAVE_LIBRARIES}/liboctave.dylib )
      	    swig_link_libraries( fmippex_wrap_octave ${OCTAVE_LIBRARIES}/liboctinterp.dylib )
      endif ()
   endif()

   if ( BUILD_SWIG_PYTHON )
      include_directories( ${PYTHON_INCLUDE_DIRS} )

      set( FMIPPEX_WRAP_PYTHON_MODULE "fmippex_wrap_python" )
      # change the output name of the python dynamic library to _fmippex, otherwise pyhton does not find it when importing...

      swig_add_library( ${FMIPPEX_WRAP_PYTHON_MODULE} LANGUAGE python SOURCES libfmippex.i )
      set_target_properties( ${SWIG_MODULE_${FMIPPEX_WRAP_PYTHON_MODULE}_REAL_NAME} PROPERTIES OUTPUT_NAME "_fmippex" )

      set_target_properties( ${SWIG_MODULE_${FMIPPEX_WRAP_PYTHON_MODULE}_REAL_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        )

      swig_link_libraries( ${FMIPPEX_WRAP_PYTHON_MODULE} fmippex ${PYTHON_LIBRARIES} )

   endif()

   if ( BUILD_SWIG_JAVA )
      include( UseJava )
      include_directories( ${JAVA_INCLUDE_PATH} )
      include_directories( ${JAVA_INCLUDE_PATH2} )

      # ATTENTION: The following lines are needed to place a generated java wrapper into an appropriate package.

      # However, the according flag causes a swig error when generating a Python wrapper (unrecognized option).
      # Therefore, the following command has to issued only AFTER the python wrapper module has been declared!
      set_property( SOURCE libfmippex.i PROPERTY SWIG_FLAGS "-package" "at.ac.ait.fmipp.exp" )

      swig_add_library( fmippex_wrap_java LANGUAGE java SOURCES libfmippex.i )
      swig_link_libraries( fmippex_wrap_java fmippex )

      if ( WIN32 )
         if ( MINGW )
            # add aliases because otherwise JNI doesn't find the functions in the dll
            set_target_properties( fmippex_wrap_java PROPERTIES LINK_FLAGS "-Wl,--add-stdcall-alias" )
         endif ()
      endif ()

      set( FMIPPEX_JAR_FILENAME "fmippex.jar" )

      add_custom_command( OUTPUT ${FMIPPEX_JAR_FILENAME} DEPENDS fmippex_wrap_java
         COMMAND ${CMAKE_COMMAND} -E echo "Compiling Java files..."
         COMMAND ${Java_JAVAC_EXECUTABLE} at/ac/ait/fmipp/exp/*.java
         COMMAND ${CMAKE_COMMAND} -E echo "Creating jar file..."
         COMMAND ${Java_JAR_EXECUTABLE} -cf ${FMIPPEX_JAR_FILENAME} at/ac/ait/fmipp/exp/*.class )

      add_custom_target( fmippex_jar ALL DEPENDS ${FMIPPEX_JAR_FILENAME} )

      add_custom_command( TARGET fmippex_wrap_java POST_BUILD
         COMMAND ${Java_JAR_EXECUTABLE} -cf temp.zip *.java
         COMMAND ${CMAKE_COMMAND} -E make_directory at/ac/ait/fmipp/exp
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )

      add_custom_command( TARGET fmippex_wrap_java POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_CURRENT_BINARY_DIR}/temp.zip temp.zip
         COMMAND ${Java_JAR_EXECUTABLE} -xf temp.zip
         COMMAND ${CMAKE_COMMAND} -E remove temp.zip
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/at/ac/ait/fmipp/exp )

      install( TARGETS fmippex_wrap_java DESTINATION lib )
      install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${FMIPPEX_JAR_FILENAME} DESTINATION lib )

      set( FMIPPEX_JAR ${CMAKE_CURRENT_BINARY_DIR}/${FMIPPEX_JAR_FILENAME} CACHE INTERNAL "find the created jar. necessary because add_jar and wildcards don't work out here and so it's complicated with dependencies." )
      set( FMIPPEX_WRAP_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "have the directory because we need the path where to look for the dynamic library." )

   endif()

endif ( SWIG_FOUND )
