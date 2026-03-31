function(funcdoodle_set_portaudio_defaults)
	set(PORTAUDIO_LIBDIR "" CACHE STRING "required")
	set(PORTAUDIO_INCLDIR "" CACHE STRING "required")
	set(PORTAUDIO_STATIC ON CACHE BOOL "Link PortAudio statically")
	set(PORTAUDIO_LIBNAME "" CACHE STRING "Override PortAudio library name")

	if(NOT PORTAUDIO_LIBDIR)
		if(APPLE)
			set(PORTAUDIO_LIBDIR "/usr/local/opt/portaudio/lib/")
		elseif(UNIX AND NOT APPLE)
			if(EXISTS "/lib64/libportaudio.a" OR EXISTS "/lib64/libportaudio.so")
				set(PORTAUDIO_LIBDIR "/lib64/")
			else()
				set(PORTAUDIO_LIBDIR "/usr/lib/")
			endif()
		elseif(WIN32)
			set(PORTAUDIO_LIBDIR "C:\\portaudio\\lib")
		endif()
	endif()

	if(NOT PORTAUDIO_INCLDIR)
		if(APPLE)
			set(PORTAUDIO_INCLDIR "/usr/local/opt/portaudio/include")
		elseif(UNIX AND NOT APPLE)
			if(EXISTS "/usr/include/portaudio.h")
				set(PORTAUDIO_INCLDIR "/usr/include")
			else()
				set(PORTAUDIO_INCLDIR "/usr/local/include")
			endif()
		elseif(WIN32)
			set(PORTAUDIO_INCLDIR "C:\\portaudio\\incl")
		endif()
	endif()

	set(PORTAUDIO_LIBDIR "${PORTAUDIO_LIBDIR}" PARENT_SCOPE)
	set(PORTAUDIO_INCLDIR "${PORTAUDIO_INCLDIR}" PARENT_SCOPE)
endfunction()

function(funcdoodle_find_portaudio out_var)
	if(PORTAUDIO_LIBNAME)
		set(_porta_names ${PORTAUDIO_LIBNAME})
	elseif(WIN32)
		if(PORTAUDIO_STATIC)
			set(_porta_names portaudio_x64_static portaudio_static portaudio_x64)
		else()
			set(_porta_names portaudio_x64 portaudio)
		endif()
	else()
		set(_porta_names portaudio)
	endif()

	if(NOT WIN32 AND PORTAUDIO_STATIC)
		set(_old_suffixes ${CMAKE_FIND_LIBRARY_SUFFIXES})
		set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
	endif()

	find_library(${out_var} NAMES ${_porta_names} PATHS ${PORTAUDIO_LIBDIR}
		NO_DEFAULT_PATH)
	if(NOT ${out_var})
		find_library(${out_var} NAMES ${_porta_names})
	endif()

	if(NOT WIN32 AND PORTAUDIO_STATIC)
		set(CMAKE_FIND_LIBRARY_SUFFIXES ${_old_suffixes})
	endif()

	if(NOT ${out_var})
		message(FATAL_ERROR
			"PortAudio library not found. Set PORTAUDIO_LIBDIR or PORTAUDIO_LIBNAME. "
			"If you only have a shared library, set PORTAUDIO_STATIC=OFF.")
	endif()
endfunction()

function(funcdoodle_apply_platform_links target)
	if(UNIX)
		target_link_libraries(${target} PRIVATE
			glfw
			${CMAKE_DL_LIBS}
			m
			pthread
			dl
		)
	elseif(WIN32)
		target_link_libraries(${target} PRIVATE
			glfw
			${CMAKE_DL_LIBS}
		)
	endif()

	if(UNIX AND NOT APPLE)
		find_package(PkgConfig REQUIRED)
		pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
		target_link_libraries(${target} PRIVATE ${GTK3_LIBRARIES})
		target_include_directories(${target} PRIVATE ${GTK3_INCLUDE_DIRS})
	endif()

	if(NOT FUNCDOODLE_USE_BUNDLED_PORTAUDIO)
		funcdoodle_find_portaudio(PORTAUDIO_LIB)

		if(APPLE)
			target_link_libraries(${target}
				PRIVATE
				${PORTAUDIO_LIB}
				"-framework OpenGL"
				"-framework Cocoa"
				"-framework IOKit"
				"-framework CoreFoundation"
				"-framework AppKit"
			)
		elseif(UNIX AND NOT APPLE)
			target_link_libraries(${target} PRIVATE
				${PORTAUDIO_LIB}
			)
		elseif(WIN32)
			target_link_libraries(${target} PRIVATE
				${CMAKE_DL_LIBS}
				${PORTAUDIO_LIB}
			)
		endif()
	endif()
endfunction()
