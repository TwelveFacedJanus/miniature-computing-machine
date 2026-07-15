#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdexcept>

class MentalShaderMaterial
{
private:
	std::vector<char> readFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open()) throw std::runtime_error("Failed to open file: " + filename);

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}

	VkShaderModule createShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		VkShaderModule shaderModule;

		if (vkCreateShaderModule(this->ld, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module!");
		}
		
		return shaderModule;
	}
	VkDevice ld;
public:
	VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
	VkPipelineShaderStageCreateInfo fragShaderStageInfo {};

	VkShaderModule vertShaderModule, fragShaderModule;
	VkPipelineShaderStageCreateInfo shaderStages[2];

	VkPipelineShaderStageCreateInfo* getStages() { return shaderStages; }
	MentalShaderMaterial(const std::string& vertex_path, const std::string& fragment_path, const std::string& name, VkDevice ld) : ld(ld)
	{
		auto vert_code = this->readFile(vertex_path);
		auto frag_code = this->readFile(fragment_path);

		this->vertShaderModule = this->createShaderModule(vert_code);
		this->fragShaderModule = this->createShaderModule(frag_code);

		this->vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		this->vertShaderStageInfo.stage= VK_SHADER_STAGE_VERTEX_BIT;
		this->vertShaderStageInfo.module = this->vertShaderModule;
		this->vertShaderStageInfo.pName = "main";
		
		this->fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		this->fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		this->fragShaderStageInfo.module = this->fragShaderModule;
		this->fragShaderStageInfo.pName = "main";
	}
	~MentalShaderMaterial() {
		vkDestroyShaderModule(this->ld, this->vertShaderModule, nullptr);
		vkDestroyShaderModule(this->ld, this->fragShaderModule, nullptr);
	};
};
