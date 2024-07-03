
# Inference Engines

## Overview

* InferenceEngines is a C++ library designed for seamless integration of various backend engines for inference tasks. 
* It supports multiple frameworks and libraries such as OpenCV DNN, TensorFlow, PyTorch (LibTorch), ONNX Runtime, TensorRT, and OpenVINO.
* The project aims to provide a unified interface for performing inference using these backends, allowing flexibility in choosing the most suitable backend based on performance or compatibility requirements.


## Dependencies

- C++17
- OpenCV
- spdlog

### Optional 
* GStreamer (1.20.3) 
* CUDA (if you want to use GPU, CUDA 12 is supported for LibTorch and TensorRT, I used CUDA 11.8 for onnx-rt)
* ONNX Runtime (1.15.1 gpu package)
* LibTorch (2.0.1-cu118)
* TensorRT (8.6.1.6)
* OpenVino (2023.2) 

## Build Instructions

### Prerequisites

Make sure you have installed the following dependencies:

- CMake >= 3.10
- OpenCV
- spdlog

### Build Steps

1. Clone the repository:

   ```bash
   git clone https://github.com/your/repository.git
   cd InferenceEngines
   ```

2. Create a build directory and navigate into it:

   ```bash
   mkdir build
   cd build
   ```

3. Configure the build with CMake:

   ```bash
   cmake ..
   ```

   Optionally, you can specify the default backend by setting `-DDEFAULT_BACKEND=your_backend` during configuration.

4. Build the project:

   ```bash
   cmake --build .
   ```

   This will compile the project along with the selected backend(s).

## Usage

To use the InferenceEngines library in your project, link against it and include necessary headers:

```cmake
target_link_libraries(your_project PRIVATE InferenceEngines)
target_include_directories(your_project PRIVATE path_to/InferenceEngines/include)
```

Ensure you have initialized and set up the selected backend(s) appropriately in your code using the provided interface headers.