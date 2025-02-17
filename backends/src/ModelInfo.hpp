#pragma once
#include <string>
#include <vector>

struct LayerInfo {
    std::string name;
    std::vector<int64_t> shape;
    size_t batch_size;
};

class ModelInfo {
private:
    std::vector<LayerInfo> inputs;
    std::vector<LayerInfo> outputs;
    
public:
    void addInput(const std::string& name, const std::vector<int64_t>& shape, size_t batch_size);
    void addOutput(const std::string& name, const std::vector<int64_t>& shape, size_t batch_size);
    const std::vector<LayerInfo>& getInputs() const;
    const std::vector<LayerInfo>& getOutputs() const;
};