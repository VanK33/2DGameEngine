#pragma once

#include "Entity.hpp"
#include <memory>
#include <sstream>

namespace ECS {

class IComponent {
public:
    using EntityID = Entity::EntityID;

    explicit IComponent(EntityID ownerId) : ownerId_(ownerId) {}
    virtual ~IComponent() = default;

    IComponent(const IComponent&) = delete;
    IComponent& operator=(const IComponent&) = delete;

    IComponent(IComponent&&) noexcept = default;
    IComponent& operator=(IComponent&&) noexcept = default;

    EntityID GetOwnerId() const { return ownerId_; }
    bool IsActive() const { return active_; }
    void SetActive(bool active) { active_ = active; }

    bool operator==(const IComponent& other) const { return ownerId_ == other.ownerId_; }
    bool operator<(const IComponent& other) const { return ownerId_ < other.ownerId_; }

    // Debugger
    virtual std::string ToString() const {
        std::stringstream ss;
        ss << "Component{owner=" << ownerId_ << ", active=" << (active_ ? "true" : "false") << "}";
        return ss.str();
    }

protected:
    const EntityID ownerId_;
    bool active_ = true;
};

} // namespace ECS 