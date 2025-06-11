#include "Entity.hpp"

namespace ECS {

Entity::Entity(EntityID id)
    : id_(id), name_("Entity" + std::to_string(id)), active_(true) {}

Entity::Entity(Entity&& other) noexcept
    : id_(other.id_), name_(std::move(other.name_)), active_(other.active_) {
    other.id_ = 0;
    other.name_.clear();
    other.active_ = false;
}

Entity& Entity::operator=(Entity&& other) noexcept {
    if (this != &other) {
        id_ = other.id_;
        name_ = std::move(other.name_);
        active_ = other.active_;

        other.id_ = 0;
        other.name_.clear();
        other.active_ = false;
    }
    return *this;
}


} // namespace ECS
