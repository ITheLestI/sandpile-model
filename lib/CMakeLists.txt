add_compile_definitions("DEBUG=$<CONFIG:Debug>")
add_library(bmp BMPWriter.cc BMPWriter.h)
add_library(sandpile Sandpile.cc Sandpile.h)
add_library(arg_parser ArgParser.cc ArgParser.h)
add_library(queue Queue.h Queue.cc)
target_link_libraries(sandpile PRIVATE bmp)
target_link_libraries(bmp PRIVATE sandpile)
target_link_libraries(sandpile PRIVATE queue)
