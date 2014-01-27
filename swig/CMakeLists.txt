cmake_minimum_required(VERSION 2.8)

project(fmipp_wrap)

find_package(SWIG)
if (SWIG_FOUND)
   include(UseSWIG)

   find_package(Java REQUIRED)
   include(UseJava)
   find_package(JNI REQUIRED)
   include_directories(${JAVA_INCLUDE_PATH})
   include_directories(${JAVA_INCLUDE_PATH2})
   include_directories(../include)

   set_source_files_properties(libfmipp.i PROPERTIES CPLUSPLUS ON)
   swig_add_module(fmipp_wrap java libfmipp.i)
   swig_link_libraries(fmipp_wrap fmipp)

   add_custom_command(OUTPUT fmipp.jar DEPENDS fmipp_wrap
   			     COMMAND ${CMAKE_COMMAND} -E echo "Compiling Java files..."
			     COMMAND ${Java_JAVAC_EXECUTABLE} *.java
			     COMMAND ${CMAKE_COMMAND} -E echo "Creating jar file..."
			     COMMAND ${Java_JAR_EXECUTABLE} -cf fmipp.jar *.class)

   add_custom_target(fmipp_jar ALL DEPENDS fmipp.jar)

   install(TARGETS fmipp_wrap DESTINATION lib)
   install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fmipp.jar DESTINATION lib)

endif (SWIG_FOUND)
