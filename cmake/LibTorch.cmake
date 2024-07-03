# LibTorch Configuration

# Set LibTorch directory (modify accordingly)
set(Torch_DIR $ENV{HOME}/libtorch/share/cmake/Torch/ CACHE PATH "Path to libtorch")

# Find LibTorch
find_package(Torch REQUIRED)


set(LIBTORCH_SOURCES
    ${INFER_ROOT}/libtorch/LibtorchInfer.cpp
    # Add more LibTorch source files here if needed
)

# Append LibTorch sources to the main sources
list(APPEND SOURCES ${LIBTORCH_SOURCES})

# Add compile definition to indicate LibTorch usage
add_compile_definitions(USE_LIBTORCH)
