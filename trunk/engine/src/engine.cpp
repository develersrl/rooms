#include "engine.h"

Log logger;

Engine *Engine::engine = 0;

Engine::Engine()
{
    try
    {
        rooms_mgr = new RoomsManager();
        events_mgr = new EventsManager();
        _state = INITIALIZING;
    }
    catch (...)
    {
        logger.write("ERROR: cannot create a valid engine!", Log::ERROR);
        exit();
    }
}

Engine::~Engine()
{
    logger.write("QUITTING ENGINE", Log::ERROR);
    delete rooms_mgr;
    delete events_mgr;
}

Engine *Engine::createEngine()
{
    if (engine == 0)
        engine = new Engine();

    return engine;
}

Log *Engine::getLogger()
{
    return &logger;
}

void Engine::exit()
{
    delete engine;
}

void Engine::click (const int x, const int y)
{
    logger.write("Mouse click received", Log::NOTE);
    switch (_state)
    {
        case GAME:
        {
            Event *event = events_mgr->event(rooms_mgr->eventAt(x, y));
            if (event == 0)
                break;
            logger.write("Event: " + event->id, Log::NOTE);
            if (rooms_mgr->checkItemPlace(event->itemReqs()) &&
                events_mgr->checkVarReqs(event->varReqs()))
                execActions(events_mgr->actionsForEvent(event->id));
            else
                logger.write("Requirements are not satisfied", Log::NOTE);
            break;
        }
    }
}

Engine::State Engine::state() const
{
    return _state;
}

void Engine::state(const Engine::State state_name)
{
    _state = state_name;
}

void Engine::loadGame(const string filename)
{

}

bool Engine::loadWorld(const string filename)
{
    try
    {
        logger.write("Loading world from " + filename, Log::NOTE);
        TiXmlDocument document(filename.c_str());
        if (!std::xmlCheckDoc(&document)) throw "ERROR: wrong xml document!";
        TiXmlElement *root = document.RootElement();
        //Load World attributes
        logger.write(root->Attribute("name"), Log::NOTE);
        rooms_mgr->size(std::xmlReadInt(root, "width"),
                        std::xmlReadInt(root, "height"));
        rooms_mgr->name(root->Attribute("name"));
        //TODO: manage different screen resolutions
        //Loading from xml
        XmlVect images =
            std::xmlGetAllChilds(root->FirstChildElement("images"), "img");
        XmlVect rooms =
            std::xmlGetAllChilds(root->FirstChildElement("rooms"), "room");
        XmlVect events =
            std::xmlGetAllChilds(root->FirstChildElement("events"), "event");
        XmlVect items =
            std::xmlGetAllChilds(root->FirstChildElement("items"), "item");
        XmlVect vars =
            std::xmlGetAllChilds(root->FirstChildElement("vars"), "var");
        //Populating model
        createImgsFromXml(images);
        createEventsFromXml(events);
        createRoomsFromXml(rooms);
        createItemsFromXml(items);
        createVarsFromXml(vars);
        //TODO: load rest of world file
        string start_room = root->Attribute("start");
        apiRoomGoto(start_room);
        _state = GAME;
        return true;
    }
    catch (const char *msg)
    {
        logger.write(msg, Log::ERROR);
        return false;
    }
}

std::vector<std::pair<string, string> > Engine::getImgNames() const
{
    std::vector<std::pair<string, string> > v(images.begin(), images.end());
    return v;
}

void Engine::createImgsFromXml(XmlVect imgs)
{
    for (XmlVect::iterator i = imgs.begin();
         i != imgs.end(); ++i)
        images[(*i)->Attribute("id")] = (*i)->Attribute("file");
}

void Engine::createVarsFromXml(XmlVect vars)
{
    for (XmlVect::iterator i = vars.begin();
         i != vars.end(); ++i)
        events_mgr->var((*i)->Attribute("id"), std::xmlReadInt((*i), "value"));
}

