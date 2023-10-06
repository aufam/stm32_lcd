file(READ version.txt LCD_VERSION)
message("LCD_VERSION : ${LCD_VERSION}")

# sources
file(GLOB_RECURSE LCD_SOURCES lcd/*.*)
add_library(lcd ${LCD_SOURCES})

# include dirs
target_include_directories(lcd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# defines
target_compile_definitions(lcd PUBLIC -DLCD_VERSION="${LCD_VERSION}")

# depends
target_link_libraries(lcd etl)
target_link_libraries(lcd periph)