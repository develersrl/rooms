#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    wizard(new Wizard(this)),
    world(NULL)
{
    ui->setupUi(this);

    rooms_list = new RoomsList;
    room_view = new RoomView;
    settings = new SettingsWidget;

    ui->splitter->addWidget(rooms_list);
    widget = new QWidget;
    layout = new QGridLayout;
    vspacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    hspacer = new QSpacerItem(0, 0, QSizePolicy::Expanding);
    layout->addWidget(room_view, 0, 0);
    layout->addItem(hspacer, 0, 1);
    layout->addItem(vspacer, 1, 0, 1, 2);
    layout->setMargin(0);
    widget->setLayout(layout);
    ui->splitter->addWidget(widget);
    ui->splitter->addWidget(settings);

    ui->centralWidget->setDisabled(true);

    connect(room_view, SIGNAL(roomChanged(Room*)), room_view, SIGNAL(selected(Room*)));
    connect(room_view, SIGNAL(selected(Room*)), settings, SLOT(updateRoomSettings(Room*)));
    connect(room_view, SIGNAL(selected(Area*)), settings, SLOT(updateAreaSettings(Area*)));
    connect(ui->action_Save, SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(ui->action_Open, SIGNAL(triggered()), this, SLOT(openProject()));
    connect(ui->action_New, SIGNAL(triggered()), wizard, SLOT(show()));
    connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(close()));

    connect(rooms_list, SIGNAL(selected(QModelIndex)),
            room_view, SLOT(changeActiveRoom(QModelIndex)));

    connect(wizard, SIGNAL(accepted()), this, SLOT(newProject()));

    adjustSize();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete wizard;
}

void MainWindow::saveProject()
{
    QString project_filename = QFileDialog::getSaveFileName(this, "Save project", QDir::homePath());
    QDir::setCurrent(project_filename.section("/", 0, -2));
    QFile file(project_filename);
    if (!file.open(QIODevice::WriteOnly))
        return;

    file.write(createXml().toAscii());
    file.close();

    QDir data_dir(QDir::currentPath() + "/" + world->name() + "_data");
    data_dir.mkpath(data_dir.absolutePath());

    for (int i = 0; i < world->rooms()->count(); i++)
    {
        world->rooms()->at(i)->background().save(data_dir.absolutePath() + "/" +
                                                 world->rooms()->at(i)->name() + "_bg.png");
    }
}

void MainWindow::openProject()
{
    QDomDocument doc("RoomsProjectFile");
    QString project_filename = QFileDialog::getOpenFileName(this, "Open project",
                                                            QDir::homePath(),
                                                            "Rooms project (*.rooms)");
    QFile file(project_filename);
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!doc.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();

    if (world != NULL)
        delete world;
    world = createWorld(doc);
    rooms_list->setWorld(world);
    room_view->setWorld(world);
    settings->setWorld(world);
    adjustSize();
    ui->centralWidget->setEnabled(true);
}

void MainWindow::newProject()
{
    if (world != NULL)
        delete world;
    world = new World(wizard->worldName(), wizard->worldSize());
    rooms_list->setWorld(world);
    room_view->setWorld(world);
    settings->setWorld(world);
    adjustSize();
    ui->centralWidget->setEnabled(true);
}

QString MainWindow::createXml() const
{
    QString xml;
    QDomDocument doc("RoomsProjectFile");

    QDomElement xworld = doc.createElement("world");
    xworld.setAttribute("version", "ROOMS_VANILLA");
    xworld.setAttribute("name", world->name());
    xworld.setAttribute("width", world->size().width());
    xworld.setAttribute("height", world->size().height());
    xworld.setAttribute("start", 0);
    doc.appendChild(xworld);

    //<images>
    //this block is useless for now because <img> contains only the filepath, which is
    //already included as "bg" attribute of <room>
    QDomElement ximages = doc.createElement("images");
    for (int i = 0; i < world->rooms()->count(); i++)
    {
        QDomElement ximg = doc.createElement("img");
        ximg.setAttribute("file", world->rooms()->at(i)->name());
        ximages.appendChild(ximg);
    }
    xworld.appendChild(ximages);
    //</images>

    //<rooms>
    QDomElement xrooms = doc.createElement("rooms");
    for (int i = 0; i < world->rooms()->count(); i++)
    {
        QDomElement xroom = doc.createElement("room");
        xroom.setAttribute("id", world->rooms()->at(i)->name());
        xroom.setAttribute("bg", "./" + world->name() + "_data/" +
                           world->rooms()->at(i)->name() + "_bg.png");
        QDomElement xareas = doc.createElement("areas");
        for (int j = 0; j < world->rooms()->at(i)->areas().count(); j++)
        {
            QDomElement xarea = doc.createElement("area");
            xarea.setAttribute("id", world->rooms()->at(i)->areas().at(j)->name());
            xarea.setAttribute("x", world->rooms()->at(i)->areas().at(j)->rect().x());
            xarea.setAttribute("y", world->rooms()->at(i)->areas().at(j)->rect().y());
            xarea.setAttribute("width", world->rooms()->at(i)->areas().at(j)->rect().width());
            xarea.setAttribute("height", world->rooms()->at(i)->areas().at(j)->rect().height());
            xareas.appendChild(xarea);
        }
        xroom.appendChild(xareas);
        xrooms.appendChild(xroom);
    }
    xworld.appendChild(xrooms);
    //</rooms>

    xml = doc.toString();
    return xml;
}

World *MainWindow::createWorld(const QDomDocument &doc)
{
    QDomElement xworld = doc.elementsByTagName("world").at(0).toElement();
    World *world = new World(xworld.attribute("name"), QSize(xworld.attribute("width").toInt(),
                                                             xworld.attribute("height").toInt()));

    //<images>
    //this block is useless for now because <img> contains only the filepath, which is
    //already included as "bg" attribute of <room>
    RoomsModel *rooms = world->rooms();
    QDomNode ximages = xworld.elementsByTagName("images").at(0);
    QDomElement ximg = ximages.firstChildElement();
    while (!ximg.isNull())
    {
        ximg = ximg.nextSiblingElement();
    }
    //</images>

    //<rooms>
    QDomNode xrooms = xworld.elementsByTagName("rooms").at(0);
    QDomElement xroom = xrooms.firstChildElement();
    while (!xroom.isNull())
    {
        rooms->appendRoom();
        Room *room = rooms->at(rooms->count()-1);
        QString id(xroom.attribute("id"));
        QString bg_file(xroom.attribute("bg"));
        QPixmap bg(bg_file);
        bg = bg.scaled(world->size());
        room->setName(id);
        room->setBackground(bg);
        //<areas>
        QDomNode xareas = xroom.firstChild();
        QDomElement xarea = xareas.firstChildElement();
        while (!xarea.isNull())
        {
            QString id(xarea.attribute("id"));
            QRect rect(xarea.attribute("x").toInt(),
                       xarea.attribute("y").toInt(),
                       xarea.attribute("width").toInt(),
                       xarea.attribute("height").toInt());
            room->addArea(rect);
            room->areas().at(room->areas().count()-1)->setName(id);
            xarea = xarea.nextSiblingElement();
        }
        //</areas>
        xroom = xroom.nextSiblingElement();
    }
    //</rooms>

    return world;
}
