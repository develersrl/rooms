#!/usr/bin/env python
from xml.etree import ElementTree

#to use OrderedDict in python < 2.7
try:
    from collections import OrderedDict
except ImportError:
    from misc.dict import OrderedDict
from structData.area import Area
from structData.action import Action
from structData.param import Param
from structData.item import Item
from structData.event import Event
from structData.room import Room
from structData.var import Var
from structData.image import Image
from structData.information import Information
from structData.varRequirement import VarRequirement
from structData.itemRequirement import ItemRequirement
from structData.world import g_world

from upgradeVersion import upgradeVersion

def loadRooms(xml_file):
    rooms = OrderedDict()
    room = None
    for line in list(xml_file.find("rooms")):
        if line.tag == "room":
            room = Room(line.attrib["id"], line.attrib["bg"],
                            line.attrib["bgm"])
            for child in line:
                if child.tag == "area":
                    area = Area(child.attrib["id"],
                            child.attrib["x"],
                            child.attrib["y"],
                            child.attrib["height"],
                            child.attrib["width"],
                            child.attrib["event"])
                else:
                    raise ValueError("invalid tag %s in room" % child.tag)
                room.areas.append(area)
            rooms[room.id] = room
        else:
            raise ValueError("invalid tag %s in rooms" % line.tag)
    return rooms

def loadEvents(xml_file):
    events = OrderedDict()
    event = None
    for line in list(xml_file.find("events")):
        if line.tag == "event":
            event = Event(line.attrib["id"])
            events[event.id] = event
            for child in line:
                if child.tag == "item_req":
                    requirement = ItemRequirement(child.attrib['id'],
                                              child.attrib['value'])
                    event.requirements.append(requirement)
                elif child.tag == "var_req":
                    requirement = VarRequirement(child.attrib['id'],
                                              child.attrib['value'])
                    event.requirements.append(requirement)
                elif child.tag == "action":
                    action = Action(child.attrib['id'])
                    event.actions.append(action)
                    for second_child in child:
                        if second_child.tag == "param":
                            param = Param(second_child.attrib['value'])
                            action.params.append(param)
                        else:
                            raise ValueError("invalid tag %s in action"
                                             % second_child.tag)
                else:
                    raise ValueError("invalid tag %s in event" % child.tag)
        else:
            raise ValueError("invalid tag %s in events" % line.tag)
    return events

def loadItems(xml_file):
    items = OrderedDict()
    for line in list(xml_file.find("items")):
        if line.tag == "item":
            item = Item(line.attrib['id'],
                            line.attrib["x"],
                            line.attrib["y"],
                            line.attrib["height"],
                            line.attrib["width"],
                            line.attrib["room"],
                            line.attrib["image"],
                            line.attrib['event'])
            items[item.id] = item
        else:
            raise ValueError("invalid tag %s in events" % line.tag)
    return items

def loadInformation(xml_file):
    informations = None
    for node in xml_file.iter('world'):
        informations = Information(node.attrib['version'],
                                   node.attrib['name'],
                                   node.attrib['width'],
                                   node.attrib['height'],
                                   node.attrib['start'])
    if informations:
        return informations
    else:
        raise ValueError("invalid file format")

def loadImages(xml_file):
    images = {}
    for line in list(xml_file.find("images")):
        if line.tag == "img":
            images[line.attrib['file']] = Image(line.attrib['file'])
        else:
            raise ValueError("invalid tag %s in images" % line.tag)
    return images

def loadVars(xml_file):
    variable = {}
    for line in list(xml_file.find("vars")):
        if line.tag == "var":
            variable[line.attrib['id']] = Var(line.attrib['id'],
                                              line.attrib['value'])
        else:
            ValueError("invalid tag %s in vars" % line.tag)
    return variable

def openFileRooms(path_file):
    """
    funzione per il caricamento dei dati salvati da un file .rooms
    prende in ingresso il path del file da controllare
    ritorna un dizionario con tutte le informazioni su rooms, events, items,
    images
    la funzione puo' prendere anche un file .rooms che ha una versione
    precedente all'ultima realizzata
    """
    xml_file = upgradeVersion(path_file)
    g_world.informations = loadInformation(xml_file)
    g_world.selected_room = g_world.informations.start
    g_world.images = loadImages(xml_file)
    g_world.items = loadItems(xml_file)
    g_world.vars = loadVars(xml_file)
    g_world.events = loadEvents(xml_file)
    g_world.rooms = loadRooms(xml_file)
