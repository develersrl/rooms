#!/usr/bin/env python

import sys

from PyQt4.QtGui import *
from PyQt4.QtCore import *

from structdata import g_project
from structdata import Area
from structdata import Event

from utils import g_ptransform
import os

from arearesize import AreaResize


class EditorButton(QToolButton):
    """
    classe base per i bottoni di editing delle room
    al costruttore deve essere passato il path dell'icona che deve essere
    mostrata e la room che sta venendo editata
    """
    attr = None

    def sizeHint(self):
        return QSize(30, 30)

    def __init__(self, icon_path, room, parent=None):
        super(EditorButton, self).__init__(parent)
        self.icon_path = icon_path
        self.room = room
        self.icon = QPixmap(self.icon_path).scaled(30, 30,
                                                       Qt.KeepAspectRatio,
                                                       Qt.SmoothTransformation)
    def paintEvent(self, event=None):
        super(EditorButton, self).paintEvent(event)
        p = QPainter(self)
        p.setOpacity(self.getOpacity())
        p.drawPixmap(QPoint(0, 0), self.icon)

    def getOpacity(self):
        """
        funzione che ritorna il valore di opacita' per l'immagine che deve
        essere disegnata sul bottone. Se la room associata al bottone ha
        settato il parametro corrispondente al bottone la funzione torna 1.
        altrimenti 0.5
        """

        if self.room is not None and getattr(self.room, self.attr):
            return 1.
        return 0.5

    def setRoom(self, room):
        """
        funzione per settare la room associata al bottone, da utilizzare
        quando cambia la room in fase di editing
        """
        self.room = room

class ChangeBGMButton(EditorButton):
    attr = "bgm"

class ChangeBGButton(EditorButton):
    attr = "bg"

class ChangeRoomName(QLineEdit):
    pass

