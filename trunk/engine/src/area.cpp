#include "area.h"

Area::Area(std::string name): id(name)
{
    enabled(true);
}

Area::~Area()
{

}

void Area::size(int x, int y, int width, int height)
{
    _x = x;
    _y = y;
    _width = width;
    _height = height;
}

int Area::x()
{
    return _x;
}

int Area::y()
{
    return _y;
}

int Area::h()
{
    return _height;
}

int Area::w()
{
    return _width;
}

std::string Area::event()
{
    return _event;
}

void Area::event(std::string event)
{
    _event = event;
}

void Area::enabled(bool value)
{
    _enabled = value;
}


bool Area::enabled()
{
    return _enabled;
}
