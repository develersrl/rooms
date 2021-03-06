#!/usr/bin/env python
from xml.etree import ElementTree
from misc.odict import OrderedDict
from os.path import split
from structdata import Action

from structdata import g_project
from structdata import class_tag

from upgradeversion import upgradeVersion

class OpenFileError(Exception):

    def __init__(self, file_path):
        self.file_path = file_path

    def __str__(self):
        return "Error opening file %s", self.file_path

def loadRooms(xml):
    rooms = OrderedDict()
    room = None
    for line in list(xml.find("rooms")):
            room = class_tag[line.tag](**line.attrib)
            for child in line:
                area = class_tag[child.tag](**child.attrib)
                room.areas.append(area)
            rooms[room.id] = room
    return rooms

def loadEvents(xml):
    events = OrderedDict()
    event = None
    for line in list(xml.find("events")):
        event = class_tag[line.tag](**line.attrib)
        events[event.id] = event
        for child in line:
            if child.tag == "item_req" or child.tag == "var_req":
                requirement = class_tag[child.tag](**child.attrib)
                event.requirements.append(requirement)
            else:
                action = Action(child.attrib['id'])
                event.actions.append(action)
                for second_child in child:
                        param = class_tag[second_child.tag](**second_child.attrib)
                        action.params.append(param)
    return events

def loadDialogs(xml):
    dialogs = OrderedDict()
    for line in list(xml.find("dialogs")):
        dialog = class_tag[line.tag](**line.attrib)
        dialogs[dialog.id] = dialog
        for child in line:
            step = class_tag[child.tag](**child.attrib)
            dialog.steps.append(step)
            for second_child in child:
                if second_child.tag == "item_req" or second_child.tag == "var_req":
                    requirement = class_tag[second_child.tag](**second_child.attrib)
                    step.requirements.append(requirement)
                elif second_child.tag == "action":
                    action = Action(second_child.attrib['id'])
                    step.actions.append(action)
                    for grand_child in second_child:
                            param = class_tag[grand_child.tag](**grand_child.attrib)
                            action.params.append(param)
                else:
                    link = class_tag[second_child.tag](**second_child.attrib)
                    step.links.append(link)
    return dialogs

def loadItems(xml):
    items = OrderedDict()
    for line in list(xml.find("items")):
        item = class_tag[line.tag](**line.attrib)
        items[item.id] = item
    return items

def loadInformation(xml):
    informations = None
    for node in xml.getiterator('world'):
        informations = class_tag[node.tag](**node.attrib)
    return informations

def loadImages(xml):
    images = {}
    for line in list(xml.find("images")):
        images[line.attrib['file']] = class_tag[line.tag](**line.attrib)
    return images

def loadVars(xml):
    variable = {}
    for line in list(xml.find("vars")):
        variable[line.attrib['id']] = class_tag[line.tag](**line.attrib)
    return variable

def openRooms(xml):

    """
    funzione che prende in ingresso la stinga relativa al file xml che si
    sta aprendo e salva i dati nel modello dei dati
    """

    world = loadInformation(xml)
    g_project.data['world'] = world
    images = loadImages(xml)
    items = loadItems(xml)
    variables = loadVars(xml)
    events = loadEvents(xml)
    rooms = loadRooms(xml)
    dialogs = loadDialogs(xml)
    g_project.data['world'] = world
    g_project.data['images'] = images
    g_project.data['items'] = items
    g_project.data['vars'] = variables
    g_project.data['events'] = events
    g_project.data['rooms'] = rooms
    g_project.data['dialogs'] = dialogs

def openFileRooms(file_path):
    """
    funzione per il caricamento dei dati salvati da un file .rooms
    prende in ingresso il path del file da controllare
    Si suppone che nel momento che il file viene passato alle funzioni per
    ottenere le informazioni del progetto il file sia in un formato corretto
    Se il caricamento va a buon fine memorizza nella variabile globale g_project
    tutte le informazioni altrimenti lancia un eccezione di tipo OpenFileError
    la funzione puo' prendere anche un file .rooms che ha una versione
    precedente all'ultima realizzata
    """

    xml = ElementTree.fromstring(open(file_path, 'rb').read())

    try:
        from xml.etree.ElementTree import ParseError as XMLError
    except ImportError:
        # Python < 2.7
        from xml.parsers.expat import ExpatError as XMLError

    try:
        xml = upgradeVersion(xml)
    except XMLError:
        raise OpenFileError(file_path)

    try:
        openRooms(xml)
    except ValueError:
        raise OpenFileError(file_path)

    from utils import g_ptransform
    g_ptransform.path_file = split(file_path)[0]