void Engine::createEventsFromXml(XmlVect events)
{
    for (XmlVect::iterator i = events.begin(); i != events.end(); ++i)
    {
        Event *event = events_mgr->addEvent((*i)->Attribute("id"));
        //create items parameters
        XmlVect ireqs = std::xmlGetAllChilds((*i)->FirstChildElement("requirements"), "item_req");
        for (XmlVect::iterator j = ireqs.begin(); j != ireqs.end(); ++j)
            event->addItemReq((*j)->Attribute("id"), (*j)->Attribute("value"));
        //create var parameters
        XmlVect vreqs = std::xmlGetAllChilds((*i)->FirstChildElement("requirements"), "var_req");
        for (XmlVect::iterator j = vreqs.begin(); j != vreqs.end(); ++j)
            event->addVarReq((*j)->Attribute("id"), std::xmlReadInt((*j), "value"));
        //create actions
        XmlVect actions = std::xmlGetAllChilds((*i)->FirstChildElement("actions_if"), "action");
        for (XmlVect::iterator j = actions.begin(); j != actions.end(); ++j)
        {
            Action *act = event->addAction((*j)->Attribute("id"));
            XmlVect params = std::xmlGetAllChilds(*j, "param");
            for (XmlVect::iterator z = params.begin(); z != params.end(); ++z)
                act->pushParam((*z)->Attribute("value"));
        }
    }
}

void Engine::createRoomsFromXml(XmlVect rooms)
{
    for (XmlVect::iterator i = rooms.begin(); i != rooms.end(); ++i)
    {
        rooms_mgr->addRoom((*i)->Attribute("id"), (*i)->Attribute("bg"));
        XmlVect areas = std::xmlGetAllChilds((*i)->FirstChildElement("areas"), "area");
        for (XmlVect::iterator j = areas.begin(); j != areas.end(); ++j)
        {
            rooms_mgr->addArea((*j)->Attribute("id"),
                               (*i)->Attribute("id"),
                               std::xmlReadInt((*j), "x"),
                               std::xmlReadInt((*j), "y"),
                               std::xmlReadInt((*j), "width"),
                               std::xmlReadInt((*j), "height"),
                               (*j)->FirstChildElement("do_event")->Attribute("value"));
        }
    }
}

void Engine::createItemsFromXml(XmlVect items)
{
    for (XmlVect::iterator i = items.begin(); i != items.end(); ++i)
    {
        rooms_mgr->addItem((*i)->Attribute("id"),
                            (*i)->Attribute("room"),
                            std::xmlReadInt((*i), "x"),
                            std::xmlReadInt((*i), "y"),
                            std::xmlReadInt((*i), "width"),
                            std::xmlReadInt((*i), "height"),
                            (*i)->FirstChildElement("do_event")->Attribute("value"),
                            (*i)->Attribute("image"));
    }
}

RoomsManager *Engine::getRoomsManager() const
{
    return rooms_mgr;
}

EventsManager *Engine::getEventsManager() const
{
    return events_mgr;
}

void Engine::execActions(std::vector <Action *> actions)
{
    std::vector <Action *>::iterator i;
    for (i = actions.begin(); i != actions.end(); ++i)
    {
        //TODO: think about improving this loop
        Action act = *(*i);
        logger.write("Exec action: " + act.id, Log::NOTE);
        if (act.id == "ROOM_GOTO")
        {
            apiRoomGoto(act.popStrParam());
        } else if (act.id == "VAR_SET")
        {
            int var_value = act.popIntParam();
            string var_name = act.popStrParam();
            apiVarSet(var_name, var_value);
        } else if (act.id == "ITEM_MOVE")
        {
            string item_dest = act.popStrParam();
            string item_id = act.popStrParam();
            apiItemMove(item_id, item_dest);
        }
    }
}

void Engine::apiRoomGoto(const string id)
{
    logger.write("ROOM_GOTO: " + id, Log::NOTE);
    rooms_mgr->currentRoom(id);
}

void Engine::apiVarSet(const string id, const int value)
{
    logger.write("VAR_SET: " + id, Log::NOTE);
    events_mgr->var(id, value);
}

void Engine::apiItemMove(const string id, const string dest)
{
    logger.write("ITEM_MOVE: " + id + ", dest: " + dest, Log::NOTE);
    rooms_mgr->moveItem(id, dest);
}


