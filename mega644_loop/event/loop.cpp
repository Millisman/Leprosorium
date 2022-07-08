#include "event/loop.h"
#include "mcu/timer.h"

namespace event {
void Loop::post(const Event &event) { get().events.push(event); }
void Loop::addHandler(Handler *ph) { get().handlers.push(ph); }
void Loop::dispatch() { get().dispatch_impl(); }
void Loop::dispatch_impl() {
    for (auto it = events.begin(); it != events.end();) {
        const Event &event = *it;
            for (auto handler : handlers) handler->onEvent(event);
            events.erase(it++);
    }
}

}  // namespace event

