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
add_executable(r_snake_40x25
    examples/snake/snake.hpp
    examples/snake/snake_40x25.cpp
)
target_link_libraries(r_snake_40x25 PRIVATE rowguelike)

###
add_executable(r_horizontal_space
    examples/horizontal_space/horizontal_space.hpp
    examples/horizontal_space/horizontal_space.cpp
)

target_link_libraries(r_horizontal_space PRIVATE rowguelike)

###
add_executable(r_birds
    examples/birds/birds.hpp
    examples/birds/birds.cpp
)

target_link_libraries(r_birds PRIVATE rowguelike)

###
add_executable(r_screensaver
    examples/screensaver/screensaver.hpp
    examples/screensaver/screensaver.cpp
)

target_link_libraries(r_screensaver PRIVATE rowguelike)

###
add_executable(r_pages
    examples/pages/pages.hpp
    examples/pages/pages.cpp
)

target_link_libraries(r_pages PRIVATE rowguelike)

###
add_executable(r_demo3d
    examples/demo3d/demo3d.hpp
    examples/demo3d/demo3d.cpp
)

target_link_libraries(r_demo3d PRIVATE rowguelike)

###
add_executable(r_pixel_arcade
    examples/pixel_arcade/pixel_arcade.hpp
    examples/pixel_arcade/pixel_arcade.cpp
)

target_link_libraries(r_pixel_arcade PRIVATE rowguelike)

###
add_custom_target(docs)
target_sources(docs PRIVATE
    README.md
    TODO.txt
    library.properties
    library.json
)
