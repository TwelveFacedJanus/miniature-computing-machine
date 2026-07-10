-- ============================================
-- Main Global Script
-- ============================================

local objects = {}
local initialized = false
local controlFunctions = {}

function onStart()
    print("=== Main Script Started ===")
    print("=== KEYBOARD CONTROLS ===")
    print("  R - Reset all objects")
    print("  Space - Toggle pause all")
    print("  S - Toggle speed (1x/2x/3x)")
    print("  P - Print all objects info")
    print("  1 - Select rectangle")
    print("  2 - Select circle")
    print("  3 - Select triangle")
    print("  4 - Select Rect2")
    print("  Up/Down - Change selected object speed")
    print("  Left/Right - Change selected object radius/scale")
    
    setupScene()
    initialized = true
end

function onUpdate(dt)
    if not initialized then
        onStart()
        return
    end
    
    local time = getTime()
    if not time then
        return
    end
    
    -- Обновляем каждый объект
    for name, obj in pairs(objects) do
        if obj then
            local funcs = getControlFunctions(name)
            local isPaused = funcs and funcs.isPaused and funcs.isPaused() or false
            local speed = funcs and funcs.getSpeed and funcs.getSpeed() or 1.0
            
            if not isPaused then
                updateObject(name, obj, time, speed)
            end
        end
    end
end

function updateObject(name, obj, time, speed)
    if name == "Rectangle" then
        local funcs = getControlFunctions(name)
        local radius = funcs and funcs.getRadius and funcs.getRadius() or 0.3
        local angle = time * 0.5 * speed
        setPosition(obj, {
            x = radius * math.cos(angle),
            y = radius * math.sin(angle)
        })
        setRotation(obj, time * 1.5 * speed)
        
    elseif name == "Circle" then
        local funcs = getControlFunctions(name)
        local radius = funcs and funcs.getRadius and funcs.getRadius() or 0.3
        setPosition(obj, {
            x = 0.5 + radius * math.sin(time * 0.8 * speed),
            y = radius * math.sin(time * 1.6 * speed) * 0.5
        })
        
    elseif name == "Triangle" then
        setRotation(obj, time * 0.8 * speed)
        local s = 0.5 + 0.3 * math.sin(time * 1.5 * speed)
        setScale(obj, {x=s, y=s})
        setPosition(obj, {
            x = -0.5 + 0.2 * math.sin(time * 0.5 * speed),
            y = -0.5 + 0.2 * math.cos(time * 0.7 * speed)
        })
        
    elseif name == "Rect2" then
        setPosition(obj, {
            x = 0.3 + 0.2 * math.sin(time * 0.3 * speed),
            y = 0.3 + 0.2 * math.cos(time * 0.4 * speed)
        })
        setRotation(obj, time * 0.5 * speed)
    end
end

-- ============================================
-- OBJECT CREATION
-- ============================================

function createRectangle(name, x, y, width, height, color)
    print("Creating rectangle: " .. name)
    local obj = createObject("rectangle", {
        name = name,
        x = x or 0,
        y = y or 0,
        width = width or 0.4,
        height = height or 0.3,
        color = color or {r = 1, g = 0, b = 0}
    })
    
    if obj then
        registerObject(obj, name)
        setupControlFunctions(name, obj)
    end
    return obj
end

function createCircle(name, x, y, radius, color)
    print("Creating circle: " .. name)
    local obj = createObject("circle", {
        name = name,
        x = x or 0,
        y = y or 0,
        radius = radius or 0.2,
        color = color or {r = 0, g = 1, b = 0}
    })
    
    if obj then
        registerObject(obj, name)
        setupControlFunctions(name, obj)
    end
    return obj
end

function createTriangle(name, x, y, size, color)
    print("Creating triangle: " .. name)
    local obj = createObject("triangle", {
        name = name,
        x = x or 0,
        y = y or 0,
        size = size or 0.3,
        color = color or {r = 0, g = 0, b = 1}
    })
    
    if obj then
        registerObject(obj, name)
        setupControlFunctions(name, obj)
    end
    return obj
