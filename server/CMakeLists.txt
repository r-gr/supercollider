if (NOT APPLE AND NOT WIN32)
	# on apple we leave this undefined so it defaults to .scx (in the code)
	add_definitions("-DSC_PLUGIN_EXT=\".so\"")
endif()

if(UNIX AND NOT APPLE)
	set(LINUX_SC_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/SuperCollider/plugins)
endif()

include_directories(${CMAKE_SOURCE_DIR}/external_libraries
					${CMAKE_SOURCE_DIR}/external_libraries/nova-simd
					${CMAKE_SOURCE_DIR}/external_libraries/nova-tt
					)

if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
else()
	include_directories(${CMAKE_SOURCE_DIR}/external_libraries/boost)
endif()

# here we choose who provides us with the FFT lib
if (APPLE)
	add_definitions("-DSC_FFT_VDSP")
else()
	find_package(FFTW3f 3.3)

	if (NOT FFTW3F_FOUND OR FFT_GREEN)
		message(STATUS "Using green fft")
		set(FFT_GREEN 1)
		add_definitions("-DSC_FFT_GREEN")
	else()
		if(WIN32)
			message(STATUS "Found fftw3f: ${FFTW3F_LIBRARY}")
		else(WIN32)
			message(STATUS "Using fftw3f")
		endif(WIN32)
		add_definitions("-DSC_FFT_FFTW")
	endif()
endif()

add_subdirectory(plugins)
add_subdirectory(scsynth)

if (SUPERNOVA)
	add_subdirectory(supernova)
endif()

set(plugins "${plugins}" PARENT_SCOPE)
set(supernova_plugins "${supernova_plugins}" PARENT_SCOPE)

# Build scvid, the video server
option(STACK_USE_SYSTEM_GHC "Pass --system-ghc to the Haskell Stack tool" OFF)
if (APPLE)
	message(SEND_ERROR "Not building scvid because macOS is not currently supported.")
elseif(WIN32)
	message(SEND_ERROR "Not building scvid because Windows is not supported.")
else()
	if (NOT LINUX)
		message(WARNING "Attempting to build scvid but this may fail because only Linux is supported.")
	endif()

	find_program(Stack_FOUND stack)
	if (Stack_FOUND)
		message(STATUS "Found Haskell Stack tool: ${Stack_FOUND}")
		file(COPY scvid DESTINATION ${CMAKE_BINARY_DIR}/server)
		if (STACK_USE_SYSTEM_GHC)
			add_custom_target(scvid ALL
				stack build --system-ghc --copy-bins --local-bin-path ${CMAKE_BINARY_DIR}/server/scvid
				COMMAND mv ${CMAKE_BINARY_DIR}/server/scvid/video-server-exe ${CMAKE_BINARY_DIR}/server/scvid/scvid
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/server/scvid
			)
		else()
			message(STATUS "Haskell Stack may produce a lot of text output now...")
			execute_process(COMMAND stack setup WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/server/scvid) # OUTPUT_QUIET ERROR_QUIET)
			add_custom_target(scvid ALL
				stack build --copy-bins --local-bin-path ${CMAKE_BINARY_DIR}/server/scvid
				COMMAND mv ${CMAKE_BINARY_DIR}/server/scvid/video-server-exe ${CMAKE_BINARY_DIR}/server/scvid/scvid
				WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/server/scvid
			)
		endif()
	else()
		message(FATAL_ERROR "Haskell Stack tool not found, cannot compile scvid.")
	endif()
endif()

# install(CODE "execute_process (COMMAND stack install --local-bin-path ${CMAKE_INSTALL_PREFIX}/bin WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/haskell-tool)")
