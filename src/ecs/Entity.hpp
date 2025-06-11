// src/ecs/Entity.hpp

#pragma once

#include <cstdint>
#include <string>
#include <sstream>

namespace ECS {

class Entity {
public:
    using EntityID = uint32_t;

    // Constructors
    explicit Entity(EntityID id);
    ~Entity() = default;

    // Copy operations - deleted to prevent accidental copies
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    // Move operations
    explicit Entity(Entity&& other) noexcept;
    Entity& operator=(Entity&& other) noexcept;

    // Getters
    EntityID GetId() const { return id_; }
    const std::string& GetName() const { return name_; }
    bool IsActive() const { return active_; }

    // Setters
    void SetName(const std::string& name) { name_ = name; }
    void SetActive(bool active) { active_ = active; }

    // Comparison operators
    bool operator==(const Entity& other) const { return id_ == other.id_; }
    bool operator<(const Entity& other) const { return id_ < other.id_; }

    // Debug support
    std::string ToString() const {
        std::stringstream ss;
        ss << "Entity{id=" << id_ 
           << ", name='" << name_ 
           << "', active=" << (active_ ? "true" : "false") << "}";
        return ss.str();
    }

private:
    EntityID id_;
    std::string name_;
    bool active_ = true;
};

} // namespace ECS 