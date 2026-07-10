-- Скрипт для прямоугольника
local state = {
    speed = 1.0,
    radius = 0.3,
    color = {r = 1.0, g = 0.0, b = 0.0},
    isPaused = false,
    isVisible = true
}

local myObject = nil
local myName = "Rectangle"
local controlFuncs = {}

function onStart(obj)
    print("Rectangle script started!")
    
    myObject = obj
    
    if obj then
        setColor(obj, state.color)
        setPosition(obj, {x=0.0, y=0.0})
        setScale(obj, {x=1.0, y=1.0})
        
        -- Регистрируем объект с именем
        local registeredName = registerObject(obj, myName)
        if registeredName then
            print("Registered with name: " .. registeredName)
        else
            print("Failed to register object!")
        end
        
        -- Создаем функции управления
        controlFuncs = {
            setSpeed = function(speed) 
                state.speed = speed 
                print("Rectangle speed: " .. speed) 
            end,
            setRadius = function(radius) 
                state.radius = radius 
                print("Rectangle radius: " .. radius) 
            end,
            togglePause = function() 
                state.isPaused = not state.isPaused 
                print("Rectangle " .. (state.isPaused and "paused" or "resumed")) 
            end,
            resetObject = function()
                if myObject then
                    setPosition(myObject, {x=0, y=0})
                    setRotation(myObject, 0)
                    setScale(myObject, {x=1, y=1})
                    print("Rectangle reset")
                end
            end,
            getSpeed = function() return state.speed end,
            getRadius = function() return state.radius end,
            isPaused = function() return state.isPaused end
        }
        
        -- Регистрируем функции управления
        if registerControlFunctions then
            registerControlFunctions("Rectangle", controlFuncs)
        end
    end
end

function onUpdate(obj, dt)
    if not obj then
        return
    end
    
    myObject = obj
    
    if state.isPaused then
        return
    end
    
    local time = getTime()
    if not time then
        return
    end
    
    local angle = time * 0.5 * state.speed
    setPosition(obj, {
        x = state.radius * math.cos(angle),
        y = state.radius * math.sin(angle)
    })
    
    setRotation(obj, time * 1.5 * state.speed)
    
    state.color = {
        r = 0.5 + 0.5 * math.sin(time * 0.7),
        g = 0.5 + 0.5 * math.sin(time * 0.9 + 1),
        b = 0.5 + 0.5 * math.sin(time * 1.1 + 2)
    }
    setColor(obj, state.color)
end