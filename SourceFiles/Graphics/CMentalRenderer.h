#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <vector>
#include <optional>
#include <set>
#include <fstream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>
#include "Objects/Any2DObject.h"




namespace MentalGraphics
{

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    // Filter messages based on severity
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        std::cerr << "[VULKAN ERROR] ";
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::cerr << "[VULKAN WARNING] ";
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        std::cerr << "[VULKAN INFO] ";
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        std::cerr << "[VULKAN VERBOSE] ";
    }

    // Optionally filter by message type
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        std::cerr << "[General] ";
    } else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        std::cerr << "[Validation] ";
    } else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        std::cerr << "[Performance] ";
    }

    // Print the message
    std::cerr << pCallbackData->pMessage << std::endl;

    // Return VK_FALSE to indicate the application should not abort
    return VK_FALSE;
}

// Forward declarations
struct QueueFamilyIndices;
struct SwapChainSupportDetails;
class MentalRenderer;

// Constants - declared as extern
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};
const bool enableValidationLayers = true;

// Debug callback
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    
    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
    
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }
    
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        
        return attributeDescriptions;
    }
};

class MentalRenderer
{
private:
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    GLFWwindow* window = nullptr;
    
    // Swapchain members
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    
    // Render pass and pipeline
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline graphicsPipeline = VK_NULL_HANDLE;
    
    // Framebuffers
    std::vector<VkFramebuffer> swapChainFramebuffers;
    
    // Command pool and buffers
    VkCommandPool commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers;
    
    // Synchronization
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    size_t currentFrame = 0;
    const int MAX_FRAMES_IN_FLIGHT = 2;
    
    // Vertex buffer
    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
    
    // Private methods
    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSurface();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createVertexBuffer();
    void createCommandBuffers();
    void createSyncObjects();
    void updateViewport();
    std::vector<std::shared_ptr<Any2DObject>> _objects;
    VkBuffer _objectVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory _objectVertexBufferMemory = VK_NULL_HANDLE;
    size_t _vertexCount = 0;
    bool _objectBufferDirty = true;
    
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    
    [[deprecated("Now MentalShaderMaterial have this private function implementation")]]
    VkShaderModule createShaderModule(const std::vector<char>& code);

    [[deprecated("Now each shader module have private function implementation")]]
    std::vector<char> readFile(const std::string& filename);
    
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, 
                      VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    
    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger);
    
    void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator);
    
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();

public:
    MentalRenderer();
    explicit MentalRenderer(GLFWwindow* window);
    ~MentalRenderer();


    void markObjectBufferDirty() { _objectBufferDirty = true; }
    void setObjects(const std::vector<std::shared_ptr<Any2DObject>>& objects);
    void updateVertexBuffer();
    
    void CreateVKInstance();
    void Setup(GLFWwindow* window);
    void cleanup();
    void drawFrame();
    void recreateSwapChain();
    void waitIdle();

    // Getters
    VkInstance getInstance() const;
    VkPhysicalDevice getPhysicalDevice() const;
    VkDevice getLogicalDevice() const;
    VkQueue getGraphicsQueue() const;
    VkQueue getPresentQueue() const;
    VkSurfaceKHR getSurface() const;
    VkSwapchainKHR getSwapChain() const;
    VkFormat getSwapChainImageFormat() const;
    VkExtent2D getSwapChainExtent() const;
    const std::vector<VkImageView>& getSwapChainImageViews() const;
};

} // namespace MentalGraphics
