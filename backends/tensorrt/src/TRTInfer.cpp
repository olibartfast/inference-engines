#include "TRTInfer.hpp"

TRTInfer::TRTInfer(const std::string& model_path) : InferenceInterface{model_path, "", true}
{

    LOG(INFO) << "Initializing TensorRT for model {}" << model_path;
    initializeBuffers(model_path);
}

void TRTInfer::initializeBuffers(const std::string& engine_path)
{


    // Create TensorRT runtime and deserialize engine
    Logger logger;
    // Create TensorRT runtime
    runtime_ = nvinfer1::createInferRuntime(logger);

    // Load engine file
    std::ifstream engine_file(engine_path, std::ios::binary);
    if (!engine_file)
    {
        throw std::runtime_error("Failed to open engine file: " + engine_path);
    }
    engine_file.seekg(0, std::ios::end);
    size_t file_size = engine_file.tellg();
    engine_file.seekg(0, std::ios::beg);
    std::vector<char> engine_data(file_size);
    engine_file.read(engine_data.data(), file_size);
    engine_file.close();

    // Deserialize engine
     engine_.reset(
        runtime_->deserializeCudaEngine(engine_data.data(), file_size),
        [](nvinfer1::ICudaEngine* engine) { engine->destroy(); });


    // Create execution context and allocate input/output buffers
    createContextAndAllocateBuffers();
}

// calculate size of tensor
size_t TRTInfer::getSizeByDim(const nvinfer1::Dims& dims)
{
    size_t size = 1;
    for (size_t i = 0; i < dims.nbDims; ++i)
    {
        if(dims.d[i] == -1 || dims.d[i] == 0)
        {
            continue;
        }
        size *= dims.d[i];
    }
    return size;
}



void TRTInfer::createContextAndAllocateBuffers()
{
    nvinfer1::Dims profile_dims = engine_->getProfileDimensions(0, 0, nvinfer1::OptProfileSelector::kMIN);
    int max_batch_size = profile_dims.d[0];
    context_ = engine_->createExecutionContext();
    context_->setBindingDimensions(0, profile_dims);
    buffers_.resize(engine_->getNbBindings());
    for (int i = 0; i < engine_->getNbBindings(); ++i)
    {
        nvinfer1::Dims dims = engine_->getBindingDimensions(i);
        auto size = getSizeByDim(dims);
        size_t binding_size;
        switch (engine_->getBindingDataType(i)) 
        {
            case nvinfer1::DataType::kFLOAT:
                binding_size = size * sizeof(float);
                break;
            case nvinfer1::DataType::kINT32:
                binding_size = size * sizeof(int);
                break;
            // Add more cases for other data types if needed
            default:
                // Handle unsupported data types
                std::exit(1);
        }
        cudaMalloc(&buffers_[i], binding_size);
   
        if (engine_->bindingIsInput(i))
        {
            LOG(INFO) << "Input layer {} {}" << num_inputs_ << engine_->getBindingName(i);
            num_inputs_++;
            continue;
        }
        LOG(INFO) << "Output layer {} {}" << num_outputs_ << engine_->getBindingName(i);
        num_outputs_++;
    }
}

std::tuple<std::vector<std::vector<TensorElement>>, std::vector<std::vector<int64_t>>> TRTInfer::get_infer_results(const cv::Mat& preprocessed_img)
{
    // Copy input data to the GPU buffers
    // Convert the input image to a blob swapping channels order from hwc to chw    
    cv::Mat blob;
    cv::dnn::blobFromImage(preprocessed_img, blob, 1.0, cv::Size(), cv::Scalar(), false, false);

    for (size_t i = 0; i < num_inputs_; ++i)
    {
        nvinfer1::Dims dims = engine_->getBindingDimensions(i);
        size_t size = getSizeByDim(dims);  // Calculate tensor size
        size_t binding_size = 0;

        // Calculate buffer size based on data type
        switch (engine_->getBindingDataType(i))
        {
            case nvinfer1::DataType::kFLOAT:
                binding_size = size * sizeof(float);
                break;
            case nvinfer1::DataType::kINT32:
                binding_size = size * sizeof(int32_t);
                break;
            default:
                // Handle unsupported data types
                std::cerr << "Unsupported input data type!\n";
                std::exit(1);
                break;
        }

        // Copy data to the appropriate GPU buffer
        switch(i)
        {
            case 0:
                cudaMemcpy(buffers_[0], blob.data, binding_size, cudaMemcpyHostToDevice);
                break;
            case 1:
                // If there's a second input, e.g., for target sizes in RT-DETR model
                std::vector<int32_t> orig_target_sizes = { static_cast<int32_t>(blob.size[2]), static_cast<int32_t>(blob.size[3]) };
                cudaMemcpy(buffers_[1], orig_target_sizes.data(), binding_size, cudaMemcpyHostToDevice);
                break;
        }
    }

    // Perform inference
    if (!context_->enqueueV2(buffers_.data(), 0, nullptr)) 
    {
        std::cerr << "Inference failed!\n";
        std::exit(1);
    }

    // Extract outputs and their shapes
    std::vector<std::vector<int64_t>> output_shapes;
    std::vector<std::vector<TensorElement>> outputs;
    
    for (size_t i = 0; i < num_outputs_; ++i)
    {
        // Get output dimensions
        nvinfer1::Dims dims = engine_->getBindingDimensions(i + num_inputs_);
        auto num_elements = getSizeByDim(dims);
        
        // Prepare tensor_data to store the output
        std::vector<TensorElement> tensor_data;
        
        // Copy the output data based on its data type
        switch (engine_->getBindingDataType(i + num_inputs_))
        {
            case nvinfer1::DataType::kFLOAT:
            {
                std::vector<float> output_data_float(num_elements);
                cudaMemcpy(output_data_float.data(), buffers_[i + num_inputs_],  num_elements * sizeof(float), cudaMemcpyDeviceToHost);
                
                // Wrap the output data into TensorElement (std::variant)
                for (const auto& value : output_data_float) {
                    tensor_data.push_back(static_cast<float>(value));
                }
                break;
            }
            case nvinfer1::DataType::kINT32:
            {
                std::vector<int32_t> output_data_int(num_elements);
                cudaMemcpy(output_data_int.data(), buffers_[i + num_inputs_],  num_elements * sizeof(int32_t), cudaMemcpyDeviceToHost);
                
                // Wrap the output data into TensorElement
                for (const auto& value : output_data_int) {
                    tensor_data.push_back(static_cast<int32_t>(value));
                }
                break;
            }
            // Add more cases for other data types if needed
            default:
                std::cerr << "Unsupported output data type!\n";
                std::exit(1);
                break;
        }
        
        // Store the output tensor and its shape
        outputs.emplace_back(std::move(tensor_data));
        
        const int64_t curr_batch = dims.d[0] == -1 ? 1 : dims.d[0];  // Handle dynamic batch size
        const auto out_shape = std::vector<int64_t>{curr_batch, dims.d[1], dims.d[2], dims.d[3]};
        output_shapes.emplace_back(out_shape);
    }

    return std::make_tuple(std::move(outputs), std::move(output_shapes));
}
