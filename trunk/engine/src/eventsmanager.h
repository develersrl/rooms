#ifndef EVENTSMANAGER_H
#define EVENTSMANAGER_H

#include <string>
#include <map>
#include <vector>

class Engine;
class Event;
class Action;

class EventsManager
{
    public:
        EventsManager(Engine *engine);
        virtual ~EventsManager();
        Event *addEvent(std::string id);
        Event *event(std::string id);
        std::vector <Action *> actionsForEvent(std::string id);
        int var(std::string id);
        void var(std::string id, int value);
    protected:
    private:
        Engine *_engine;

        std::map <std::string, Event *> _events;
        //ATTENTION: var names that begin with '!' identify internal vars.
        std::map <std::string, int> _vars;

};

#endif // EVENTSMANAGER_H