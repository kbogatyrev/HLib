cmake_minimum_required (VERSION 3.16)

project (Zal-Core VERSION 5.0.0)

set(CMAKE_CXX_STANDARD 17)

#
#	String tests
#
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
			/source-charset:utf-8
			/wd4996		# unsafe function -- ignored
			/JMC /ifcOutput "out/build/x64-Debug/" /GS /W3 /Zc:wchar_t 
			/Zi /Gm- /Od #/Fd "out/build/x64-Debug/ZalMainLib.pdb" 
			/Zc:inline /fp:precise #/D "_DEBUG" 
			/errorReport:prompt /WX- /Zc:forScope /RTC1 /Gd /MDd /std:c++17 /FC 
#			/Fa "out/build/x64-Debug" /EHsc /nologo /Fo "out/build/x64-Debug" /Fp "out/build/x64-Debug/ZalMainLib.pch" 
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
			/source-charset:utf-8
			/wd4996		# unsafe function -- ignored
			/ifcOutput "out/build/x64-Release/" /GS /GL /W3 /Gy /Zc:wchar_t 
			/Zi /Gm- /O2 #/Fd"C:\dev\Zal-Windows\lib\x64\Release\ZalMainLib.pdb" 
			/Zc:inline /fp:precise 
			/errorReport:prompt /WX- /Zc:forScope /Gd /Oi /MD 
			/std:c++17 /FC 
			#/Fa"x64\Release\" 
			/EHsc /nologo 
			#/Fo"x64\Release\" /Fp"x64\Release\ZalMainLib.pch" 
			/diagnostics:column 
		)
	endif()
else()
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#	set(CMAKE_CXX_STANDARD 17)
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable (ZalStringTest)

target_sources (ZalStringTest PRIVATE
	StringTest.cpp
) 


