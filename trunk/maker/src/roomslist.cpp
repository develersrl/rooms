#include "roomslist.h"
#include <QMenu>

RoomsList::RoomsList(QWidget *parent) :
    QListView(parent)
{
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showContextMenu(const QPoint &)));
}

void RoomsList::setWorld(World *world)
{
    this->world = world;
}

void RoomsList::showContextMenu(const QPoint &point)
{
    QMenu *menu = new QMenu;
    menu->addAction(tr("Add a room"), this, SLOT(addRoom()));
    menu->exec(mapToGlobal(point));
}

void RoomsList::addRoom()
{
    bool found = false;
    int suffix_num = 0;
    QString suffix;
    QString name;

    while (!found)
    {
        suffix.setNum(suffix_num);
        found = true;
        for (int i = 0; i < world->rooms()->count(); i++)
        {
            if (world->rooms()->at(i)->name().endsWith(suffix))
            {
                found = false;
                break;
            }
        }
        suffix_num++;
    }

    name = tr("Room") + " " + suffix;
    world->rooms()->appendRoom(name);
}