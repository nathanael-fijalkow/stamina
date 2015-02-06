cmake_minimum_required (VERSION 2.6)
project (Acme++)

option(MSVC "Compile for Visual Studio" OFF)
if(MSVC)
	option(MSVC_SHARED_RT "Used shared runtime libraries" OFF)
	
		IF(NOT MSVC_SHARED_RT)
	foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
		if(${flag_var} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
	endif(${flag_var} MATCHES "/MD")
	endforeach(flag_var)
	ENDIF(NOT MSVC_SHARED_RT)

	add_definitions(-D_USE_MATH_DEFINES)
	add_definitions(-DMSVC)
	add_definitions(-D_WINSOCKAPI_)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	
	set(SDKDIRECTORY
		"C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib"
		CACHE STRING "Directory containing librairies such as ksguid.lib"
		)
	
	link_directories("${SDKDIRECTORY}")
else()
set(CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -std=c++11")
endif (MSVC)

option (HAVE_COLORS
    "Enable terminal colors" ON)
set(CMAKE_BUILD_TYPE Debug)
if (HAVE_COLORS)
    add_definitions (-DHAVE_COLORS)
endif (HAVE_COLORS)


set (EXTRA_LIBS
)

include_directories(
"${PROJECT_SOURCE_DIR}"
)

include (Sources.cmake)

set (ALL_SOURCES ${SOURCES}
Sources.cmake
)

# ADD HEADERS FILES 
foreach (SOURCE ${ALL_SOURCES})
	set (FILENAME_PREFIX)
	set (FILENAME_PATH)
	GET_FILENAME_COMPONENT(FILENAME_PREFIX ${SOURCE} NAME_WE)
	GET_FILENAME_COMPONENT(FILENAME_PATH ${SOURCE} PATH)
	
		message("${SOURCE}")

	if (FILENAME_PATH STREQUAL "")
		set (FILENAME ${FILENAME_PREFIX})
	else ()
		set (FILENAME ${FILENAME_PATH}/${FILENAME_PREFIX})
	endif()
	
	if(EXISTS "${PROJECT_SOURCE_DIR}/${FILENAME}.h")
		message("Adding header " ${FILENAME}.h)
        set (ALL_HEADERS ${ALL_HEADERS} "${FILENAME}.h")
	endif(EXISTS "${PROJECT_SOURCE_DIR}/${FILENAME}.h")
	if(EXISTS "${PROJECT_SOURCE_DIR}/${FILENAME}.hpp")
		message("Adding header " ${FILENAME}.hpp)
        set (ALL_HEADERS ${ALL_HEADERS} "${FILENAME}.hpp")
	endif(EXISTS "${PROJECT_SOURCE_DIR}/${FILENAME}.hpp")
endforeach (SOURCE)


add_executable (AcmeTest ${ALL_SOURCES} ${ALL_HEADERS} Test.cpp)
target_link_libraries (AcmeTest ${EXTRA_LIBS})

add_executable (OneLeakTest ${ALL_SOURCES} ${ALL_HEADERS} OneLeak.cpp)
target_link_libraries (OneLeakTest ${EXTRA_LIBS})

add_executable (Statistics ${ALL_SOURCES} ${ALL_HEADERS} Statistics.cpp)
target_link_libraries (Statistics ${EXTRA_LIBS})

add_executable (TestAut ${ALL_SOURCES} ${ALL_HEADERS} TestAut.cpp)
target_link_libraries (TestAut ${EXTRA_LIBS})

add_executable (TestStarHeight ${ALL_SOURCES} ${ALL_HEADERS} TestStarHeight.cpp)
target_link_libraries (TestStarHeight ${EXTRA_LIBS})
