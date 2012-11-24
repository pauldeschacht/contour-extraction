add_executable(img_to_rgb img_to_rgb.cpp)

find_package(ImageMagick REQUIRED)
include_directories("./" "/opt/local/include/ImageMagick")
target_link_libraries(img_to_rgb "/opt/local/lib/libMagickWand.dylib")
