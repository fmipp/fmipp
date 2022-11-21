# -------------------------------------------------------------------
# Copyright (c) 2013-2020, AIT Austrian Institute of Technology GmbH.
# All rights reserved. See file FMIPP_LICENSE for details.
# -------------------------------------------------------------------

cmake_minimum_required( VERSION 3.8 )


project( fmipp )


# add a target to generate documentation with Doxygen
find_package( Doxygen )
if ( DOXYGEN_FOUND )
   configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen/Doxyfile @ONLY )
   add_custom_target( doxygen_doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" VERBATIM )
endif ()

# stop execution of cmake in case only the documentation is needed
if ( BUILD_DOXYGEN_DOCS_ONLY )
   return()
endif()


# optional builds: to switch the building of different subparts off, use a GUI or "cmake -Doption=OFF [...]"
# when you have switched them off, use the GUI or "cmake -Doption=ON [...]" to switch them on again
option( BUILD_SWIG "SWIG - Simplified Wrapper and Interface Generator stuff." ON )
option( BUILD_TESTS "Unit tests for FMI++." ON )

# include the sundials library and choose use an integrator from CVode
option( INCLUDE_SUNDIALS "Use SUNDIALS integrator suite." OFF )
if ( INCLUDE_SUNDIALS )
   message( "ATTENTION: Sundials will be included!" )

   set( SUNDIALS_INCLUDEDIR "" CACHE PATH "Optional: Sundials include directory." )
   include_directories( ${SUNDIALS_INCLUDEDIR} )

   set( SUNDIALS_LIBRARYDIR "" CACHE PATH "Optional: Sundials library directory." )
   link_directories( ${SUNDIALS_LIBRARYDIR} )

   add_definitions( -DUSE_SUNDIALS )
endif ()


if ( BUILD_SWIG )

   find_package( SWIG REQUIRED )

   option( BUILD_SWIG_JAVA "Build Java wrapper." ON )
   if ( BUILD_SWIG_JAVA )
      find_package( Java REQUIRED )
      find_package( JNI REQUIRED )
      # set( BUILD_SWIG_JAVA_PACKAGE "" CACHE STRING "Optional: Put java wrapper into a package." )

      option( JAVA_MAVEN_INSTALL "Optional: Install java wrapper to local repository." OFF )
      option( JAVA_MAVEN_DEPLOY "Optional: Deploy java wrapper to remote and local repository." OFF )
      if ( JAVA_MAVEN_INSTALL OR JAVA_MAVEN_DEPLOY )
          find_program( MAVEN_EXE mvn REQUIRED )
          message( STATUS "Found Maven: ${MAVEN_EXE}" )
          message( "Will use Maven to install/deploy Java wrapper. Make sure that your instance of Maven is properly configured!" )

          set( JAVA_MAVEN_PKG_VERSION "0.0.1" CACHE STRING "Optional: Define package version for local Maven installation." )
      endif ()

      if ( JAVA_MAVEN_DEPLOY )
          set( JAVA_MAVEN_REPO_ID "github" CACHE STRING "Optional: ID to map on server section of settings.xml." )
          set( JAVA_MAVEN_REPO_URL "https://maven.pkg.github.com/ait-lablink/lablink-fmusim" CACHE STRING "Optional: URL of the repository to deploy." )
      endif ()
   endif()

   option( BUILD_SWIG_PYTHON "Build Python wrapper." ON )
   option( BUILD_SWIG_PYTHON_3 "Force building for Python 3.x." OFF )
   if ( BUILD_SWIG_PYTHON )
      if ( BUILD_SWIG_PYTHON_3 )
         set( BUILD_SWIG_MIN_PYTHON_VERSION "3" )
      else()
         set( BUILD_SWIG_MIN_PYTHON_VERSION "2" )
      endif()
      find_package( PythonInterp ${BUILD_SWIG_MIN_PYTHON_VERSION} REQUIRED )
      find_package( PythonLibs ${BUILD_SWIG_MIN_PYTHON_VERSION} REQUIRED )
   endif()

   option( BUILD_SWIG_OCTAVE "Build Octave wrapper." OFF )
   if ( BUILD_SWIG_OCTAVE )
      find_package( HDF5 REQUIRED )
   endif ()

   option( BUILD_SWIG_MATLAB "Build MATLAB wrapper." OFF )
   if ( BUILD_SWIG_MATLAB )
      find_package( Matlab REQUIRED COMPONENTS MX_LIBRARY )
   endif ()
endif ()


option( DYMOLA2015_WORKAROUND "Use a workaround for the calculation of Jacobians for Dymola FMUs" OFF )
if ( DYMOLA2015_WORKAROUND )
  add_definitions( -DDYMOLA2015_WORKAROUND )
endif()


# set the name of the FMU-binaries-subdirectory according to the current OS
if ( WIN32 )
   if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set( FMU_BIN_DIR "win64" )
   else ()
      set( FMU_BIN_DIR "win32" )
   endif ()
   set( FMU_BIN_EXT ".dll" )
elseif ( APPLE )
   if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set( FMU_BIN_DIR "darwin64" )
   else ()
      set( FMU_BIN_DIR "darwin32" )
   endif ()
   set( FMU_BIN_EXT ".dylib" )
elseif ( CYGWIN )
   if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set( FMU_BIN_DIR "cygwin64" )
   else ()
      set( FMU_BIN_DIR "cygwin32" )
   endif ()
   set( FMU_BIN_EXT ".dll" )
elseif ( UNIX )
   if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set( FMU_BIN_DIR "linux64" )
   else ()
      set( FMU_BIN_DIR "linux32" )
   endif ()
   set( FMU_BIN_EXT ".so" )
endif ()

add_definitions( -DFMU_BIN_DIR="${FMU_BIN_DIR}" )
add_definitions( -DFMU_BIN_EXT="${FMU_BIN_EXT}" )

# OS-specific compiler settings
if ( WIN32 ) # windows-specific
   set( PATH_SEPARATOR ";" )
   set( CMAKE_SHARED_LIBRARY_PREFIX "" )
   set( CMAKE_SHARED_MODULE_PREFIX "" )
   add_definitions( -DWIN32 )
   if ( MINGW )
      add_definitions( -DMINGW -Wall -D_hypot=hypot ) # -O1
      if ( CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7.0" )
         set( CMAKE_CXX_FLAGS "-std=c++0x" )
      else ()
         set( CMAKE_CXX_FLAGS "-std=gnu++11" )
      endif ()
   endif ()
   if ( MSVC )
      # disable auto-linking and use cmake's dependency handling
      add_definitions( -DBOOST_ALL_NO_LIB /W3 )
   endif ()
else () # linux-specific
   set( PATH_SEPARATOR ":" )
   add_definitions( -Wall ) # -O1
      if ( CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7.0" )
         set( CMAKE_CXX_FLAGS "-std=c++0x" )
      else ()
         set( CMAKE_CXX_FLAGS "-std=c++11" )
      endif ()
endif ()


# Boost dependencies here
find_package( Boost 1.72.0 REQUIRED )
if ( Boost_FOUND )
   include_directories( ${Boost_INCLUDE_DIRS} )
   set( BOOST_LIBRARYDIR ${Boost_LIBRARY_DIRS} )

   option( BOOST_STATIC_LINKING "Turn on/off static linking for Boost" OFF )
   if ( BOOST_STATIC_LINKING )
      set( Boost_USE_STATIC_LIBS ON ) # ON when using static linking.
      if ( WIN32 )
        set( Boost_LIB_PREFIX "lib" )
      endif ()
      message( "ATTENTION: Use static linking for Boost libraries!" )
   else ()
      set( Boost_USE_STATIC_LIBS OFF )
   endif ()

   set( Boost_USE_MULTITHREADED ON )
   set( Boost_USE_STATIC_RUNTIME OFF )
endif ()


# common include directories
include_directories( ${fmipp_SOURCE_DIR} )
include_directories( ${fmipp_SOURCE_DIR}/common )
include_directories( ${fmipp_SOURCE_DIR}/common/fmi_v1.0 )


# FMI++ import library
add_subdirectory( import )


# FMI++ export library
add_subdirectory( export )


# add tests for FMI++
if ( BUILD_TESTS )
   enable_testing()
   add_subdirectory( test )

   add_test_fmipp( testFMUModelExchange )
   add_test_fmipp( testFMUIntegrator )
   add_test_fmipp( testIncrementalFMU )
   add_test_fmipp( testRollbackFMU )
   add_test_fmipp( testFMIExportUtilities )
   add_test_fmipp( testFMI2ExportUtilities )
   add_test_fmipp( testFixedStepSizeFMU )
   add_test_fmipp( testVariableStepSizeFMU )
   add_test_fmipp( testInterpolatingFixedStepSizeFMU )
   add_test_fmipp( testModelDescription )
   add_test_fmipp( testModelManager )
   add_test_fmipp( testFMU2SDKImport )
   add_test_fmipp( testFMU2Integrator )
   add_test_fmipp( testFMU2ModelExchange )

   # add tests for SWIG interfaces to FMI++
   if ( BUILD_SWIG )
      if ( BUILD_SWIG_JAVA )
         add_test_fmipp_java( testFMUModelExchange )
         add_test_fmipp_java( testFMUCoSimulation )
         add_test_fmipp_java( testIncrementalFMU )
      endif ()

      if (BUILD_SWIG_PYTHON)
         add_test_fmipp_python( testFMUModelExchange )
         add_test_fmipp_python( testFMUCoSimulation )
         add_test_fmipp_python( testIncrementalFMU )
         add_test_fmipp_python( testRollbackFMU )
         add_test_fmipp_python( testFixedStepSizeFMU )
         add_test_fmipp_python( testVariableStepSizeFMU )
         add_test_fmipp_python( testInterpolatingFixedStepSizeFMU )
      endif ()
   endif ()
endif ()


# Add to local Maven repository.
if ( BUILD_SWIG )
   if ( BUILD_SWIG_JAVA )
      if ( JAVA_MAVEN_INSTALL )
         # Add fmippim.jar to local Maven repository.
         message( STATUS "Add target maven_deploy_fmippim_jar" )
         add_custom_target( maven_install_fmippim_jar ALL
            COMMAND ${MAVEN_EXE} install:install-file -Dfile=${FMIPPIM_JAR} -DgroupId=at.ac.ait.fmipp -DartifactId=imp -Dversion=${JAVA_MAVEN_PKG_VERSION} -Dpackaging=jar -DgeneratePom=true )
         add_dependencies( maven_install_fmippim_jar fmipp_jar )

         # Add fmippex.jar to local Maven repository.
         message( STATUS "Add target maven_deploy_fmippex_jar" )
         add_custom_target( maven_install_fmippex_jar ALL
            COMMAND ${MAVEN_EXE} install:install-file -Dfile=${FMIPPEX_JAR} -DgroupId=at.ac.ait.fmipp -DartifactId=exp -Dversion=${JAVA_MAVEN_PKG_VERSION} -Dpackaging=jar -DgeneratePom=true )
         add_dependencies( maven_install_fmippex_jar fmippex_jar )

         # Add FMI++ shared libs (and dependencies) to local Maven repository.
         if ( MSVC )

            # dll path depends on configuration (Debug, Release, etc.)
            foreach ( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
               if ( INCLUDE_SUNDIALS )
                  string( TOLOWER "sundials-${CMAKE_SYSTEM_NAME}-${OUTPUTCONFIG}-${CMAKE_VS_PLATFORM_NAME}" CONFIG )
                  message( STATUS "Add target maven_deploy_libfmipp_with_sundials_${CONFIG}" )
                  add_custom_target( maven_deploy_libfmipp_with_sundials_${CONFIG} ALL
                     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/import/swig/${OUTPUTCONFIG}/fmippim_wrap_java.dll ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}
                     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/export/swig/${OUTPUTCONFIG}/fmippex_wrap_java.dll ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}
                     COMMAND ${CMAKE_COMMAND} -E copy ${SUNDIALS_LIBRARYDIR}/sundials_cvode.dll ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}
                     COMMAND ${CMAKE_COMMAND} -E copy ${SUNDIALS_LIBRARYDIR}/sundials_nvecserial.dll ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}
                     COMMAND ${CMAKE_COMMAND} -E copy ${SUNDIALS_LIBRARYDIR}/sundials_sunlinsoldense.dll ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}
                     COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG} ${Java_JAR_EXECUTABLE} -cfM libfmipp.zip fmippim.dll fmippex.dll fmippim_wrap_java.dll fmippex_wrap_java.dll sundials_cvode.dll sundials_nvecserial.dll sundials_sunlinsoldense.dll
                     COMMAND ${MAVEN_EXE} install:install-file -Dfile=${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/libfmipp.zip -DgroupId=at.ac.ait.fmipp -DartifactId=libfmipp -Dversion=${JAVA_MAVEN_PKG_VERSION} -Dpackaging=zip -DgeneratePom=true -Dclassifier=natives-libfmipp-${CONFIG} )
                  add_dependencies( maven_deploy_libfmipp_with_sundials_${CONFIG} maven_install_fmippim_jar maven_install_fmippex_jar )
               else ()
                  string( TOLOWER "${CMAKE_SYSTEM_NAME}-${OUTPUTCONFIG}-${CMAKE_VS_PLATFORM_NAME}" CONFIG )
                  message( STATUS "Add target maven_deploy_libfmipp_${CONFIG}" )
                  add_custom_target( maven_deploy_libfmipp_${CONFIG} ALL
                     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/import/swig/${OUTPUTCONFIG}/fmippim_wrap_java.dll ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}
                     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/export/swig/${OUTPUTCONFIG}/fmippex_wrap_java.dll ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}
                     COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG} ${Java_JAR_EXECUTABLE} -cfM libfmipp.zip fmippim.dll fmippex.dll fmippim_wrap_java.dll fmippex_wrap_java.dll
                     COMMAND ${MAVEN_EXE} install:install-file -Dfile=${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/libfmipp.zip -DgroupId=at.ac.ait.fmipp -DartifactId=libfmipp -Dversion=${JAVA_MAVEN_PKG_VERSION} -Dpackaging=zip -DgeneratePom=true -Dclassifier=natives-libfmipp-${CONFIG} )
                  add_dependencies( maven_deploy_libfmipp_${CONFIG} maven_install_fmippim_jar maven_install_fmippex_jar )
               endif()
            endforeach()
         endif()
      endif()

      if ( JAVA_MAVEN_DEPLOY )
         # Add fmippim.jar to remote and local Maven repository.
         message( STATUS "Add target maven_deploy_fmippim_jar" )
         add_custom_target( maven_deploy_fmippim_jar ALL
            COMMAND ${MAVEN_EXE} deploy:deploy-file -Dfile=${FMIPPIM_JAR} -DgroupId=at.ac.ait.fmipp -DartifactId=imp -Dversion=${JAVA_MAVEN_PKG_VERSION} -Dpackaging=jar -DgeneratePom=true -DrepositoryId=${JAVA_MAVEN_REPO_ID} -Durl=${JAVA_MAVEN_REPO_URL} )
         add_dependencies( maven_deploy_fmippim_jar fmipp_jar )

         # Add fmippex.jar to local Maven repository.
         message( STATUS "Add target maven_deploy_fmippex_jar" )
         add_custom_target( maven_deploy_fmippex_jar ALL
            COMMAND ${MAVEN_EXE} deploy:deploy-file -Dfile=${FMIPPEX_JAR} -DgroupId=at.ac.ait.fmipp -DartifactId=exp -Dversion=${JAVA_MAVEN_PKG_VERSION} -Dpackaging=jar -DgeneratePom=true -DrepositoryId=${JAVA_MAVEN_REPO_ID} -Durl=${JAVA_MAVEN_REPO_URL} )
         add_dependencies( maven_deploy_fmippex_jar fmippex_jar )

         # Add FMI++ shared libs (and dependencies) to local Maven repository.
         if ( MSVC )

            # dll path depends on configuration (Debug, Release, etc.)
            foreach ( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
               if ( INCLUDE_SUNDIALS )
                  string( TOLOWER "sundials-${CMAKE_SYSTEM_NAME}-${OUTPUTCONFIG}-${CMAKE_VS_PLATFORM_NAME}" CONFIG )
                  message( STATUS "Add target maven_deploy_libfmipp_with_sundials_${CONFIG}" )
                  add_custom_target( maven_deploy_libfmipp_with_sundials_${CONFIG} ALL
                     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/import/swig/${OUTPUTCONFIG}/fmippim_wrap_java.dll ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}
                     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/export/swig/${OUTPUTCONFIG}/fmippex_wrap_java.dll ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}
                     COMMAND ${CMAKE_COMMAND} -E copy ${SUNDIALS_LIBRARYDIR}/sundials_cvode.dll ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}
                     COMMAND ${CMAKE_COMMAND} -E copy ${SUNDIALS_LIBRARYDIR}/sundials_nvecserial.dll ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}
                     COMMAND ${CMAKE_COMMAND} -E copy ${SUNDIALS_LIBRARYDIR}/sundials_sunlinsoldense.dll ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}
                     COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG} ${Java_JAR_EXECUTABLE} -cfM libfmipp.zip fmippim.dll fmippex.dll fmippim_wrap_java.dll fmippex_wrap_java.dll sundials_cvode.dll sundials_nvecserial.dll sundials_sunlinsoldense.dll
                     COMMAND ${MAVEN_EXE} deploy:deploy-file -Dfile=${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/libfmipp.zip -DgroupId=at.ac.ait.fmipp -DartifactId=libfmipp -Dversion=${JAVA_MAVEN_PKG_VERSION} -Dpackaging=zip -DgeneratePom=true -Dclassifier=natives-libfmipp-${CONFIG} -DrepositoryId=${JAVA_MAVEN_REPO_ID} -Durl=${JAVA_MAVEN_REPO_URL} )
                  add_dependencies( maven_deploy_libfmipp_with_sundials_${CONFIG} maven_deploy_fmippim_jar maven_deploy_fmippex_jar )
               else ()
                  string( TOLOWER "${CMAKE_SYSTEM_NAME}-${OUTPUTCONFIG}-${CMAKE_VS_PLATFORM_NAME}" CONFIG )
                  message( STATUS "Add target maven_deploy_libfmipp_${CONFIG}" )
                  add_custom_target( maven_deploy_libfmipp_${CONFIG} ALL
                     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/import/swig/${OUTPUTCONFIG}/fmippim_wrap_java.dll ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}
                     COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/export/swig/${OUTPUTCONFIG}/fmippex_wrap_java.dll ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}
                     COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/${OUTPUTCONFIG} ${Java_JAR_EXECUTABLE} -cfM libfmipp.zip fmippim.dll fmippex.dll fmippim_wrap_java.dll fmippex_wrap_java.dll
                     COMMAND ${MAVEN_EXE} deploy:deploy-file -Dfile=${CMAKE_BINARY_DIR}/${OUTPUTCONFIG}/libfmipp.zip -DgroupId=at.ac.ait.fmipp -DartifactId=libfmipp -Dversion=${JAVA_MAVEN_PKG_VERSION} -Dpackaging=zip -DgeneratePom=true -Dclassifier=natives-libfmipp-${CONFIG} -DrepositoryId=${JAVA_MAVEN_REPO_ID} -Durl=${JAVA_MAVEN_REPO_URL} )
                  add_dependencies( maven_deploy_libfmipp_${CONFIG} maven_deploy_fmippim_jar maven_deploy_fmippex_jar )
               endif()
            endforeach()
         endif()
      endif()

   endif()
endif()