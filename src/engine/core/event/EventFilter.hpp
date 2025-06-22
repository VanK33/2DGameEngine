// src/engine/core/event/EventFilter.hpp

#pragma once

#include "Event.hpp"
#include <vector>
#include <memory>

namespace engine::event {

class EventFilter {
public:
    virtual ~EventFilter() = default;

    virtual bool ShouldProcess(const std::shared_ptr<Event>& event) const = 0;
};

class TypeFilter : public EventFilter {
public:
    explicit TypeFilter(const std::vector<EventType>& allowedTypes = {});
    bool ShouldProcess(const std::shared_ptr<Event>& event) const override;

    void AddAllowedType(EventType type);
    void RemoveAllowedType(EventType type);

    const std::vector<EventType>& GetAllowedTypes() const;

private:
    std::vector<EventType> allowedTypes_;
};

class PriorityFilter : public EventFilter {
public:
    explicit PriorityFilter(EventPriority minPriority = EventPriority::LOW);
    bool ShouldProcess(const std::shared_ptr<Event>& event) const override;
    
    void SetMinPriority(EventPriority priority);
    EventPriority GetMinPriority() const;

private:
    EventPriority minPriority_;
};

class AndFilter : public EventFilter {
public:
    AndFilter();
    explicit AndFilter(std::vector<std::unique_ptr<EventFilter>> filters);

    bool ShouldProcess(const std::shared_ptr<Event>& event) const override;

    void AddFilter(std::unique_ptr<EventFilter> filter);
    void ClearFilters();
    size_t GetFilterCount() const;

private:
    std::vector<std::unique_ptr<EventFilter>> filters_;
};

class OrFilter : public EventFilter {
public:
    OrFilter();
    explicit OrFilter(std::vector<std::unique_ptr<EventFilter>> filters);
    bool ShouldProcess(const std::shared_ptr<Event>& event) const override;

    void AddFilter(std::unique_ptr<EventFilter> filter);
    void ClearFilters();
    size_t GetFilterCount() const;

private:
    std::vector<std::unique_ptr<EventFilter>> filters_;
};

class NotFilter : public EventFilter {
public:
    explicit NotFilter(std::unique_ptr<EventFilter> filter);
    
    bool ShouldProcess(const std::shared_ptr<Event>& event) const override;
    
    void SetFilter(std::unique_ptr<EventFilter> filter);
    
private:
    std::unique_ptr<EventFilter> filter_;
};

class AllowAllFilter : public EventFilter {
public:
    bool ShouldProcess(const std::shared_ptr<Event>& event) const override;
};

class BlockAllFilter : public EventFilter {
public:
    bool ShouldProcess(const std::shared_ptr<Event>& event) const override;
};

} // namespace engine::event
