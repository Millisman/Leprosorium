#pragma once

#include "event/event.h"
#include "event/handler.h"
#include "utils/cpp.h"
#include "utils/queue.h"

namespace event {

class Loop {
    Loop() {}
public:
    static void post(const Event &event);
    static void dispatch();
    static void addHandler(Handler *ph);
protected:
    friend class Handler;
private:
    static Loop &get() {
        static Loop loop;
        return loop;
    }
    void dispatch_impl();
    utils::Queue<Event> events;
    utils::Queue<Handler *> handlers;
    DISALLOW_COPY_AND_ASSIGN(Loop);
};

}  // namespace event

