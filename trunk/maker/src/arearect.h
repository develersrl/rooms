#ifndef AREAWIDGET_H
#define AREAWIDGET_H

#include <QGraphicsRectItem>
#include "core/area.h"

class AreaRect : public QGraphicsRectItem
{

public:
    explicit AreaRect(Area *area);
    Area *area() const;

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    Area *_area;
};

#endif // AREAWIDGET_H
