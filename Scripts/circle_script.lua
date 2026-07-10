-- Скрипт для круга
local state = {
    speed = 1.0,
    radius = 0.3,
    isPaused = false
}

local myObject = nil
local myName = "Circle"
local controlFuncs = {}

function onStart(obj)
    print("Circle script started!")
    
    myObject = obj
    
    if obj then
        setColor(obj, {r=0.0, g=1.0, b=0.0})
        setPosition(obj, {x=0.5, y=0.0})
        
        local registeredName = registerObject(obj, myName)
        print("Registered with name: " .. (registeredName or "nil"))
        
        controlFuncs = {
            setSpeed = function(speed) 
                state.speed = speed 
                print("Circle speed: " .. speed) 
            end,
            setRadius = function(radius) 
                state.radius = radius 
                print("Circle radius: " .. radius) 
            end,
            togglePause = function() 
                state.isPaused = not state.isPaused 
                print("Circle " .. (state.isPaused and "paused" or "resumed")) 
            end,
            getSpeed = function() return state.speed end,
            getRadius = function() return state.radius end,
            isPaused = function() return state.isPaused end
        }
        
        if registerControlFunctions then
            registerControlFunctions("Circle", controlFuncs)
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
    
    setPosition(obj, {
        x = 0.5 + state.radius * math.sin(time * 0.8 * state.speed),
        y = state.radius * math.sin(time * 1.6 * state.speed) * 0.5
    })
    
    setColor(obj, {
        r = 0.2 + 0.8 * math.abs(math.sin(time * 0.5)),
        g = 0.2 + 0.8 * math.abs(math.sin(time * 0.7 + 1)),
        b = 0.2 + 0.8 * math.abs(math.sin(time * 0.9 + 2))
    })
end