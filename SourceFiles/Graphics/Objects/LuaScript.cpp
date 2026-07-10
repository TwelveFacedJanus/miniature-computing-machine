#include "LuaScript.h"
#include "Any2DObject.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <string>

namespace MentalGraphics
{

// Static instance
LuaScript* LuaScript::_instance = nullptr;

// ============================================
// LuaScript Implementation
// ============================================

LuaScript::LuaScript() : _L(nullptr) {
    _L = luaL_newstate();
    if (_L) {
        luaL_openlibs(_L);
        registerGlobalFunctions();
        _instance = this;
        
        // Add script path for require
        lua_getglobal(_L, "package");
        lua_getfield(_L, -1, "path");
        std::string currentPath = lua_tostring(_L, -1);
        lua_pop(_L, 1);
        
        std::string newPath = currentPath + ";./Scripts/?.lua;./Scripts/?/init.lua";
        lua_pushstring(_L, newPath.c_str());
        lua_setfield(_L, -2, "path");
        lua_pop(_L, 1);
    }
}

LuaScript::~LuaScript() {
    if (_L) {
        lua_close(_L);
        _L = nullptr;
    }
}

bool LuaScript::loadScript(const std::string& filename) {
    if (!_L) return false;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        _lastError = "Failed to open script: " + filename;
        std::cerr << _lastError << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return loadString(buffer.str());
}

bool LuaScript::loadString(const std::string& script) {
    if (!_L) return false;
    
    if (luaL_loadstring(_L, script.c_str()) != LUA_OK) {
        _lastError = lua_tostring(_L, -1);
        lua_pop(_L, 1);
        std::cerr << "Lua load error: " << _lastError << std::endl;
        return false;
    }
    
    if (lua_pcall(_L, 0, LUA_OK, 0) != LUA_OK) {
        _lastError = lua_tostring(_L, -1);
        lua_pop(_L, 1);
        std::cerr << "Lua execution error: " << _lastError << std::endl;
        return false;
    }
    
    return true;
}

// ============================================
// callFunction Implementations
// ============================================

bool LuaScript::callFunction(const std::string& funcName) {
    return callFunction(funcName, nullptr, 0.0f);
}

bool LuaScript::callFunction(const std::string& funcName, float dt) {
    return callFunction(funcName, nullptr, dt);
}

bool LuaScript::callFunction(const std::string& funcName, Any2DObject* obj, float dt) {
    if (!_L) return false;
    
    lua_getglobal(_L, funcName.c_str());
    
    if (!lua_isfunction(_L, -1)) {
        lua_pop(_L, 1);
        return true;
    }
    
    int argCount = 0;
    
    if (obj) {
        lua_pushlightuserdata(_L, obj);
        argCount++;
    }
    
    lua_pushnumber(_L, dt);
    argCount++;
    
    if (lua_pcall(_L, argCount, 0, 0) != LUA_OK) {
        _lastError = lua_tostring(_L, -1);
        lua_pop(_L, 1);
        std::cerr << "Lua call error (" << funcName << "): " << _lastError << std::endl;
        return false;
    }
    
    return true;
}

bool LuaScript::callFunction(const std::string& funcName, const std::string& arg) {
    if (!_L) return false;
    
    lua_getglobal(_L, funcName.c_str());
    if (!lua_isfunction(_L, -1)) {
        lua_pop(_L, 1);
        return true;
    }
    
    lua_pushstring(_L, arg.c_str());
    
    if (lua_pcall(_L, 1, 0, 0) != LUA_OK) {
        _lastError = lua_tostring(_L, -1);
        lua_pop(_L, 1);
        std::cerr << "Lua call error (" << funcName << "): " << _lastError << std::endl;
        return false;
    }
    
    return true;
}

bool LuaScript::callFunction(const std::string& funcName, Any2DObject* obj) {
    return callFunction(funcName, obj, 0.0f);
}

bool LuaScript::callFunction(const std::string& funcName, Any2DObject* obj, const std::string& arg) {
    if (!_L) return false;
    
    lua_getglobal(_L, funcName.c_str());
    if (!lua_isfunction(_L, -1)) {
        lua_pop(_L, 1);
        return true;
    }
    
    int argCount = 0;
    if (obj) {
        lua_pushlightuserdata(_L, obj);
        argCount++;
    }
    
    if (!arg.empty()) {
        lua_pushstring(_L, arg.c_str());
        argCount++;
    }
    
    if (lua_pcall(_L, argCount, 0, 0) != LUA_OK) {
        _lastError = lua_tostring(_L, -1);
        lua_pop(_L, 1);
        std::cerr << "Lua call error (" << funcName << "): " << _lastError << std::endl;
        return false;
    }
    
    return true;
}

bool LuaScript::callFunction(const std::string& funcName, float dt, const std::string& arg) {
    if (!_L) return false;
    
    lua_getglobal(_L, funcName.c_str());
    if (!lua_isfunction(_L, -1)) {
        lua_pop(_L, 1);
        return true;
    }
    
    int argCount = 0;
    lua_pushnumber(_L, dt);
    argCount++;
    
    if (!arg.empty()) {
        lua_pushstring(_L, arg.c_str());
        argCount++;
    }
    
    if (lua_pcall(_L, argCount, 0, 0) != LUA_OK) {
        _lastError = lua_tostring(_L, -1);
        lua_pop(_L, 1);
        std::cerr << "Lua call error (" << funcName << "): " << _lastError << std::endl;
        return false;
    }
    
    return true;
}

// ============================================
// Lua Callback Functions
// ============================================

int LuaScript::lua_print(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (i > 1) std::cout << " ";
        const char* str = lua_tostring(L, i);
        if (str) std::cout << str;
        else std::cout << "[nil]";
    }
    std::cout << std::endl;
    return 0;
}

