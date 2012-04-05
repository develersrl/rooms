#!/usr/bin/env python

from structdata import g_project
from openfilerooms import openRooms
from savefilerooms import saveRooms

class UndoRedo(object):

    """
    classe utilizzata per le operazioni di undo/redo dell'editor.
    La classe salva i vari passi, e' possibile tornare un passo indietro con
    la funzione di undo e tornare un passo avanti con quella di redo. 
    Tutte le volte che si salva un passo di modifica deve essere salvata la
    room correntemente selezionata tramite la funzione addSelectedRoom.
    La classe implementa un template observer per far agli oggetti interessati
    quando e' stato aggiunto uno step di salvataggio
    """

    def __init__(self):
        g_project.subscribe(self)
        self.reset()
        self._subscibers = []

    def reset(self):
        self._list_of_project = []
        self._add_element = True
        self._list_index = -1
        self._list_of_room = []

    def clearUndoRedoList(self):
        self.reset()

    def undo(self):
        self._list_index -= 1
        self._add_element = False
        openRooms(self._list_of_project[self._list_index])

    def redo(self):
        self._add_element = False
        self._list_index += 1
        openRooms(self._list_of_project[self._list_index])


    def getCurrentRoom(self):
        room_id = self._list_of_room[self._list_index]
        room = g_project.data['rooms'][room_id]
        return room

    def addSelectedRoom(self, room=None):
        if room is not None:
            self._list_of_room.append(room.id)
        else:
            self._list_of_room.append("")

    def moreUndo(self):
        if len(self._list_of_project) == 0:
            return False
        return (self._list_index > 0)

    def moreRedo(self):
        if len(self._list_of_project) == 0:
            return False
        return (self._list_index < len(self._list_of_project) - 1)

    def updateData(self):
        if self._add_element:
            self._list_index += 1
            if len(self._list_of_project) > 0:
                self._list_of_project = self._list_of_project[:self._list_index]
                self._list_of_room = self._list_of_room[:self._list_index]
            self._list_of_project.append(saveRooms())
        self._add_element = True
        self.notify()

    def unsubscribe(self, unsubscriber):
        self._subscibers.remove(unsubscriber)

    def subscribe(self, subscriber):
        assert subscriber not in self._subscibers
        self._subscibers.append(subscriber)

    def notify(self):
        for subscriber in self._subscibers:
            subscriber.updateData()


g_undoredo = UndoRedo()
