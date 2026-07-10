#include "CMentalWindow.h"
#include "CMentalRenderer.h"
#include "Objects/LuaScript.h"
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <thread>

namespace MentalGraphics
{

// Deleter Implementation
void Deleter::operator()(GLFWwindow* window) const noexcept {
    if (window) {
        glfwDestroyWindow(window);
        std::cout << "GLFW window destroyed.\n";
    }
}

// MentalWindow Implementation
template <typename T>
bool MentalWindow<T>::_glfwInitialized = false;

template <typename T>
void MentalWindow<T>::initializeGLFW() {
    if (!_glfwInitialized) {
        std::cout << "Initializing GLFW...\n";
        
        if (!glfwInit()) {
            std::cerr << "GLFW initialization failed!\n";
            throw std::runtime_error("Failed to initialize GLFW!");
        }
        
        _glfwInitialized = true;
        std::cout << "✓ GLFW initialized successfully.\n";
        std::cout << "  GLFW version: " << glfwGetVersionString() << "\n";
    }
}

template <typename T>
void MentalWindow<T>::createWindow() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    
    _pWindow.reset(glfwCreateWindow(800, 600, "Mental Graphics Engine", nullptr, nullptr));
    if (!_pWindow) {
        const char* description;
        int code = glfwGetError(&description);
        std::cerr << "Failed to create GLFW window!\n";
        std::cerr << "  Error code: " << code << "\n";
        std::cerr << "  Description: " << (description ? description : "Unknown error") << "\n";
        throw std::runtime_error("Failed to create GLFW window!");
    }
    std::cout << "✓ GLFW window created.\n";
}

template <typename T>
void MentalWindow<T>::setupRenderer() {
    _pMentalRenderer = std::make_unique<MentalRenderer>();
    _pMentalRenderer->Setup(_pWindow.get());
}

template <typename T>
MentalWindow<T>::MentalWindow() {
    std::cout << "Creating mental window...\n";
    
    initializeGLFW();
    createWindow();
    setupRenderer();
    
    std::cout << "✓ Mental window created successfully!\n";
}

template <typename T>
MentalWindow<T>::~MentalWindow() {
    // Order matters: destroy renderer first (it depends on GLFW window)
    _pMentalRenderer.reset();
    
    if (_glfwInitialized) {
        glfwTerminate();
        _glfwInitialized = false;
        std::cout << "GLFW terminated.\n";
    }
    std::cout << "Mental window destroyed.\n";
}

template <typename T>
GLFWwindow* MentalWindow<T>::getWindow() const {
    return _pWindow.get();
}

template <typename T>
MentalRenderer* MentalWindow<T>::getRenderer() const {
    return _pMentalRenderer.get();
}

template <typename T>
bool MentalWindow<T>::shouldClose() const {
    return _pWindow ? glfwWindowShouldClose(_pWindow.get()) : true;
}

template <typename T>
void MentalWindow<T>::setupKeyCallbacks() {
    glfwSetKeyCallback(_pWindow.get(), [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        // Только на нажатие клавиши
        if (action != GLFW_PRESS) {
            return;
        }
        
        auto& scriptManager = LuaScriptManager::getInstance();
        auto mainScript = scriptManager.getMainScript();
        
        if (!mainScript) {
            std::cout << "Main script not loaded!" << std::endl;
            return;
        }
        
        // Преобразуем GLFW key в строку
        std::string keyName;
        switch (key) {
            case GLFW_KEY_SPACE: keyName = "space"; break;
            case GLFW_KEY_ESCAPE: keyName = "escape"; break;
            case GLFW_KEY_ENTER: keyName = "enter"; break;
            case GLFW_KEY_LEFT: keyName = "left"; break;
            case GLFW_KEY_RIGHT: keyName = "right"; break;
            case GLFW_KEY_UP: keyName = "up"; break;
            case GLFW_KEY_DOWN: keyName = "down"; break;
            case GLFW_KEY_R: keyName = "r"; break;
            case GLFW_KEY_S: keyName = "s"; break;
            case GLFW_KEY_P: keyName = "p"; break;
            case GLFW_KEY_C: keyName = "c"; break;
            case GLFW_KEY_1: keyName = "1"; break;
            case GLFW_KEY_2: keyName = "2"; break;
            case GLFW_KEY_3: keyName = "3"; break;
            case GLFW_KEY_4: keyName = "4"; break;
            case GLFW_KEY_5: keyName = "5"; break;
            default: 
                // Для других клавиш используем имя
                const char* keyNamePtr = glfwGetKeyName(key, scancode);
                if (keyNamePtr) {
                    keyName = keyNamePtr;
                } else {
                    keyName = "unknown";
                }
                break;
        }
        
        // Вызываем функцию onKeyPress в main.lua
        std::cout << "Key pressed: " << keyName << std::endl;
        mainScript->callFunction("onKeyPress", keyName);
    });
}

