
message(STATUS "Test enabled")
find_package(GTest REQUIRED)
enable_testing()
# Add test directories
if(DEFAULT_BACKEND STREQUAL "LIBTENSORFLOW" )
    add_subdirectory(backends/libtensorflow/test)
elseif(DEFAULT_BACKEND STREQUAL "LIBTORCH" )
    add_subdirectory(backends/libtorch/test)
elseif(DEFAULT_BACKEND STREQUAL "ONNX_RUNTIME")    
    add_subdirectory(backends/onnx-runtime/test)
elseif (DEFAULT_BACKEND STREQUAL "OPENCV_DNN")
    add_subdirectory(backends/opencv-dnn/test)
elseif(DEFAULT_BACKEND STREQUAL "TENSORRT")    
    add_subdirectory(backends/tensorrt/test)
elseif(DEFAULT_BACKEND STREQUAL "OPENVINO")
    add_subdirectory(backends/openvino/test)    
endif()    
