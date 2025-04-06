#ifndef RAYCASTER_WIDGET_H
#define RAYCASTER_WIDGET_H

#include "controller.h"

#include <QButtonGroup>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QRadioButton>
#include <QWidget>

class RaycasterWidget : public QMainWindow {
    Q_OBJECT

   public:
    explicit RaycasterWidget(QWidget* parent = nullptr);
    void OnModeChanged(int mode);

   protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

   private:
    QWidget* drawing_area_;
    Controller controller_;
    QString mode_ = "light";
    bool creating_polygon_ = false;

    QButtonGroup* mode_group_;
    QRadioButton* light_mode_radio_;
    QRadioButton* polygons_mode_radio_;

    void DrawLightArea(QPainter& painter);
    std::vector<QPointF> GetLights() const;
    void DrawPolygons(QPainter& painter) const;
    void DrawLightSource(QPainter& painter) const;
    void CreateModeSelector(QWidget* parent);
    void UpdateBorderPolygon();
};
#endif  // RAYCASTER_WIDGET_H