int LuaScript::lua_getPosition(lua_State* L) {
    Any2DObject* obj = (Any2DObject*)lua_touserdata(L, 1);
    if (!obj) {
        lua_pushnil(L);
        return 1;
    }
    
    lua_newtable(L);
    lua_pushnumber(L, obj->position.x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, obj->position.y);
    lua_setfield(L, -2, "y");
    return 1;
}

int LuaScript::lua_setPosition(lua_State* L) {
    Any2DObject* obj = (Any2DObject*)lua_touserdata(L, 1);
    if (!obj) return 0;
    
    lua_getfield(L, 2, "x");
    lua_getfield(L, 2, "y");
    
    float x = lua_tonumber(L, -2);
    float y = lua_tonumber(L, -1);
    lua_pop(L, 2);
    
    obj->position = glm::vec2(x, y);
    return 0;
}

int LuaScript::lua_getRotation(lua_State* L) {
    Any2DObject* obj = (Any2DObject*)lua_touserdata(L, 1);
    if (!obj) {
        lua_pushnil(L);
        return 1;
    }
    
    lua_pushnumber(L, obj->rotation);
    return 1;
}

int LuaScript::lua_setRotation(lua_State* L) {
    Any2DObject* obj = (Any2DObject*)lua_touserdata(L, 1);
    if (!obj) return 0;
    
    float rot = lua_tonumber(L, 2);
    obj->rotation = rot;
    return 0;
}

int LuaScript::lua_getScale(lua_State* L) {
    Any2DObject* obj = (Any2DObject*)lua_touserdata(L, 1);
    if (!obj) {
        lua_pushnil(L);
        return 1;
    }
    
    lua_newtable(L);
    lua_pushnumber(L, obj->scale.x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, obj->scale.y);
    lua_setfield(L, -2, "y");
    return 1;
}

int LuaScript::lua_setScale(lua_State* L) {
    Any2DObject* obj = (Any2DObject*)lua_touserdata(L, 1);
    if (!obj) return 0;
    
    lua_getfield(L, 2, "x");
    lua_getfield(L, 2, "y");
    
    float x = lua_tonumber(L, -2);
    float y = lua_tonumber(L, -1);
    lua_pop(L, 2);
    
    obj->scale = glm::vec2(x, y);
    return 0;
}

int LuaScript::lua_getColor(lua_State* L) {
    Any2DObject* obj = (Any2DObject*)lua_touserdata(L, 1);
    if (!obj) {
        lua_pushnil(L);
        return 1;
    }
    
    lua_newtable(L);
    lua_pushnumber(L, obj->color.r);
    lua_setfield(L, -2, "r");
    lua_pushnumber(L, obj->color.g);
    lua_setfield(L, -2, "g");
    lua_pushnumber(L, obj->color.b);
    lua_setfield(L, -2, "b");
    return 1;
}

