-- Скрипт для треугольника
local state = {
    speed = 1.0,
    scale = 0.5,
    isPaused = false
}

local myObject = nil
local myName = "Triangle"
local controlFuncs = {}

function onStart(obj)
    print("Triangle script started!")
    
    myObject = obj
    
    if obj then
        setColor(obj, {r=0.0, g=0.0, b=1.0})
        setPosition(obj, {x=-0.5, y=-0.5})
        setScale(obj, {x=0.5, y=0.5})
        
        local registeredName = registerObject(obj, myName)
        print("Registered with name: " .. (registeredName or "nil"))
        
        controlFuncs = {
            setSpeed = function(speed) 
                state.speed = speed 
                print("Triangle speed: " .. speed) 
            end,
            setScale = function(scale) 
                state.scale = scale 
                print("Triangle scale: " .. scale) 
            end,
            togglePause = function() 
                state.isPaused = not state.isPaused 
                print("Triangle " .. (state.isPaused and "paused" or "resumed")) 
            end,
            getSpeed = function() return state.speed end,
            getScale = function() return state.scale end,
            isPaused = function() return state.isPaused end
        }
        
        if registerControlFunctions then
            registerControlFunctions("Triangle", controlFuncs)
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
    
    setRotation(obj, time * 0.8 * state.speed)
    
    local s = state.scale + 0.3 * math.sin(time * 1.5 * state.speed)
    setScale(obj, {x=s, y=s})
    
    setPosition(obj, {
        x = -0.5 + 0.2 * math.sin(time * 0.5 * state.speed),
        y = -0.5 + 0.2 * math.cos(time * 0.7 * state.speed)
    })
end