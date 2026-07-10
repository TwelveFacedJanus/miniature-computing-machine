#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <iostream>
#include <stdexcept>
#include "Objects/Any2DObject.h"

namespace MentalGraphics
{

struct Deleter {
    void operator()(GLFWwindow* window) const noexcept;
};

template <typename T = GLFWwindow>
class MentalWindow
{
private:
    std::unique_ptr<T, Deleter> _pWindow;
    std::unique_ptr<class MentalRenderer> _pMentalRenderer;
    static bool _glfwInitialized;
    
    void initializeGLFW();
    void createWindow();
    void setupRenderer();
    void setupKeyCallbacks();
public:
    MentalWindow();
    ~MentalWindow();
    
    MentalWindow(const MentalWindow&) = delete;
    MentalWindow(MentalWindow&&) = delete;
    MentalWindow& operator=(const MentalWindow&) = delete;
    MentalWindow& operator=(MentalWindow&&) = delete;
    
    [[nodiscard]] GLFWwindow* getWindow() const;
    [[nodiscard]] class MentalRenderer* getRenderer() const;
    [[nodiscard]] bool shouldClose() const;
    
    void mainLoop();
    void pollEvents() const;
};

} // namespace MentalGraphics