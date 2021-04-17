cmake_minimum_required(VERSION 3.15)

find_package(Vulkan REQUIRED)

add_library(RxCore STATIC 
        "src/Vulkan/Device.cpp"
        "src/Vulkan/Device.h"
        "src/Vulkan/Instance.cpp"
        "src/Vulkan/Instance.hpp"
        "src/Vulkan/PhysicalDevice.cpp"
        "src/Vulkan/PhysicalDevice.hpp"
        "src/Vulkan/Surface.cpp"
        "src/Vulkan/Surface.hpp"
        "src/Vulkan/SwapChain.cpp"
        "src/Vulkan/SwapChain.hpp"
        "src/Vulkan/PipelineBuilder.cpp"
        "src/Vulkan/PipelineBuilder.hpp"
        "src/Vulkan/CommandBuffer.cpp"
        "src/Vulkan/CommandBuffer.hpp"
        src/Vulkan/Vulk.hpp
        src/Vulkan/Buffer.cpp
        src/Vulkan/Buffer.hpp
        src/Vulkan/VertexBuffer.cpp
        src/Vulkan/VertexBuffer.h
        src/Vulkan/IndexBuffer.hpp
        src/Vulkan/Image.hpp
        src/Vulkan/Image.cpp
        src/Vulkan/DescriptorSet.hpp
        src/Vulkan/DescriptorSet.cpp
        src/Vulkan/DescriptorPool.cpp
        src/Vulkan/DescriptorPool.hpp
        src/Vulkan/Queue.cpp
        src/Vulkan/Queue.hpp
        src/Vulkan/CommandPool.cpp
        src/Vulkan/CommandPool.hpp
        src/Vulkan/VMA.cpp 
        src/RXCore.h
        src/optick/optick_core.cpp
        src/optick/optick_server.cpp
        src/optick/optick_serialization.cpp
        src/optick/optick_miniz.cpp
        src/optick/optick_message.cpp
        src/optick/optick_gpu.cpp
        src/optick/optick_gpu.vulkan.cpp
        src/Jobs/JobManager.cpp
        src/Jobs/JobManager.hpp
        src/Vulkan/FrameBuffer.h
        src/Vulkan/Shader.h
        src/Vulkan/DeviceObject.h
        src/Vulkan/Allocation.cpp
        src/Vulkan/Allocation.h
        src/Vulkan/IPipelineLayout.h
        src/Jobs/ThreadResources.cpp
        src/Jobs/ThreadResources.h 
        src/Vulkan/Pipeline.h)

#target_compile_features(RxCore PUBLIC cxx_std_20)
target_compile_options(RxCore PUBLIC -DNOMINMAX)
target_compile_options(RxCore PUBLIC -DUSE_OPTICK=1)
target_compile_options(RxCore PUBLIC -DVK_USE_PLATFORM_WIN32_KHR)
target_compile_options(RxCore PUBLIC -DGLFW_INCLUDE_VULKAN -DVK_USE_PLATFORM_WIN32_KHR -DGLFW_EXPOSE_NATIVE_WIN32 )
#target_compile_options(RxCore PUBLIC -DGLM_FORCE_SILENT_WARNINGS)
#target_compile_options(RxCore PUBLIC -DGLM_FORCE_DEPTH_ZERO_TO_ONE -DGLM_FORCE_XYZW_ONLY)# -DGLM_FORCE_LEFT_HANDED)
#target_compile_options(RxCore PUBLIC -DRX_ASSET_DIR="${PROJECT_SOURCE_DIR}/assets")

target_compile_options(RxCore PUBLIC -DVULKAN_HPP_NO_SPACESHIP_OPERATOR=1)
# /wd4275 /wd4305 /wd4310
if (MSVC)
    #target_compile_options(RxCore PUBLIC /std:c++17)
    target_compile_options(RxCore PRIVATE /arch:AVX2 /W4 /WX /wd4100)
    target_compile_options(RxCore PUBLIC -D_CRT_SECURE_NO_WARNINGS -D_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING -DWIN32_LEAN_AND_MEAN)
    set_source_files_properties(src/Vulkan/VMA.cpp PROPERTIES COMPILE_FLAGS /W3 )
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(RxCore PRIVATE -Werror)
        target_compile_options(RxCore PRIVATE -Wno-missing-field-initializers)
        target_compile_options(RxCore PRIVATE -Wno-unused-variable)
        target_compile_options(RxCore PRIVATE -Wno-unused-function)
    set_source_files_properties(src/Vulkan/VMA.cpp PROPERTIES COMPILE_FLAGS -Wno-unused-function )
    endif()
endif ()
#target_compile_features(RxCore PRIVATE cxx_std_20)
#target_compile_options(RxCore PUBLIC -DVULKAN_HPP_NO_SMART_HANDLE)
#target_compile_options(RxCore PUBLIC -DVULKAN_HPP_NO_STRUCT_CONSTRUCTORS)
#target_compile_options(RxCore PUBLIC -DSPDLOG_ACTIVE_LEVEL=0)
target_compile_options(RxCore PUBLIC -DVULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
target_include_directories(RxCore PUBLIC "src")
target_include_directories(RxCore PUBLIC ${Vulkan_INCLUDE_DIR})
target_link_libraries(RxCore PUBLIC ${Vulkan_LIBRARY})
target_link_libraries(RxCore PUBLIC glfw)
target_link_libraries(RxCore PUBLIC RXUtil)
set_target_properties(RxCore PROPERTIES CXX_STANDARD 20)
