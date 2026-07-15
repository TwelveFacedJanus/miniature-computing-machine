#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include "LuaScript.h"

namespace MentalGraphics
{

// Forward declarations
struct Vertex2D;
class Any2DObject;

// ============================================
// Vertex Structure for 2D Objects
// ============================================
struct Vertex2D {
    glm::vec2 position;   // 2D position
    glm::vec3 color;      // Color (RGB)
    glm::vec2 texCoord;   // Texture coordinates (optional)
    
    Vertex2D() : position(0.0f), color(1.0f), texCoord(0.0f) {}
    Vertex2D(const glm::vec2& pos, const glm::vec3& col, const glm::vec2& tex = glm::vec2(0.0f))
        : position(pos), color(col), texCoord(tex) {}
};

// ============================================
// 2D Object Class
// ============================================
class Any2DObject {
public:
    // Transformation properties
    glm::vec2 position = glm::vec2(0.0f);
    glm::vec2 scale = glm::vec2(1.0f);
    float rotation = 0.0f;  // In radians
    glm::vec2 pivot = glm::vec2(0.5f, 0.5f);  // Pivot point (0-1 range)
    
    // Color properties
    glm::vec3 color = glm::vec3(1.0f);
    float opacity = 1.0f;
    
    // Depth/layer
    int layer = 0;
    
    // Visibility
    bool visible = true;
    
    // Name for debugging
    std::string name = "Unnamed";

    std::shared_ptr<LuaScript> getScript() const { return _script; }
    void setScript(const std::shared_ptr<LuaScript>& script) { _script = script; }
    void setScriptFile(const std::string& filename);
    
    // ============================================
    // Constructors
    // ============================================
    Any2DObject() = default;
    
    Any2DObject(const glm::vec2& pos, const glm::vec2& scl = glm::vec2(1.0f), float rot = 0.0f)
        : position(pos), scale(scl), rotation(rot) {}
    
    virtual ~Any2DObject() = default;
    
    // ============================================
    // Vertex Management
    // ============================================
    
    // Add a single vertex
    void addVertex(const Vertex2D& vertex) {
        _vertices.push_back(vertex);
        _dirty = true;
    }
    
    // Add multiple vertices
    void addVertices(const std::vector<Vertex2D>& vertices) {
        _vertices.insert(_vertices.end(), vertices.begin(), vertices.end());
        _dirty = true;
    }
    
    // Add a rectangle
    void addRectangle(const glm::vec2& size, const glm::vec3& color = glm::vec3(1.0f)) {
        float halfWidth = size.x / 2.0f;
        float halfHeight = size.y / 2.0f;
        
        // Clear existing vertices
        _vertices.clear();
        
        // Triangle 1
        _vertices.push_back(Vertex2D(glm::vec2(-halfWidth, -halfHeight), color));
        _vertices.push_back(Vertex2D(glm::vec2(halfWidth, -halfHeight), color));
        _vertices.push_back(Vertex2D(glm::vec2(-halfWidth, halfHeight), color));
        
        // Triangle 2
        _vertices.push_back(Vertex2D(glm::vec2(halfWidth, -halfHeight), color));
        _vertices.push_back(Vertex2D(glm::vec2(halfWidth, halfHeight), color));
        _vertices.push_back(Vertex2D(glm::vec2(-halfWidth, halfHeight), color));
        
        _dirty = true;
    }
    
    // Add a triangle
    void addTriangle(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, 
                     const glm::vec3& color = glm::vec3(1.0f)) {
        _vertices.clear();
        _vertices.push_back(Vertex2D(p1, color));
        _vertices.push_back(Vertex2D(p2, color));
        _vertices.push_back(Vertex2D(p3, color));
        _dirty = true;
    }
    
    // Add a circle (approximated with triangles)
    void addCircle(float radius, int segments = 32, const glm::vec3& color = glm::vec3(1.0f)) {
        _vertices.clear();
        
        for (int i = 0; i < segments; i++) {
            float theta1 = 2.0f * 3.14159f * i / segments;
            float theta2 = 2.0f * 3.14159f * (i + 1) / segments;
            
            glm::vec2 center(0.0f);
            glm::vec2 p1(radius * cos(theta1), radius * sin(theta1));
            glm::vec2 p2(radius * cos(theta2), radius * sin(theta2));
            
            _vertices.push_back(Vertex2D(center, color));
            _vertices.push_back(Vertex2D(p1, color));
            _vertices.push_back(Vertex2D(p2, color));
        }
        
        _dirty = true;
    }
    
    // Add a line
    void addLine(const glm::vec2& start, const glm::vec2& end, const glm::vec3& color = glm::vec3(1.0f)) {
        // Lines need a different approach - using a thin rectangle
        glm::vec2 dir = glm::normalize(end - start);
        float length = glm::length(end - start);
        float thickness = 0.02f;
        
        glm::vec2 perp(-dir.y, dir.x);
        
        glm::vec2 p1 = start - perp * thickness;
        glm::vec2 p2 = start + perp * thickness;
        glm::vec2 p3 = end + perp * thickness;
        glm::vec2 p4 = end - perp * thickness;
        
        _vertices.clear();
        _vertices.push_back(Vertex2D(p1, color));
        _vertices.push_back(Vertex2D(p2, color));
        _vertices.push_back(Vertex2D(p3, color));
        
        _vertices.push_back(Vertex2D(p1, color));
        _vertices.push_back(Vertex2D(p3, color));
        _vertices.push_back(Vertex2D(p4, color));
        
        _dirty = true;
    }
    
