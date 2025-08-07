# Include framework-specific source files and libraries
if (DEFAULT_BACKEND STREQUAL "OPENCV_DNN")
    target_include_directories(${PROJECT_NAME} PRIVATE ${INFER_ROOT}/opencv-dnn/src)
elseif (DEFAULT_BACKEND STREQUAL "ONNX_RUNTIME")
    target_include_directories(${PROJECT_NAME} PRIVATE ${ONNX_RUNTIME_DIR}/include ${INFER_ROOT}/onnx-runtime/src)
    target_link_directories(${PROJECT_NAME} PRIVATE ${ONNX_RUNTIME_DIR}/lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${ONNX_RUNTIME_DIR}/lib/libonnxruntime.so)
elseif (DEFAULT_BACKEND STREQUAL "LIBTORCH")
    target_include_directories(${PROJECT_NAME} PRIVATE ${INFER_ROOT}/libtorch/src)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${TORCH_LIBRARIES})
    target_compile_definitions(${PROJECT_NAME} PRIVATE C10_USE_GLOG)
elseif (DEFAULT_BACKEND STREQUAL "TENSORRT")
    target_include_directories(${PROJECT_NAME} PRIVATE /usr/local/cuda/include ${TENSORRT_DIR}/include ${INFER_ROOT}/tensorrt/src)
    target_link_directories(${PROJECT_NAME} PRIVATE  /usr/local/cuda/lib64 ${TENSORRT_DIR}/lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE nvinfer nvonnxparser cudart)
elseif(DEFAULT_BACKEND STREQUAL "LIBTENSORFLOW" )
    # Set TensorFlow include directories for the target
    target_include_directories(${PROJECT_NAME} PRIVATE ${TensorFlow_INCLUDE_DIR} ${INFER_ROOT}/libtensorflow/src)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${TensorFlow_CC_LIBRARY} ${TensorFlow_FRAMEWORK_LIBRARY})
elseif(DEFAULT_BACKEND STREQUAL "OPENVINO")
    target_include_directories(${PROJECT_NAME} PRIVATE ${InferenceEngine_INCLUDE_DIRS} ${INFER_ROOT}/openvino/src)
    target_link_libraries(${PROJECT_NAME} PRIVATE openvino::runtime )
elseif(DEFAULT_BACKEND STREQUAL "GGML")
    target_include_directories(${PROJECT_NAME} PRIVATE ${GGML_DIR}/include ${INFER_ROOT}/ggml/src)
    target_link_directories(${PROJECT_NAME} PRIVATE ${GGML_DIR}/lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        ${GGML_DIR}/lib/libggml-base.so
        ${GGML_DIR}/lib/libggml-cpu.so
        ${GGML_DIR}/lib/libggml-blas.so
    )
endif()
