-- ============================================
-- Common Mental Scripts Library
-- ============================================

-- Basic math utilities
function lerp(a, b, t)
    return a + (b - a) * t
end

function clamp(value, min, max)
    return math.max(min, math.min(max, value))
end

function map(value, fromMin, fromMax, toMin, toMax)
    local t = (value - fromMin) / (fromMax - fromMin)
    return lerp(toMin, toMax, t)
end

function smoothstep(edge0, edge1, x)
    local t = clamp((x - edge0) / (edge1 - edge0), 0, 1)
    return t * t * (3 - 2 * t)
end

function easeIn(t)
    return t * t
end

function easeOut(t)
    return 1 - (1 - t) * (1 - t)
end

function easeInOut(t)
    return t < 0.5 and 2 * t * t or 1 - math.pow(-2 * t + 2, 2) / 2
end

-- Object utilities (wrappers for C++ functions)
function getPos(obj)
    return getPosition(obj)
end

function setPos(obj, x, y)
    if type(x) == "table" then
        setPosition(obj, x)
    else
        setPosition(obj, {x = x, y = y})
    end
end

function getRot(obj)
    return getRotation(obj)
end

function setRot(obj, angle)
    setRotation(obj, angle)
end

function getScl(obj)
    return getScale(obj)
end

function setScl(obj, x, y)
    if type(x) == "table" then
        setScale(obj, x)
    else
        setScale(obj, {x = x, y = y})
    end
end

function getCol(obj)
    return getColor(obj)
end

function setCol(obj, r, g, b)
    if type(r) == "table" then
        setColor(obj, r)
    else
        setColor(obj, {r = r, g = g, b = b})
    end
end

function move(obj, dx, dy)
    local pos = getPos(obj)
    setPos(obj, pos.x + dx, pos.y + dy)
end

function rotate(obj, da)
    setRot(obj, getRot(obj) + da)
end

function scale(obj, factor)
    local s = getScl(obj)
    setScl(obj, s.x * factor, s.y * factor)
end

-- Oscillation
function oscillate(time, min, max, speed)
    return min + (max - min) * (0.5 + 0.5 * math.sin(time * speed))
end

function oscillatePosition(obj, minX, maxX, minY, maxY, speed)
    local time = getTime()
    setPos(obj,
        oscillate(time, minX, maxX, speed),
        oscillate(time, minY, maxY, speed)
    )
end

function oscillateRotation(obj, minAngle, maxAngle, speed)
    local time = getTime()
    setRot(obj, oscillate(time, minAngle, maxAngle, speed))
end

function oscillateScale(obj, minX, maxX, minY, maxY, speed)
    local time = getTime()
    setScl(obj,
        oscillate(time, minX, maxX, speed),
        oscillate(time, minY, maxY, speed)
    )
end

function oscillateColor(obj, minR, maxR, minG, maxG, minB, maxB, speed)
    local time = getTime()
    setCol(obj,
        oscillate(time, minR, maxR, speed),
        oscillate(time, minG, maxG, speed),
        oscillate(time, minB, maxB, speed)
    )
end

-- Orbit
function orbitAround(obj, centerX, centerY, radius, speed, phase)
    local time = getTime()
    local angle = time * speed + (phase or 0)
    setPos(obj,
        centerX + radius * math.cos(angle),
        centerY + radius * math.sin(angle)
    )
end

function followCircle(obj, centerX, centerY, radius, speed, phase)
    orbitAround(obj, centerX, centerY, radius, speed, phase)
end

function followFigure8(obj, centerX, centerY, radius, speed, phase)
    local time = getTime()
    local angle = time * speed + (phase or 0)
    setPos(obj,
        centerX + radius * math.sin(angle),
        centerY + radius * math.sin(2 * angle) * 0.5
    )
end