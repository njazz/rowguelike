project(rowguelike)

set(CMAKE_CXX_STANDARD 11)
enable_language(CXX)

add_library(rowguelike INTERFACE
    src/rowguelike.hpp

    src/r_terminal.hpp
    src/r_lcd.hpp
)

add_executable(rowguelike_tests
   tests/rowguelike_tests.cpp
)

target_link_libraries(rowguelike_tests PRIVATE rowguelike)
include_directories(src)

###
add_executable(r_pong
    examples/pong/pong.hpp
    examples/pong/pong.cpp
)
target_link_libraries(r_pong PRIVATE rowguelike)

###
add_executable(r_one
    examples/one/one.hpp
    examples/one/one.cpp
)
target_link_libraries(r_one PRIVATE rowguelike)

###
add_executable(r_snake
    examples/snake/snake.hpp
    examples/snake/snake.cpp
)
target_link_libraries(r_snake PRIVATE rowguelike)

###
add_executable(r_horizontal_space
    examples/horizontal_space/horizontal_space.hpp
    examples/horizontal_space/horizontal_space.cpp
)

target_link_libraries(r_horizontal_space PRIVATE rowguelike)

###
add_executable(r_custom
    examples/custom/custom.hpp
    examples/custom/custom.cpp
)

target_link_libraries(r_custom PRIVATE rowguelike)

###
add_custom_target(docs)
target_sources(docs PRIVATE
    README.md
    TODO.txt
    library.properties
    library.json
)
