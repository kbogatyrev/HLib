add_executable(HLibStringTest
        StringTest.cpp
)

target_link_libraries(HLibStringTest
        PRIVATE
        HLib
)