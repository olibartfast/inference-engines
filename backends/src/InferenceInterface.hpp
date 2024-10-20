#pragma once
#include "common.hpp"
#include <variant>
using TensorElement = std::variant<float, int32_t, int64_t>;

class InferenceInterface{
    	
    public:
        InferenceInterface(const std::string& weights, const std::string& modelConfiguration,
         bool use_gpu = false)
        {

        }

        
        virtual std::tuple<std::vector<std::vector<TensorElement>>, std::vector<std::vector<int64_t>>> get_infer_results(const cv::Mat& input_blob) = 0;

    protected:
        std::vector<float> blob2vec(const cv::Mat& input_blob);

};