#ifndef DRAWDEVICE_H
#define DRAWDEVICE_H

#include "../src/engine.h"

#include <QtGui> //QWidget, QImage
#include <map> //std::map
#include <string> //std::string
#include <fstream> //ifstream in fileExists()

using std::string;

namespace
{
    bool fileExists(string filename)
    {
        std::ifstream ifile(filename.c_str());
        return ifile;
    }
}

/*! \brief Graphic interface class.
 *         It handles user event and it controls engine.
 */
class DrawDevice: public QWidget
{
    Q_OBJECT
    private:
        Engine *engine;
        std::map <string, QPixmap *> images;
        string item_text;
        QListWidget dialog_list;
    public:
        DrawDevice(Engine *eng, QWidget *parent=0);
        ~DrawDevice();
    public:
        void initialize();
        bool loadImage(string filename);
        void quit(int status);
    public slots:
        void dialogChosed(QListWidgetItem *item);
        //QT EVENTS
        void paintEvent(QPaintEvent *event);
        void mousePressEvent(QMouseEvent * event);
        void mouseMoveEvent(QMouseEvent *event);
        void keyPressEvent(QKeyEvent *event);
        void update();
    protected:
        void drawRoom(QPainter &painter);
};

#endif // DRAWDEVICE_H
