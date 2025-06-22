// src/engine/core/event/ConditionalEventListener.hpp

#pragma once

#include "EventListener.hpp"
#include "EventFilter.hpp"
#include <memory>

namespace engine::event {
class ConditionalEventListener : public EventListener {
public:
    ConditionalEventListener() = default;
    explicit ConditionalEventListener(std::unique_ptr<EventFilter> filter);

    void SetFilter(std::unique_ptr<EventFilter> filter);
    EventFilter* GetFilter() const { return filter_.get(); }
    
    void onEvent(const std::shared_ptr<Event>& event) override;

protected:
    virtual void ProcessEvent(const std::shared_ptr<Event>& event) = 0;

private:
    std::unique_ptr<EventFilter> filter_;
};

} // namespace engine::event