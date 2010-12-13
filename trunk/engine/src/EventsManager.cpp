#include "EventsManager.h"
#include "Event.h"
#include "Action.h"
#include "RoomsEngine.h"

EventsManager::EventsManager(RoomsEngine *engine)
{
    //ctor
    _engine = engine;
}

EventsManager::~EventsManager()
{
    //dtor
    std::map<std::string, Event *>::iterator i;
    for (i = _events.begin(); i != _events.end(); i++)
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
std::vector <Action *> EventsManager::actionsForEvent(std::string id)
{
    //TODO: handle error here
    return _events[id]->actions();
}
