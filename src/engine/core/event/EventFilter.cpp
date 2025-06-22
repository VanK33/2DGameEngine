// src/engine/core/event/EventFilter.cpp

#include "EventFilter.hpp"
#include <algorithm>

namespace engine::event {

// TypeFilter implementation
TypeFilter::TypeFilter(const std::vector<EventType>& allowedTypes) : allowedTypes_(allowedTypes) {}

bool TypeFilter::ShouldProcess(const std::shared_ptr<Event>& event) const {
    return std::find(allowedTypes_.begin(), allowedTypes_.end(), event->getType()) != allowedTypes_.end();
}

void TypeFilter::AddAllowedType(EventType type) {
    if (std::find(allowedTypes_.begin(), allowedTypes_.end(), type) 
        == allowedTypes_.end()) {
        allowedTypes_.push_back(type);
    }
}

void TypeFilter::RemoveAllowedType(EventType type) {
    allowedTypes_.erase(
        std::remove(allowedTypes_.begin(), allowedTypes_.end(), type),
        allowedTypes_.end()
    );
}

const std::vector<EventType>& TypeFilter::GetAllowedTypes() const {
    return allowedTypes_;
}

// PriorityFilter implementation
PriorityFilter::PriorityFilter(EventPriority minPriority)
    : minPriority_(minPriority) {}

bool PriorityFilter::ShouldProcess(const std::shared_ptr<Event>& event) const {
    return static_cast<int>(event->getPriority()) <= static_cast<int>(minPriority_);
}

void PriorityFilter::SetMinPriority(EventPriority priority) {
    minPriority_ = priority;
}

EventPriority PriorityFilter::GetMinPriority() const {
    return minPriority_;
}

// AndFilter implementation
AndFilter::AndFilter() = default;

AndFilter::AndFilter(std::vector<std::unique_ptr<EventFilter>> filters)
    : filters_(std::move(filters)) {}

bool AndFilter::ShouldProcess(const std::shared_ptr<Event>& event) const {
    for (const auto& filter : filters_) {
        if (!filter->ShouldProcess(event)) {
            return false;
        }
    }
    return true;
}

void AndFilter::AddFilter(std::unique_ptr<EventFilter> filter) {
    if (filter) {
        filters_.push_back(std::move(filter));
    }
}

void AndFilter::ClearFilters() {
    filters_.clear();
}

size_t AndFilter::GetFilterCount() const {
    return filters_.size();
}

// OrFilter implementation
OrFilter::OrFilter() = default;

OrFilter::OrFilter(std::vector<std::unique_ptr<EventFilter>> filters)
    : filters_(std::move(filters)) {}

bool OrFilter::ShouldProcess(const std::shared_ptr<Event>& event) const {
    for (const auto& filter : filters_) {
        if (filter->ShouldProcess(event)) {
            return true;
        }
    }
    return false;
}

void OrFilter::AddFilter(std::unique_ptr<EventFilter> filter) {
    if (filter) {
        filters_.push_back(std::move(filter));
    }
}

void OrFilter::ClearFilters() {
    filters_.clear();
}

size_t OrFilter::GetFilterCount() const {
    return filters_.size();
}

// NotFilter implementation
NotFilter::NotFilter(std::unique_ptr<EventFilter> filter)
    : filter_(std::move(filter)) {}

bool NotFilter::ShouldProcess(const std::shared_ptr<Event>& event) const {
    return filter_ ? !filter_->ShouldProcess(event) : true;
}

void NotFilter::SetFilter(std::unique_ptr<EventFilter> filter) {
    filter_ = std::move(filter);
}

// AllowAllFilter implementation
bool AllowAllFilter::ShouldProcess(const std::shared_ptr<Event>& event) const {
    return true;
}

// BlockAllFilter implementation
bool BlockAllFilter::ShouldProcess(const std::shared_ptr<Event>& event) const {
    return false;
}


} // namespace engine::event