int LuaScript::lua_setColor(lua_State* L) {
    Any2DObject* obj = (Any2DObject*)lua_touserdata(L, 1);
    if (!obj) return 0;
    
    lua_getfield(L, 2, "r");
    lua_getfield(L, 2, "g");
    lua_getfield(L, 2, "b");
    
    float r = lua_tonumber(L, -3);
    float g = lua_tonumber(L, -2);
    float b = lua_tonumber(L, -1);
    lua_pop(L, 3);
    
    obj->color = glm::vec3(r, g, b);
    return 0;
}

int LuaScript::lua_getTime(lua_State* L) {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float elapsed = std::chrono::duration<float>(currentTime - startTime).count();
    lua_pushnumber(L, elapsed);
    return 1;
}

int LuaScript::lua_math_lerp(lua_State* L) {
    float a = lua_tonumber(L, 1);
    float b = lua_tonumber(L, 2);
    float t = lua_tonumber(L, 3);
    lua_pushnumber(L, a + (b - a) * t);
    return 1;
}

int LuaScript::lua_math_clamp(lua_State* L) {
    float value = lua_tonumber(L, 1);
    float min = lua_tonumber(L, 2);
    float max = lua_tonumber(L, 3);
    lua_pushnumber(L, std::max(min, std::min(max, value)));
    return 1;
}

// ============================================
// Статические методы класса LuaScript
// ============================================

int LuaScript::lua_command(lua_State* L) {
    const char* cmd = lua_tostring(L, 1);
    if (!cmd) {
        return 0;
    }
    
    std::string command(cmd);
    std::cout << "Executing command: " << command << std::endl;
    
    if (command == "pause") {
        lua_getglobal(L, "togglePauseAll");
        if (lua_isfunction(L, -1)) {
            lua_pcall(L, 0, 0, 0);
        }
    } else if (command == "reset") {
        lua_getglobal(L, "resetAllObjects");
        if (lua_isfunction(L, -1)) {
            lua_pcall(L, 0, 0, 0);
        }
    } else if (command == "print") {
        lua_getglobal(L, "printAllObjects");
        if (lua_isfunction(L, -1)) {
            lua_pcall(L, 0, 0, 0);
        }
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }
    
    return 0;
}

int LuaScript::lua_selectObject(lua_State* L) {
    const char* name = lua_tostring(L, 1);
    if (!name) {
        lua_pushnil(L);
        return 1;
    }
    
    lua_getglobal(L, "selectObject");
    if (lua_isfunction(L, -1)) {
        lua_pushstring(L, name);
        lua_pcall(L, 1, 0, 0);
    }
    
    return 0;
}

int LuaScript::lua_getAllObjects(lua_State* L) {
    lua_getglobal(L, "getAllObjects");
    if (lua_isfunction(L, -1)) {
        lua_pcall(L, 0, 1, 0);
        return 1;
    }
    
    lua_newtable(L);
    return 1;
}

