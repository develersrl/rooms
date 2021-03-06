#!/usr/bin/env python

from xml.dom import minidom
from xml.etree import ElementTree

from structdata import g_project

def prettify(content):
    """
    Return a pretty-printed XML string for the Element.
    """
    rough_string = ElementTree.tostring(content, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")

def saveData(top, tag, dictionary):
    tag_dict = {}
    dict_todo = []
    #cicla su tutti gli elementi del dizionario
    #se trova delle liste le salva per poi richiamare se stessa
    #su questi per poter memorizzare i dati
    for key, value in dictionary.items():
        if not isinstance(value, list):
            tag_dict[key] = value
        else:
            dict_todo.append(value)
    father_tag = ElementTree.SubElement(top, tag, tag_dict)
    for el in dict_todo:
        for single_el in el:
            saveData(father_tag, single_el.tag_name, single_el.dictionary())

def saveRooms():
    top = ElementTree.Element("world",
                              g_project.data['world'].dictionary())
    for data_key, data_value in g_project.data.items():
        if data_key != "world":
            father = ElementTree.SubElement(top, data_key)
            for key, value in data_value.items():
                saveData(father, value.tag_name,
                         value.dictionary())
    return top

def saveFileRooms(path_file):
    """
    funzione che salva la struttura dati su un file .rooms
    prende in ingresso il path del file
    """

    write_file = open(path_file, 'w')
    write_file.write(prettify(saveRooms()))