template <typename T>
void MentalWindow<T>::mainLoop() {
    MentalRenderer* renderer = _pMentalRenderer.get();
    if (!renderer) {
        std::cerr << "ERROR: Renderer not initialized!\n";
        return;
    }
    
    std::cout << "\n=== Entering main loop ===\n";
    
    auto& scriptManager = LuaScriptManager::getInstance();
    
    // Загружаем main.lua - она создаст объекты
    if (!scriptManager.loadMainScript("Scripts/main.lua")) {
        std::cout << "Warning: main.lua not found or failed to load.\n";
    }
    
    // Получаем все объекты из менеджера
    auto cppObjects = scriptManager.getAllObjectsFromCpp();
    
    // Конвертируем shared_ptr в вектор для рендерера
    std::vector<std::shared_ptr<Any2DObject>> renderObjects;
    for (auto& obj : cppObjects) {
        if (obj) {
            renderObjects.push_back(obj);
        }
    }
    
    // Передаем объекты в рендерер
    renderer->setObjects(renderObjects);
    
    // Настройка клавиатуры
    setupKeyCallbacks();
    
    // FPS counter
    int frameCount = 0;
    auto lastTime = std::chrono::high_resolution_clock::now();
    bool framebufferResized = false;
    
    // Resize callback
    glfwSetFramebufferSizeCallback(_pWindow.get(), [](GLFWwindow* window, int width, int height) {
        auto* userData = static_cast<bool*>(glfwGetWindowUserPointer(window));
        if (userData) {
            *userData = true;
        }
    });
    glfwSetWindowUserPointer(_pWindow.get(), &framebufferResized);
    
    while (!this->shouldClose()) {
        this->pollEvents();
        
        // Handle resize
        if (framebufferResized) {
            int width, height;
            glfwGetFramebufferSize(_pWindow.get(), &width, &height);
            
            if (width > 0 && height > 0) {
                try {
                    renderer->recreateSwapChain();
                    framebufferResized = false;
                } catch (const std::exception& e) {
                    std::cerr << "Failed to recreate swapchain: " << e.what() << std::endl;
                    break;
                }
            }
        }
        
        // Обновляем Lua
        static float time = 0.0f;
        time += 0.01f;
        scriptManager.updateAll(0.01f);
        scriptManager.updateMain(0.01f);
        
        // Рендерим
        renderer->drawFrame();
        
        // FPS counter
        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime);
        
        if (elapsed >= std::chrono::seconds(1)) {
            std::string title = "Mental Graphics Engine - FPS: " + std::to_string(frameCount);
            glfwSetWindowTitle(_pWindow.get(), title.c_str());
            frameCount = 0;
            lastTime = currentTime;
        }
    }
    
    renderer->waitIdle();
    std::cout << "=== Exiting main loop ===\n";
}


template <typename T>
void MentalWindow<T>::pollEvents() const {
    glfwPollEvents();
}

// Explicit template instantiation
template class MentalWindow<GLFWwindow>;

} // namespace MentalGraphics