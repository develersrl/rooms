#include "eventsmanager.h"
#include "event.h"
#include "action.h"
#include "engine.h"

EventsManager::EventsManager(Engine *engine)
{
    _engine = engine;
}

EventsManager::~EventsManager()
{
    for (std::map<std::string, Event *>::iterator i = _events.begin();
         i != _events.end(); i++)
    {
        delete i->second;
    }
    _events.clear();
}

Event *EventsManager::addEvent(std::string id)
{
    if (event(id) != 0)
        return 0;
    else
    {
        Event *e = new Event(id);
        _events[id] = e;
        return e;
    }
}

Event *EventsManager::event(std::string id)
{
    std::map<std::string, Event *>::iterator i = _events.find(id);
    if (i == _events.end())
        return 0;
    else
        return i->second;
}

int EventsManager::var(std::string id)
{
    std::map<std::string, int>::iterator i = _vars.find(id);
    if (i == _vars.end())
        return 0;
    else
        return i->second;
}

void EventsManager::var(std::string id, int value)
{
    _vars[id] = value;
}

std::vector <Action *> EventsManager::actionsForEvent(std::string id)
{
    return _events[id]->actions();
}
