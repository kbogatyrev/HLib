cmake_minimum_required (VERSION 3.16)

project (HLib VERSION 4.6.3)

if (MSVC)
	if (CMAKE_BUILD_TYPE MATCHES "Debug")
		message (STATUS "Windows debug mode")
		add_compile_definitions(
			WIN32
			_WINDOWS
			_UNICODE
			UNICODE
			_DEBUG
			_CONSOLE
		)
		add_compile_options(
			/JMC /ifcOutput "out/build/x64-Debug/" /GS /W3 /Zc:wchar_t 
			/Zi /Gm- /Od 
			/Zc:inline /fp:precise 
			/errorReport:prompt /WX- /Zc:forScope /RTC1 /Gd /MDd /std:c++17 /FC 
			/diagnostics:column 
		)
	else()
		message (STATUS "Windows release mode")
		add_compile_definitions(
			WIN32
			_WINDOWS
			_UNICODE
			UNICODE
			_CONSOLE
			NDEBUG
		)
		add_compile_options(
			/ifcOutput "out/build/x64-Release/" /GS /GL /W3 /Gy /Zc:wchar_t 
			/Zi /Gm- /O2 
			/Zc:inline /fp:precise 
			/errorReport:prompt /WX- /Zc:forScope /Gd /Oi /MD 
			/std:c++17 /FC 
			/EHsc /nologo 
			/diagnostics:column 
		)
	endif()
else()
	set(CMAKE_CXX_STANDARD 20)
    add_compile_options(-Wall -Wextra -Werror -Wno-unused-function)
endif()

add_executable (StringTest)

target_include_directories (StringTest PUBLIC
#	../HLib
#	../ThirdParty/Sqlite
#	../ThirdParty/sha1
#	../ThirdParty/spdlog/include/spdlog
#	/usr/include/spdlog
)

target_sources (StringTest PRIVATE
    StringTest.cpp)

#if (CMAKE_BUILD_TYPE MATCHES "Debug")
#	add_custom_command(TARGET ZalMainLib POST_BUILD
#		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ZalMainLib> ../../../../Zal-Windows/TestApp/bin/debug
#	)
#else()
#	add_custom_command(TARGET ZalMainLib POST_BUILD
#		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ZalMainLib> ../../../../Zal-Windows/TestApp/bin/release
#	)
#endif()
