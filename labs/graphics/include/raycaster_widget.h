#ifndef RAYCASTER_WIDGET_H
#define RAYCASTER_WIDGET_H

#include <QMainWindow>

class RaycasterWidget : public QMainWindow
{
    Q_OBJECT

public:
    RaycasterWidget(QWidget *parent = nullptr);
    ~RaycasterWidget();
};
#endif // RAYCASTER_WIDGET_H