end

-- ============================================
-- OBJECT REGISTRATION
-- ============================================

function registerObject(obj, name)
    if not obj then
        print("Error: Trying to register nil object")
        return false
    end
    
    if type(obj) ~= "userdata" then
        print("Error: Invalid object type: " .. type(obj))
        return false
    end
    
    if not name or type(name) ~= "string" then
        name = "Object_" .. tostring(#objects + 1)
    end
    
    for existingName, existingObj in pairs(objects) do
        if existingObj == obj then
            print("Object already registered: " .. existingName)
            return existingName
        end
    end
    
    objects[name] = obj
    print("Registered object: " .. name)
    return name
end

function registerControlFunctions(name, funcs)
    if not name or not funcs then
        return
    end
    controlFunctions[name] = funcs
    print("Registered control functions for: " .. name)
end

function getObject(name)
    if not name or type(name) ~= "string" then
        return nil
    end
    return objects[name]
end

function getAllObjects()
    return objects
end

function getControlFunctions(name)
    if not name or type(name) ~= "string" then
        return nil
    end
    return controlFunctions[name]
end

-- ============================================
-- CONTROL FUNCTIONS
-- ============================================

function setupControlFunctions(name, obj)
    if not obj then
        return
    end
    
    -- Храним состояние в отдельной таблице, а не в userdata
    local state = {
        speed = 1.0,
        radius = 0.3,
        isPaused = false
    }
    
    -- Сохраняем состояние в глобальной таблице
    if not objStates then
        objStates = {}
    end
    objStates[name] = state
    
    local funcs = {
        setSpeed = function(speed) 
            state.speed = speed 
            print(name .. " speed: " .. speed) 
        end,
        setRadius = function(radius) 
            state.radius = radius 
            print(name .. " radius: " .. radius) 
        end,
        togglePause = function() 
            state.isPaused = not state.isPaused 
            print(name .. " " .. (state.isPaused and "paused" or "resumed")) 
        end,
        getSpeed = function() return state.speed end,
        getRadius = function() return state.radius end,
        isPaused = function() return state.isPaused end,
        resetObject = function()
            setPosition(obj, {x=0, y=0})
            setRotation(obj, 0)
            setScale(obj, {x=1, y=1})
            print(name .. " reset")
        end
    }
    
    registerControlFunctions(name, funcs)
end

-- ============================================
-- SCENE SETUP
-- ============================================

function setupScene()
    print("=== Setting up scene ===")
    
    local rect = createRectangle("Rectangle", 0, 0, 0.4, 0.3, {r=1, g=0, b=0})
    local circle = createCircle("Circle", 0.5, 0, 0.2, {r=0, g=1, b=0})
    local triangle = createTriangle("Triangle", -0.5, -0.5, 0.3, {r=0, g=0, b=1})
    local rect2 = createRectangle("Rect2", 0.3, 0.3, 0.2, 0.2, {r=1, g=1, b=0})
    
    print("Scene setup complete!")
    print("Objects created: " .. #objects)
end

-- ============================================
-- KEYBOARD HANDLING
-- ============================================

function onKeyPress(key)
    print("Key pressed: " .. key)
    
    if key == "r" then
        resetAllObjects()
    elseif key == "space" then
        togglePauseAll()
    elseif key == "s" then
        toggleSpeed()
    elseif key == "p" then
        printAllObjects()
    elseif key == "1" then
        selectObject("Rectangle")
    elseif key == "2" then
        selectObject("Circle")
    elseif key == "3" then
        selectObject("Triangle")
    elseif key == "4" then
        selectObject("Rect2")
    elseif key == "up" then
        if selectedObject then
            increaseSpeed(selectedObject)
        end
    elseif key == "down" then
        if selectedObject then
            decreaseSpeed(selectedObject)
        end
    elseif key == "right" then
        if selectedObject then
            increaseRadius(selectedObject)
        end
    elseif key == "left" then
        if selectedObject then
            decreaseRadius(selectedObject)
        end
    else
        print("Unknown key: " .. key)
    end
end

-- ============================================
-- SELECTION
-- ============================================

local selectedObject = nil
local selectedName = nil

function selectObject(name)
    print("Selecting object: " .. name)
    selectedName = name
    selectedObject = getObject(name)
    
    if selectedObject then
        print("✓ Selected: " .. name)
        pcall(function()
            setColor(selectedObject, {r=1, g=1, b=0})
        end)
    else
        print("✗ Object not found: " .. name)
        print("Available objects:")
        for n, _ in pairs(objects) do
            print("  " .. n)
        end
        selectedObject = nil
        selectedName = nil
    end
end

function getSelectedObject()
    return selectedObject, selectedName
end

-- ============================================
-- CONTROLS
-- ============================================

function increaseSpeed(obj)
    local funcs = getControlFunctions(selectedName)
    if funcs and funcs.setSpeed then
        local currentSpeed = funcs.getSpeed and funcs.getSpeed() or 1.0
        funcs.setSpeed(currentSpeed + 0.5)
        print("Speed increased to: " .. (currentSpeed + 0.5))
    else
        print("No speed control for: " .. (selectedName or "unknown"))
    end
end

function decreaseSpeed(obj)
    local funcs = getControlFunctions(selectedName)
    if funcs and funcs.setSpeed then
        local currentSpeed = funcs.getSpeed and funcs.getSpeed() or 1.0
        funcs.setSpeed(math.max(0.5, currentSpeed - 0.5))
        print("Speed decreased to: " .. math.max(0.5, currentSpeed - 0.5))
    else
        print("No speed control for: " .. (selectedName or "unknown"))
    end
end

function increaseRadius(obj)
    local funcs = getControlFunctions(selectedName)
    if funcs and funcs.setRadius then
        local currentRadius = funcs.getRadius and funcs.getRadius() or 0.3
        funcs.setRadius(currentRadius + 0.1)
        print("Radius increased to: " .. (currentRadius + 0.1))
    else
        print("No radius control for: " .. (selectedName or "unknown"))
    end
end

function decreaseRadius(obj)
    local funcs = getControlFunctions(selectedName)
    if funcs and funcs.setRadius then
        local currentRadius = funcs.getRadius and funcs.getRadius() or 0.3
        funcs.setRadius(math.max(0.1, currentRadius - 0.1))
        print("Radius decreased to: " .. math.max(0.1, currentRadius - 0.1))
    else
        print("No radius control for: " .. (selectedName or "unknown"))
    end
end

-- ============================================
-- GLOBAL CONTROLS
-- ============================================

local isPaused = false
local speedMultiplier = 1.0

function togglePauseAll()
    isPaused = not isPaused
    print("Global pause: " .. tostring(isPaused))
end

function toggleSpeed()
    if speedMultiplier == 1.0 then
        speedMultiplier = 2.0
    elseif speedMultiplier == 2.0 then
        speedMultiplier = 3.0
    else
        speedMultiplier = 1.0
    end
    print("Global speed: " .. speedMultiplier .. "x")
end

function resetAllObjects()
    for name, obj in pairs(objects) do
        if obj then
            pcall(function()
                setPosition(obj, {x = 0, y = 0})
                setRotation(obj, 0)
                setScale(obj, {x = 1, y = 1})
            end)
        end
    end
    print("All objects reset!")
end

function printAllObjects()
    print("=== All Objects ===")
    if next(objects) == nil then
        print("  No objects registered!")
        return
    end
    
    for name, obj in pairs(objects) do
        if obj then
            pcall(function()
                local pos = getPosition(obj)
                local rot = getRotation(obj)
                local scl = getScale(obj)
                print(string.format("  %s: pos=(%.2f, %.2f) rot=%.2f scale=(%.2f, %.2f)", 
                    name, pos.x, pos.y, rot, scl.x, scl.y))
            end)
        end
    end
end