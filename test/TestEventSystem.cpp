#include "../src/events/EventManager.hpp"
#include "../src/events/EventListener.hpp"
#include "../src/events/EventType.hpp"
#include "../src/events/Event.hpp"
#include <iostream>
#include <memory>

using namespace game::events;

struct KeyData {
    int keycode;
};


class ListenerA : public EventListener {
public:
    void onEvent(std::shared_ptr<Event> event) override {
        if (event->getType() == EventType::KEY_DOWN) {
            auto data = std::static_pointer_cast<KeyData>(event->getData());
            std::cout << "[ListenerA] KeyDown received: " << data->keycode << "\n";
        }
    }
};


class ListenerB : public EventListener {
public:
    void onEvent(std::shared_ptr<Event> event) override {
        if (event->getType() == EventType::KEY_DOWN) {
            auto data = std::static_pointer_cast<KeyData>(event->getData());
            std::cout << "[ListenerB] KeyDown received: " << data->keycode << "\n";
        }
    }
};

int main() {
    EventManager manager;
    
    ListenerA a;
    ListenerB b;

    // subscribe key_down event
    manager.subscribe(EventType::KEY_DOWN, &a);
    manager.subscribe(EventType::KEY_DOWN, &b);

    auto payload1 = std::make_shared<KeyData>(KeyData{65});  // 'A'
    manager.publish(std::make_shared<Event>(EventType::KEY_DOWN, payload1));

    manager.update(); // should A and B recieve

    std::cout << "\n[Unsubscribing ListenerB]\n\n";
    manager.unsubscribe(EventType::KEY_DOWN, &b);

    auto payload2 = std::make_shared<KeyData>(KeyData{66});  // 'B'
    manager.publish(std::make_shared<Event>(EventType::KEY_DOWN, payload2));

    manager.update(); // should A recieve only

    return 0;

}
