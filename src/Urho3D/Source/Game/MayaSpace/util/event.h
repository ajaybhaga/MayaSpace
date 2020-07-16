//
// MunvoAgentSim
//
// C++ Implementation by Ajay Bhaga
//
// Based on http://www.dannyb.me/posts/2014/02/creating_event_system_c++/

#ifndef EANN_SIMPLE_EVENT_H
#define EANN_SIMPLE_EVENT_H

#include <functional>
#include <vector>
#include <memory>
#include <iostream>

class EventHandler {
public:
    using Func = std::function<void()>;

private:
    Func _func;

public:
    int id;
    static int counter;

    EventHandler();
    EventHandler(const Func &func);
    void operator()();
    void operator=(const EventHandler &func);
    bool operator==(const EventHandler &del);
    bool operator!=(std::nullptr_t);
};

class Event {
private:
    std::vector<std::shared_ptr<EventHandler>> handlers;
    void notifyHandlers();

public:
    void addHandler(const EventHandler &handler);
    void removeHandler(const EventHandler &handler);
    void operator()();
    Event &operator+=(const EventHandler &handler);
    Event &operator+=(const EventHandler::Func &handler);
    Event &operator-=(const EventHandler &handler);
    Event &operator-=(const EventHandler::Func &handler);
};

#endif //EANN_SIMPLE_EVENT_H
