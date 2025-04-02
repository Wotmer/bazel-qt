#include "../include/raycaster_widget.h"

#include <qboxlayout.h>
#include <qgroupbox.h>

RaycasterWidget::RaycasterWidget(QWidget* parent) : QMainWindow(parent) {
    const std::vector<QPointF> border = {
      QPointF(0, 0), QPointF(width(), 0), QPointF(0, height()), QPointF(width(), height())};
    controller_.AddPolygon(Polygon(border));
    createModeSelector();
}

void RaycasterWidget::OnModeChanged(int mode) {
    mode_ = (mode == 0) ? "light" : "polygons";
    update();
}

void RaycasterWidget::createModeSelector() {
    QGroupBox* mode_box = new QGroupBox("Режим", this);                          // NOLINT
    QVBoxLayout* vbox = new QVBoxLayout;                                         // NOLINT

    light_mode_radio_ = new QRadioButton("Режим света", mode_box);               // NOLINT
    polygons_mode_radio_ = new QRadioButton("Режим многоугольников", mode_box);  // NOLINT

    light_mode_radio_->setChecked(true);

    mode_group_ = new QButtonGroup(this);  // NOLINT
    mode_group_->addButton(light_mode_radio_, 0);
    mode_group_->addButton(polygons_mode_radio_, 1);

    connect(
        mode_group_, QOverload<int>::of(&QButtonGroup::buttonClicked), this,
        &RaycasterWidget::OnModeChanged);

    vbox->addWidget(light_mode_radio_);
    vbox->addWidget(polygons_mode_radio_);
    mode_box->setLayout(vbox);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);  // NOLINT
    mainLayout->addWidget(mode_box);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    this->setLayout(mainLayout);
}

void RaycasterWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    drawPolygons(painter);
    drawLightSources(painter);
    if (mode_ == "light") {
        drawLightArea(painter);
    }
}

void RaycasterWidget::mousePressEvent(QMouseEvent* event) {
    if (mode_ == "polygons") {
        if (event->button() == Qt::LeftButton) {
            if (!creating_polygon_) {
                controller_.AddPolygon(Polygon({event->pos()}));
                creating_polygon_ = true;
            } else {
                controller_.AddPolygon(Polygon({event->pos()}));
            }
        } else if (event->button() == Qt::RightButton) {
            creating_polygon_ = false;
        }
    } else if (mode_ == "light") {
        controller_.SetLightSource(event->pos());
    }
    update();
}

void RaycasterWidget::mouseMoveEvent(QMouseEvent* event) {
    if (mode_ == "light") {
        controller_.SetLightSource(event->pos());
        update();
    } else if (mode_ == "polygons" && creating_polygon_) {
        controller_.UpdateLastPolygon(event->pos());
        update();
    }
}

void RaycasterWidget::keyPressEvent(QKeyEvent* event) {
    if (mode_ == "light") {
        QPointF light = controller_.GetLightSource();
        int step = 5;
        switch (event->key()) {
            case Qt::Key_Left: light.rx() -= step; break;
            case Qt::Key_Right: light.rx() += step; break;
            case Qt::Key_Up: light.ry() -= step; break;
            case Qt::Key_Down: light.ry() += step; break;
            default: return;
        }
        controller_.SetLightSource(light);
        update();
}


QButtonGroup* mode_group_ = new QButtonGroup;
QRadioButton* light_mode_radio_ = new QRadioButton;
QRadioButton* polygons_mode_radio_ = new QRadioButton;

void drawLightArea(QPainter& painter);
void drawPolygons(QPainter& painter);
void drawLightSources(QPainter& painter);