int LuaScript::lua_registerObject(lua_State* L) {
    Any2DObject* obj = (Any2DObject*)lua_touserdata(L, 1);
    if (!obj) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    lua_getglobal(L, "registerObject");
    if (lua_isfunction(L, -1)) {
        lua_pushlightuserdata(L, obj);
        lua_pcall(L, 1, 0, 0);
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

int LuaScript::lua_getObject(lua_State* L) {
    const char* name = lua_tostring(L, 1);
    if (!name) {
        lua_pushnil(L);
        return 1;
    }
    
    lua_getglobal(L, "getObject");
    if (lua_isfunction(L, -1)) {
        lua_pushstring(L, name);
        lua_pcall(L, 1, 1, 0);
        return 1;
    }
    
    lua_pushnil(L);
    return 1;
}

// ============================================
// createObject - создает объекты из Lua
// ============================================

int LuaScript::lua_createObject(lua_State* L) {
    const char* type = lua_tostring(L, 1);
    if (!type) {
        lua_pushnil(L);
        return 1;
    }
    
    // Получаем параметры из таблицы
    lua_getfield(L, 2, "name");
    lua_getfield(L, 2, "x");
    lua_getfield(L, 2, "y");
    lua_getfield(L, 2, "width");
    lua_getfield(L, 2, "height");
    lua_getfield(L, 2, "radius");
    lua_getfield(L, 2, "size");
    lua_getfield(L, 2, "color");
    
    const char* name = lua_tostring(L, -8);
    float x = lua_tonumber(L, -7);
    float y = lua_tonumber(L, -6);
    float width = lua_tonumber(L, -5);
    float height = lua_tonumber(L, -4);
    float radius = lua_tonumber(L, -3);
    float size = lua_tonumber(L, -2);
    
    // Получаем цвет
    glm::vec3 color(1.0f, 1.0f, 1.0f);
    if (lua_istable(L, -1)) {
        lua_getfield(L, -1, "r");
        lua_getfield(L, -2, "g");
        lua_getfield(L, -3, "b");
        float r = lua_tonumber(L, -3);
        float g = lua_tonumber(L, -2);
        float b = lua_tonumber(L, -1);
        color = glm::vec3(r, g, b);
        lua_pop(L, 3);
    }
    
    lua_pop(L, 8);
    
    std::cout << "Creating object of type: " << type << " name: " << (name ? name : "unnamed") << std::endl;
    
    // Создаем объект
    std::shared_ptr<Any2DObject> obj;
    
    if (strcmp(type, "rectangle") == 0) {
        obj = Any2DObjectFactory::createRectangle(
            glm::vec2(width > 0 ? width : 0.4f, height > 0 ? height : 0.3f),
            color,
            glm::vec2(x, y)
        );
    } else if (strcmp(type, "circle") == 0) {
        obj = Any2DObjectFactory::createCircle(
            radius > 0 ? radius : 0.2f,
            32,
            color,
            glm::vec2(x, y)
        );
    } else if (strcmp(type, "triangle") == 0) {
        float s = size > 0 ? size : 0.3f;
        obj = Any2DObjectFactory::createTriangle(
            glm::vec2(0.0f, -s),
            glm::vec2(s, s),
            glm::vec2(-s, s),
            color,
            glm::vec2(x, y)
        );
    } else {
        std::cerr << "Unknown object type: " << type << std::endl;
        lua_pushnil(L);
        return 1;
    }
    
    if (!obj) {
        std::cerr << "Failed to create object of type: " << type << std::endl;
        lua_pushnil(L);
        return 1;
    }
    
    if (name) {
        obj->name = name;
    }
    
    // Сохраняем объект в менеджере
    auto& manager = LuaScriptManager::getInstance();
    manager.addObject(name ? name : "unnamed", obj);
    
    // Возвращаем указатель на объект в Lua
    lua_pushlightuserdata(L, obj.get());
    
    return 1;
}

// ============================================
// registerGlobalFunctions
// ============================================

void LuaScript::registerGlobalFunctions() {
    if (!_L) return;
    
    // Register C++ functions for Lua
    lua_register(_L, "print", lua_print);
    lua_register(_L, "getPosition", lua_getPosition);
    lua_register(_L, "setPosition", lua_setPosition);
    lua_register(_L, "getRotation", lua_getRotation);
    lua_register(_L, "setRotation", lua_setRotation);
    lua_register(_L, "getScale", lua_getScale);
    lua_register(_L, "setScale", lua_setScale);
    lua_register(_L, "getColor", lua_getColor);
    lua_register(_L, "setColor", lua_setColor);
    lua_register(_L, "getTime", lua_getTime);
    lua_register(_L, "lerp", lua_math_lerp);
    lua_register(_L, "clamp", lua_math_clamp);
    
    // Новые функции
    lua_register(_L, "getAllObjects", lua_getAllObjects);
    lua_register(_L, "selectObject", lua_selectObject);
    lua_register(_L, "command", lua_command);
    lua_register(_L, "registerObject", lua_registerObject);
    lua_register(_L, "getObject", lua_getObject);
    lua_register(_L, "createObject", lua_createObject);
}

// ============================================
// Set/Get Global
// ============================================

void LuaScript::setGlobal(const std::string& name, int value) {
    if (!_L) return;
    lua_pushinteger(_L, value);
    lua_setglobal(_L, name.c_str());
}

void LuaScript::setGlobal(const std::string& name, float value) {
    if (!_L) return;
    lua_pushnumber(_L, value);
    lua_setglobal(_L, name.c_str());
}

void LuaScript::setGlobal(const std::string& name, const std::string& value) {
    if (!_L) return;
    lua_pushstring(_L, value.c_str());
    lua_setglobal(_L, name.c_str());
}

void LuaScript::setGlobal(const std::string& name, const glm::vec2& value) {
    if (!_L) return;
    lua_newtable(_L);
    lua_pushnumber(_L, value.x);
    lua_setfield(_L, -2, "x");
    lua_pushnumber(_L, value.y);
    lua_setfield(_L, -2, "y");
    lua_setglobal(_L, name.c_str());
}

void LuaScript::setGlobal(const std::string& name, const glm::vec3& value) {
    if (!_L) return;
    lua_newtable(_L);
    lua_pushnumber(_L, value.x);
    lua_setfield(_L, -2, "x");
    lua_pushnumber(_L, value.y);
    lua_setfield(_L, -2, "y");
    lua_pushnumber(_L, value.z);
    lua_setfield(_L, -2, "z");
    lua_setglobal(_L, name.c_str());
}

void LuaScript::bindObject(Any2DObject* obj, const std::string& name) {
    if (!_L || !obj) return;
    lua_pushlightuserdata(_L, obj);
    lua_setglobal(_L, name.c_str());
}

// ============================================
// LuaScriptManager Implementation
// ============================================

void LuaScriptManager::addObject(const std::string& name, std::shared_ptr<Any2DObject> obj) {
    _cppObjects[name] = obj;
    std::cout << "✓ Object stored in C++ manager: " << name << std::endl;
}

std::shared_ptr<Any2DObject> LuaScriptManager::getObjectFromCpp(const std::string& name) {
    auto it = _cppObjects.find(name);
    if (it != _cppObjects.end()) {
        return it->second;
    }
    return nullptr;
}

bool LuaScriptManager::loadMainScript(const std::string& filename) {
    _mainScriptFile = filename;
    _mainScript = std::make_shared<LuaScript>();
    
    if (!_mainScript->loadScript(filename)) {
        std::cerr << "Failed to load main script: " << filename << std::endl;
        _mainScriptLoaded = false;
        return false;
    }
    
    _mainScript->callFunction("onStart");
    
    _mainScriptLoaded = true;
    std::cout << "✓ Main script loaded: " << filename << std::endl;
    return true;
}

void LuaScriptManager::updateMain(float dt) {
    if (_mainScriptLoaded && _mainScript) {
        _mainScript->callFunction("onUpdate", dt);
    }
}

void LuaScriptManager::registerObjectWithMain(const std::string& name, Any2DObject* obj) {
    if (_mainScriptLoaded && _mainScript) {
        _mainScript->bindObject(obj, name);
        _mainScript->callFunction("registerObject", obj, 0.0f);
    }
}

void LuaScriptManager::assignScript(Any2DObject* obj, const std::string& scriptFile) {
    if (!obj) return;
    
    ScriptData data;
    data.script = std::make_shared<LuaScript>();
    data.filename = scriptFile;
    data.object = obj;
    
    if (!data.script->loadScript(scriptFile)) {
        std::cerr << "Failed to load script for object: " << scriptFile << std::endl;
        return;
    }
    
    data.script->bindObject(obj);
    data.script->callFunction("onStart", obj, 0.0f);
    
    _scripts[obj] = data;
    std::cout << "✓ Lua script assigned: " << scriptFile << std::endl;
}

void LuaScriptManager::updateAll(float dt) {
    for (auto& pair : _scripts) {
        auto& data = pair.second;
        if (data.script) {
            data.script->callFunction("onUpdate", data.object, dt);
        }
    }
}
std::vector<std::shared_ptr<Any2DObject>> LuaScriptManager::getAllObjectsFromCpp() {
    std::vector<std::shared_ptr<Any2DObject>> result;
    for (auto& pair : _cppObjects) {
        if (pair.second) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::shared_ptr<LuaScript> LuaScriptManager::getScript(Any2DObject* obj) {
    auto it = _scripts.find(obj);
    if (it != _scripts.end()) {
        return it->second.script;
    }
    return nullptr;
}

void LuaScriptManager::reloadAll() {
    for (auto& pair : _scripts) {
        auto& data = pair.second;
        if (data.script) {
            data.script->loadScript(data.filename);
        }
    }
}

void LuaScriptManager::registerObject(Any2DObject* obj) {
    if (_scripts.find(obj) == _scripts.end()) {
        ScriptData data;
        data.object = obj;
        _scripts[obj] = data;
    }
}

} // namespace MentalGraphics