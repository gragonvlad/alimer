project(volk)
cmake_minimum_required(VERSION 3.0)

add_library(volk STATIC volk.c)

if(MSVC)
  target_compile_options(volk PRIVATE /W4 /WX)
else()
  target_compile_options(volk PRIVATE -Wall -Wextra -Werror)
endif()

target_include_directories(volk PUBLIC "${ALIMER_THIRD_PARTY_DIR}")
# if(DEFINED ENV{VULKAN_SDK})
# 	target_include_directories(volk PUBLIC "$ENV{VULKAN_SDK}/include")
# endif()