    // Add a square
    void addSquare(float size, const glm::vec3& color = glm::vec3(1.0f)) {
        addRectangle(glm::vec2(size, size), color);
    }
    
    // ============================================
    // Getters
    // ============================================
    
    const std::vector<Vertex2D>& getVertices() const { return _vertices; }
    bool isDirty() const { return _dirty; }
    void clearDirty() { _dirty = false; }
    size_t getVertexCount() const { return _vertices.size(); }
    bool isEmpty() const { return _vertices.empty(); }
    
    // Get transformed vertices (with position, scale, rotation applied)
    std::vector<Vertex2D> getTransformedVertices() const {
        std::vector<Vertex2D> transformed;
        transformed.reserve(_vertices.size());
        
        // Build transformation matrix
        glm::mat4 transform = glm::mat4(1.0f);
        
        // Translate to pivot (to rotate around center)
        transform = glm::translate(transform, glm::vec3(-pivot.x, -pivot.y, 0.0f));
        
        // Scale
        transform = glm::scale(transform, glm::vec3(scale, 1.0f));
        
        // Rotate
        transform = glm::rotate(transform, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        
        // Translate back from pivot and to final position
        transform = glm::translate(transform, glm::vec3(position.x + pivot.x, position.y + pivot.y, 0.0f));
        
        for (const auto& vertex : _vertices) {
            glm::vec4 pos = transform * glm::vec4(vertex.position, 0.0f, 1.0f);
            Vertex2D transformedVertex;
            transformedVertex.position = glm::vec2(pos.x, pos.y);
            transformedVertex.color = vertex.color * color;  // Multiply by object color
            transformedVertex.texCoord = vertex.texCoord;
            transformed.push_back(transformedVertex);
        }
        
        return transformed;
    }
    
    // ============================================
    // Utility Functions
    // ============================================
    
    void clear() {
        _vertices.clear();
        _dirty = false;
    }
    
    void setColor(const glm::vec3& newColor) {
        color = newColor;
        _dirty = true;
    }
    
    void setPosition(const glm::vec2& newPos) {
        position = newPos;
        _dirty = true;
    }
    
    void setScale(const glm::vec2& newScale) {
        scale = newScale;
        _dirty = true;
    }
    
    void setRotation(float newRotation) {
        rotation = newRotation;
        _dirty = true;
    }
    
    // Move by delta
    void move(const glm::vec2& delta) {
        position += delta;
        _dirty = true;
    }
    
    // Scale by factor
    void scaleBy(float factor) {
        scale *= factor;
        _dirty = true;
    }
    
    // Rotate by delta
    void rotate(float delta) {
        rotation += delta;
        _dirty = true;
    }
    
    // ============================================
    // Comparison Operators for Sorting
    // ============================================
    
    bool operator<(const Any2DObject& other) const {
        return layer < other.layer;
    }
    
    bool operator>(const Any2DObject& other) const {
        return layer > other.layer;
    }

private:
    std::vector<Vertex2D> _vertices;
    bool _dirty = false;
    std::shared_ptr<LuaScript> _script;
    std::string _scriptFile;
};

// ============================================
// 2D Object Factory
// ============================================
class Any2DObjectFactory {
public:
    static std::shared_ptr<Any2DObject> createRectangle(
        const glm::vec2& size,
        const glm::vec3& color = glm::vec3(1.0f),
        const glm::vec2& position = glm::vec2(0.0f)) {
        
        auto obj = std::make_shared<Any2DObject>();
        obj->position = position;
        obj->color = color;
        obj->addRectangle(size, color);
        return obj;
    }
    
    static std::shared_ptr<Any2DObject> createTriangle(
        const glm::vec2& p1,
        const glm::vec2& p2,
        const glm::vec2& p3,
        const glm::vec3& color = glm::vec3(1.0f),
        const glm::vec2& position = glm::vec2(0.0f)) {
        
        auto obj = std::make_shared<Any2DObject>();
        obj->position = position;
        obj->color = color;
        obj->addTriangle(p1, p2, p3, color);
        return obj;
    }
    
    static std::shared_ptr<Any2DObject> createCircle(
        float radius,
        int segments = 32,
        const glm::vec3& color = glm::vec3(1.0f),
        const glm::vec2& position = glm::vec2(0.0f)) {
        
        auto obj = std::make_shared<Any2DObject>();
        obj->position = position;
        obj->color = color;
        obj->addCircle(radius, segments, color);
        return obj;
    }
    
    static std::shared_ptr<Any2DObject> createLine(
        const glm::vec2& start,
        const glm::vec2& end,
        const glm::vec3& color = glm::vec3(1.0f)) {
        
        auto obj = std::make_shared<Any2DObject>();
        obj->color = color;
        obj->addLine(start, end, color);
        return obj;
    }
    
    static std::shared_ptr<Any2DObject> createSquare(
        float size,
        const glm::vec3& color = glm::vec3(1.0f),
        const glm::vec2& position = glm::vec2(0.0f)) {
        
        auto obj = std::make_shared<Any2DObject>();
        obj->position = position;
        obj->color = color;
        obj->addSquare(size, color);
        return obj;
    }
};

} // namespace MentalGraphics