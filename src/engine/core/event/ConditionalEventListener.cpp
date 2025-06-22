// src/engine/core/event/ConditionalEventListener.cpp

#include "ConditionalEventListener.hpp"

namespace engine::event {

ConditionalEventListener::ConditionalEventListener(std::unique_ptr<EventFilter> filter)
    : filter_(std::move(filter)) {}

void ConditionalEventListener::SetFilter(std::unique_ptr<EventFilter> filter) {
    filter_ = std::move(filter);
}

void ConditionalEventListener::onEvent(const std::shared_ptr<Event>& event) {
    if (!filter_ || filter_->ShouldProcess(event)) {
        ProcessEvent(event);
    }
}

} // namespace engine::event