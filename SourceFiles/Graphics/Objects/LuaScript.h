#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>
#ifdef __cplusplus
}
#endif

#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <iostream>

namespace MentalGraphics
{

class Any2DObject;

class LuaScript {
public:
    LuaScript();
    ~LuaScript();
    
    // Load and execute script
    bool loadScript(const std::string& filename);
    bool loadString(const std::string& script);
    
    // Call functions with different signatures
    bool callFunction(const std::string& funcName);
    bool callFunction(const std::string& funcName, float dt);
    bool callFunction(const std::string& funcName, const std::string& arg);
    bool callFunction(const std::string& funcName, Any2DObject* obj);
    bool callFunction(const std::string& funcName, Any2DObject* obj, float dt);
    bool callFunction(const std::string& funcName, Any2DObject* obj, const std::string& arg);
    bool callFunction(const std::string& funcName, float dt, const std::string& arg);
    
    // Register C++ functions for Lua
    void registerGlobalFunctions();
    
    // Get Lua state
    lua_State* getState() { return _L; }
    
    // Error handling
    std::string getLastError() const { return _lastError; }
    
    // Set/get variables
    void setGlobal(const std::string& name, int value);
    void setGlobal(const std::string& name, float value);
    void setGlobal(const std::string& name, const std::string& value);
    void setGlobal(const std::string& name, const glm::vec2& value);
    void setGlobal(const std::string& name, const glm::vec3& value);
    
    template<typename T>
    T getGlobal(const std::string& name, T defaultValue = T());
    
    // Object binding
    void bindObject(Any2DObject* obj, const std::string& name = "self");
    
    // Static instance for global callbacks
    static LuaScript* getInstance() { return _instance; }

private:
    lua_State* _L;
    std::string _lastError;
    static LuaScript* _instance;
    
    // Lua callback functions
    static int lua_print(lua_State* L);
    static int lua_getPosition(lua_State* L);
    static int lua_setPosition(lua_State* L);
    static int lua_getRotation(lua_State* L);
    static int lua_setRotation(lua_State* L);
    static int lua_getScale(lua_State* L);
    static int lua_setScale(lua_State* L);
    static int lua_getColor(lua_State* L);
    static int lua_setColor(lua_State* L);
    static int lua_getTime(lua_State* L);
    static int lua_math_lerp(lua_State* L);
    static int lua_math_clamp(lua_State* L);
    
    // Новые функции как статические методы класса
    static int lua_getAllObjects(lua_State* L);
    static int lua_selectObject(lua_State* L);
    static int lua_command(lua_State* L);
    static int lua_registerObject(lua_State* L);
    static int lua_getObject(lua_State* L);
    static int lua_createObject(lua_State* L);
};

// LuaScriptManager
class LuaScriptManager {
public:
    static LuaScriptManager& getInstance() {
        static LuaScriptManager instance;
        return instance;
    }
    
    void assignScript(Any2DObject* obj, const std::string& scriptFile);
    void updateAll(float dt);
    std::shared_ptr<LuaScript> getScript(Any2DObject* obj);
    void reloadAll();
    void registerObject(Any2DObject* obj);
    std::vector<std::shared_ptr<Any2DObject>> getAllObjectsFromCpp();
    
    
    // Main script
    bool loadMainScript(const std::string& filename = "Scripts/main.lua");
    void updateMain(float dt);
    std::shared_ptr<LuaScript> getMainScript() const { return _mainScript; }
    void registerObjectWithMain(const std::string& name, Any2DObject* obj);
    void addObject(const std::string& name, std::shared_ptr<Any2DObject> obj);
    std::shared_ptr<Any2DObject> getObjectFromCpp(const std::string& name);

private:
    LuaScriptManager() = default;
    ~LuaScriptManager() = default;
    
    struct ScriptData {
        std::shared_ptr<LuaScript> script;
        std::string filename;
        Any2DObject* object;
    };
    
    std::unordered_map<Any2DObject*, ScriptData> _scripts;
    std::shared_ptr<LuaScript> _mainScript;
    std::string _mainScriptFile = "Scripts/main.lua";
    bool _mainScriptLoaded = false;
    std::unordered_map<std::string, std::shared_ptr<Any2DObject>> _cppObjects;
};



} // namespace MentalGraphics