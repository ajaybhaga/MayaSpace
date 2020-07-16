//
// MunvoAgentSim
//
// C++ Implementation by Ajay Bhaga
//
// Based on http://www.dannyb.me/posts/2014/02/creating_event_system_c++/

#include "event.h"

int EventHandler::counter;

EventHandler::EventHandler() : id{0} {}

EventHandler::EventHandler(const Func &func) : _func{func} {
    this->id = ++EventHandler::counter;
}

void EventHandler::operator()() {
    this->_func();
}

void EventHandler::operator=(const EventHandler &func) {
    if (this->_func == nullptr) {
        this->_func = func;
        this->id = ++EventHandler::counter;
    } else {
        std::cerr << "Error!" << std::endl;
    }
}

bool EventHandler::operator==(const EventHandler &del) {
    return this->id = del.id;
}

bool EventHandler::operator!=(std::nullptr_t) {
    return this->_func != nullptr;
}

void Event::notifyHandlers() {
    std::vector<std::shared_ptr<EventHandler>>::iterator func = this->handlers.begin();
    for (; func != this->handlers.end(); ++func) {
        if (*func != nullptr && (*func)->id != 0) {
            (*(*func))();
        }
    }
}

void Event::addHandler(const EventHandler &handler) {
    this->handlers.push_back(std::shared_ptr<EventHandler>(new EventHandler{handler}));
}

void Event::removeHandler(const EventHandler &handler) {
    std::vector<std::shared_ptr<EventHandler>>::iterator toRemove = this->handlers.begin();
    for (; toRemove != this->handlers.end(); ++toRemove) {
        if (*(*toRemove) == handler) {
            this->handlers.erase(toRemove);
            break;
        }
    }
}

void Event::operator()() {
    this->notifyHandlers();
}

Event &Event::operator+=(const EventHandler &handler) {
    this->addHandler(handler);
    return *this;
}

Event &Event::operator+=(const EventHandler::Func &handler) {
    this->addHandler(EventHandler{handler});
    return *this;
}

Event &Event::operator-=(const EventHandler &handler) {
    this->removeHandler(handler);
    return *this;
}

Event &Event::operator-=(const EventHandler::Func &handler) {
    this->removeHandler(EventHandler{handler});
    return *this;
}