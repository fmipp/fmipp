cmake_minimum_required( VERSION 2.8.12 )

project( fmippim_wrap )

find_package( SWIG )
if ( SWIG_FOUND )
   include( UseSWIG )

   set_source_files_properties( libfmippim.i PROPERTIES CPLUSPLUS ON )

   if ( BUILD_SWIG_PYTHON )
      find_package( PythonInterp REQUIRED )
      find_package( PythonLibs REQUIRED )
      include_directories( ${PYTHON_INCLUDE_DIRS} )

      set( FMIPP_WRAP_PYTHON_MODULE "fmippim_wrap_python" )
      # change the output name of the python dynamic library to _fmippim, otherwise pyhton does not find it when importing...

      swig_add_module( ${FMIPP_WRAP_PYTHON_MODULE} python libfmippim.i )
      set_target_properties( ${SWIG_MODULE_${FMIPP_WRAP_PYTHON_MODULE}_REAL_NAME} PROPERTIES OUTPUT_NAME "_fmippim" )
      swig_link_libraries( ${FMIPP_WRAP_PYTHON_MODULE} fmippim ${PYTHON_LIBRARIES} )
   endif()

   if ( BUILD_SWIG_JAVA )
      find_package( Java REQUIRED )
      include( UseJava )
      find_package( JNI REQUIRED )
      include_directories( ${JAVA_INCLUDE_PATH} )
      include_directories( ${JAVA_INCLUDE_PATH2} )

      # ATTENTION: The following lines are needed to place a generated java wrapper into an appropriate package.
      # However, the according flag causes a swig error when generating a Python wrapper (unrecognized option).
      # Therefore, the following command has to issued only AFTER the python wrapper module has been declared!
      if ( NOT ${BUILD_SWIG_JAVA_PACKAGE} STREQUAL "" )
         set_property( SOURCE libfmippim.i PROPERTY SWIG_FLAGS "-package" "${BUILD_SWIG_JAVA_PACKAGE}" )
      endif()

      swig_add_module( fmippim_wrap_java java libfmippim.i )
      swig_link_libraries( fmippim_wrap_java fmippim )
   
      if ( WIN32 )
         if ( MINGW )
            # add aliases because otherwise JNI doesn't find the functions in the dll
            set_target_properties( fmippim_wrap_java PROPERTIES LINK_FLAGS "-Wl,--add-stdcall-alias" )
	 endif ()
      endif ()

      set( FMIPPIM_JAR_FILENAME "fmippim.jar" )
      add_custom_command( OUTPUT ${FMIPPIM_JAR_FILENAME} DEPENDS fmippim_wrap_java
                              COMMAND ${CMAKE_COMMAND} -E echo "Compiling Java files..."
			      COMMAND ${Java_JAVAC_EXECUTABLE} *.java
			      COMMAND ${CMAKE_COMMAND} -E echo "Creating jar file..."
			      COMMAND ${Java_JAR_EXECUTABLE} -cf fmippim.jar *.class
			      MAIN_DEPENDENCY ${FMIPPIM_JAR_FILENAME})

      add_custom_target( fmippim_jar ALL DEPENDS ${FMIPPIM_JAR_FILENAME} )

      install( TARGETS fmippim_wrap_java DESTINATION lib )
      install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${FMIPPIM_JAR_FILENAME} DESTINATION lib )

      set( FMIPPIM_JAR ${CMAKE_CURRENT_BINARY_DIR}/${FMIPPIM_JAR_FILENAME} CACHE INTERNAL "find the created jar. necessary because add_jar and wildcards don't work out here and so it's complicated with dependencies." )
      set( FMIPPIM_WRAP_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "have the directory because we need the path where to look for the dynamic library." )

   endif()

endif ( SWIG_FOUND )
