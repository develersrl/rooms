#include "settingswidget.h"

SettingsWidget::SettingsWidget(World *world, QWidget *parent) :
    QWidget(parent)
{
    setWorld(world);
    setupUi();

    setFixedWidth(250);

    connect(room_name, SIGNAL(textEdited(QString)), this, SLOT(validateRoomName(QString)));
    connect(room_name, SIGNAL(editingFinished()), this, SLOT(setRoomName()));
    connect(area_name, SIGNAL(textEdited(QString)), this, SLOT(validateAreaName(QString)));
    connect(area_name, SIGNAL(editingFinished()), this, SLOT(setAreaName()));
    connect(new_action_button, SIGNAL(clicked()), this, SLOT(newAction()));

    area_settings->hide();
}

void SettingsWidget::setWorld(World *world)
{
    setDisabled(true);
    _world = world;
}

void SettingsWidget::updateRoomSettings(Room *room)
{
    active_room = room;
    area_settings->hide();
    room_settings->show();
    room_name->setText(room->name());
    setEnabled(true);
}

void SettingsWidget::updateAreaSettings(Area *area)
{
    active_area = area;
    room_settings->hide();
    area_settings->show();
    area_name->setText(area->name());
    actions_list->clear();
    for (int i = 0; i < active_area->actions().count(); i++)
    {
        actions_list->addItem(active_area->actions().at(i)->toString());
    }
}

void SettingsWidget::validateRoomName(const QString &text)
{
    if (_world->rooms()->roomExists(text))
    {
        QPalette palette = room_name->palette();
        palette.setColor(QPalette::Base, QColor(255, 0, 0));
        room_name->setPalette(palette);
    }
    else
        room_name->setPalette(QPalette(QPalette::Base, Qt::white));
}

void SettingsWidget::setRoomName()
{
    if (!_world->rooms()->roomExists(room_name->text()))
        active_room->setName(room_name->text());
    else
        room_name->setText(active_room->name());

    room_name->setPalette(QPalette(QPalette::Base, Qt::white));
}

void SettingsWidget::validateAreaName(const QString &text)
{
    if (active_room->areaExists(text))
    {
        QPalette palette = area_name->palette();
        palette.setColor(QPalette::Base, QColor(255, 0, 0));
        area_name->setPalette(palette);
    }
    else
        area_name->setPalette(QPalette(QPalette::Base, Qt::white));
}

void SettingsWidget::setAreaName()
{
    if (!active_room->areaExists(area_name->text()))
        active_area->setName(area_name->text());
    else
        area_name->setText(active_area->name());

    area_name->setPalette(QPalette(QPalette::Base, Qt::white));
}

void SettingsWidget::newAction()
{
    Action *action = active_area->addAction();
    action->setType((Action::ActionType)action_combobox->currentIndex());
    action->setRoom(room_combobox->currentText());
    actions_list->addItem(action->toString());
}

void SettingsWidget::setupUi()
{
    vspacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    hspacer = new QSpacerItem(0, 0, QSizePolicy::Expanding);

    //Room settings
    room_settings = new QGroupBox("Room settings");
    room_settings_layout = new QVBoxLayout;

    room_name_layout = new QHBoxLayout;
    room_name_label = new QLabel("Room name:");
    room_name = new QLineEdit;
    room_name_layout->addWidget(room_name_label);
    room_name_layout->addWidget(room_name);

    room_settings_layout->addLayout(room_name_layout);
    room_settings->setLayout(room_settings_layout);

    //Area settings
    area_settings = new QGroupBox("Area settings");
    area_settings_layout = new QVBoxLayout;

    area_name_layout = new QHBoxLayout;
    area_name_label = new QLabel("Area name:");
    area_name = new QLineEdit;
    area_name_layout->addWidget(area_name_label);
    area_name_layout->addWidget(area_name);

    action_comboboxes_layout = new QHBoxLayout;
    action_combobox = new QComboBox;
    room_combobox = new QComboBox;
    action_combobox->addItem("Go to room");
    action_combobox->setFixedHeight(20);
    room_combobox->setFixedHeight(20);
    room_combobox->setModel(_world->rooms());
    action_comboboxes_layout->addWidget(action_combobox);
    action_comboboxes_layout->addWidget(room_combobox);

    actions_label = new QLabel("<b>Do actions:</b>");

    new_action_layout = new QHBoxLayout;
    new_action_button = new QPushButton("Add action");
    new_action_button->setFixedHeight(20);
    new_action_layout->addSpacerItem(hspacer);
    new_action_layout->addWidget(new_action_button);

    actions_list = new QListWidget;

    area_settings_layout->addLayout(area_name_layout);
    area_settings_layout->addWidget(actions_label);
    area_settings_layout->addWidget(actions_list);
    area_settings_layout->addLayout(action_comboboxes_layout);
    area_settings_layout->addLayout(new_action_layout);

    area_settings->setLayout(area_settings_layout);

    //Settings
    settings_layout = new QVBoxLayout;
    settings_layout->addWidget(room_settings);
    settings_layout->addWidget(area_settings);
    settings_layout->addSpacerItem(vspacer);

    setLayout(settings_layout);
}