class RoomEditor(QWidget):

    """
    classe per l'editing della room. E' possibile andare a modificare il
    background e la background music della room, il suo nome e aggiungere
    nuove area. Inoltre visualizza tutte le aree precedentemente create
    """

    def __init__(self, room=None, parent=None):
        super(RoomEditor, self).__init__(parent)
        self.room = room
        if self.room is not None:
            self.room_bg = QPixmap(g_ptransform.relativeToAbsolute(room.bg))
            self.setMinimumSize(self.room_bg.width(), self.room_bg.height())
        else:
            self.room_bg = None
            self.setMinimumSize(int(g_project.data['world'].width),
                                int(g_project.data['world'].height))
        self.setSizePolicy(QSizePolicy(QSizePolicy.Preferred,
                                       QSizePolicy.Preferred))

        self.area_drag_start = None
        self.area_drag_curr = None
        self.resize_areas = []

        self.change_room_name = ChangeRoomName()
        self.change_room_name.setAlignment(Qt.AlignCenter)
        self.change_room_bgm = ChangeBGMButton("image/musical_note.png", self.room)
        self.change_room_bg = ChangeBGButton("image/PageTurn.jpg", self.room)
        self.setRoom(self.room)
        self.setMouseTracking(True)

        horizontal_button_layout = QHBoxLayout()
        horizontal_button_layout.addWidget(self.change_room_bgm)
        horizontal_button_layout.addStretch()
        horizontal_button_layout.addWidget(self.change_room_bg)

        vertical_layout = QVBoxLayout()
        vertical_layout.addLayout(horizontal_button_layout)
        vertical_layout.addStretch()
        horizontal = QHBoxLayout()
        horizontal.setAlignment(Qt.AlignCenter)
        horizontal.addStretch()
        horizontal.addWidget(self.change_room_name)
        horizontal.addStretch()
        vertical_layout.addLayout(horizontal)

        self.setLayout(vertical_layout)

        self.createAllAreaResize()

        self.connect(self.change_room_name,
                     SIGNAL("textEdited(const QString &)"),
                     self.updateRoomName)
        self.connect(self.change_room_bg, SIGNAL("clicked()"), self.setRoomBg)
        self.connect(self.change_room_bgm, SIGNAL("clicked()"), self.setRoomBgm)


    def createAllAreaResize(self):
        """
        funzione che mostra tutte le aree mediante AreaResize
        nella room corrente
        """
        self.resize_areas = []
        if self.room is not None:
            for area in self.room.areas:
                self.createAreaResize(area)

    def createAreaResize(self, area):
        """
        funzione che crea un oggetto di tipo AreaResize associato ad un'area
        che deve essere passata come parametro
        """
        area_resize = AreaResize(area, self.room_bg.width(),
                                     self.room_bg.height(), self)
        area_resize.move(float(area.x), float(area.y))
        self.resize_areas.append(area_resize)
        area_resize.show()

    def setRoomBg(self):
        """funzione per settare il background della room"""
        file_open = QFileDialog()
        path_file = file_open.getOpenFileName(filter="*.png *.jpg")
        if path_file:
            room_bg = os.path.relpath(unicode(path_file))
            g_project.changeImage(self.room.bg, room_bg)
            self.room.bg = room_bg
            g_project.notify()

    def setRoomBgm(self):
        """funzione per settare la background music della room"""
        file_open = QFileDialog()
        path_file = file_open.getOpenFileName(filter="*.mp3")
        if path_file:
            self.room.bgm = os.path.relpath(unicode(path_file))
            g_project.notify()

    def createArea(self, x_start, y_start, x_stop, y_stop, event_name):
        """
        funzione che crea una nuova area e mostra l'AreaResize associata.
        Alla funzione vengono passate le coordinate dell'angolo in alto a
        sinistra, il punto in basso a destra e il nome dell'evento a essa
        associata
        """
        area = Area.create(self.room, max(0, x_start), max(0, y_start),
                           min(x_stop, self.room_bg.width()),
                           min(y_stop, self.room_bg.height()),
                           event_name)
        self.createAreaResize(area)

    def mousePressEvent(self, event=None):
        if 0 <= event.pos().x() <= self.room_bg.width() and\
           0 <= event.pos().y() <= self.room_bg.height():
            self.area_drag_start = event.pos()
            self.area_drag_curr = event.pos()
            self.update()

    def keyPressEvent(self, event=None):
        if event.key() == Qt.Key_Escape:
            self.area_drag_start = None
            self.area_drag_curr = None
            self.update()


    def mouseMoveEvent(self, e):
        if self.area_drag_start is None:
            return
        #controllo di essere nell'area dell'mmagine di sfondo
        x = self.clamp(e.x(), self.room_bg.width())
        y = self.clamp(e.y(), self.room_bg.height())
        self.area_drag_curr = QPoint(x, y)
        self.update()

    def clamp(self, value, maximium):
        return min(value, maximium) if value > 0 else maximium(0, value)

    def mouseReleaseEvent(self, e):
        if self.area_drag_curr is None:
            return
        x = min(self.area_drag_start.x(), self.area_drag_curr.x())
        y = min(self.area_drag_start.y(), self.area_drag_curr.y())
        width = abs(self.area_drag_curr.x() - self.area_drag_start.x())
        height = abs(self.area_drag_curr.y() - self.area_drag_start.y())
        event = Event.create()
        g_project.data['events'][event.id] = event
        self.createArea(x, y, width, height, event.id)
        g_project.notify()
        self.area_drag_start = None
        self.area_drag_curr = None
        self.update()

    def paintEvent(self, e):
        QWidget.paintEvent(self, e)

        p = QPainter(self)
        if self.room is not None:
            p.drawPixmap(QPoint(0, 0), self.room_bg)
        p.setPen(Qt.blue)
        # Draw currently painted area
        if self.area_drag_start is not None and self.area_drag_curr is not None:
            p.drawRect(QRect(self.area_drag_start.x(),
                             self.area_drag_start.y(),
                       self.area_drag_curr.x() - self.area_drag_start.x(),
                       self.area_drag_curr.y() - self.area_drag_start.y()))

    def updateRoomName(self, name):
        """funzione per il cambio del nome della room"""
        new_room_name = unicode(name)
        self.emit(SIGNAL("currentRoomNameChanged(QString)"),
                  new_room_name)
        g_project.changeRoomName(self.room.id, new_room_name)

    def setRoom(self, room):
        if room:
            self.room = room
            self.change_room_name.setText(self.room.id)
            if os.path.exists(self.room.bg):
                self.room_bg = QPixmap(self.room.bg)
            else:
                self.room_bg = QPixmap(g_ptransform.relativeToAbsolute(self.room.bg))

    def changeCurrentRoom(self, room_id):
        """
        funzione per cambiare la room visualizzata, prende in ingresso
        l'id della room che deve essere visualizzata
        """
        self.room = g_project.data['rooms'][unicode(room_id)]
        if self.room:
            if os.path.exists(self.room.bg):
                self.room_bg = QPixmap(self.room.bg)
            else:
                self.room_bg = QPixmap(g_ptransform.relativeToAbsolute(self.room.bg))
            self.change_room_name.setText(self.room.id)
            for resize_area in self.resize_areas:
                resize_area.setParent(None)
            self.resize_areas = []
            self.createAllAreaResize()
            self.change_room_bg.setRoom(self.room)
            self.change_room_bgm.setRoom(self.room)
            self.setEnableEditor(True)
            self.update()

    def updateData(self):
        if self.room is not None and self.room.id in g_project.data['rooms'].keys():
            self.setRoom(self.room)
            self.setMinimumSize(int(g_project.data['world'].width),
                              int(g_project.data['world'].height))
            for resize_area in self.resize_areas:
                    resize_area.setParent(None)
            self.resize_areas = []
            self.createAllAreaResize()
            self.setEnableEditor(True)
        else:
            self.change_room_name.setText("")
            self.setEnableEditor(False)


    def setEnableEditor(self, value):
        self.change_room_bg.setEnabled(value)
        self.change_room_bgm.setEnabled(value)
        self.change_room_name.setEnabled(value)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    rm = RoomEditor(None)
    rm.show()
    app.exec_()
