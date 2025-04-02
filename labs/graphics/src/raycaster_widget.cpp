#include "../include/raycaster_widget.h"

#include <QPainterPath>
#include <qboxlayout.h>
#include <qgroupbox.h>

RaycasterWidget::RaycasterWidget(QWidget* parent) : QMainWindow(parent) {  // NOLINT
    QWidget* centralWidget = new QWidget(this);                            // NOLINT
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);              // NOLINT
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget* controlPanel = new QWidget();  // NOLINT
    controlPanel->setFixedHeight(65);
    CreateModeSelector(controlPanel);

    drawingArea_ = new QWidget();  // NOLINT
    drawingArea_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    drawingArea_->setStyleSheet("background-color: white;");

    mainLayout->addWidget(controlPanel);
    mainLayout->addWidget(drawingArea_, 1);

    UpdateBorderPolygon();
}

void RaycasterWidget::OnModeChanged(const int mode) {
    mode_ = (mode == 0) ? "light" : "polygons";
    update();
}

void RaycasterWidget::CreateModeSelector(QWidget* parent) {
    QGroupBox* mode_box = new QGroupBox("", parent);                             // NOLINT
    QHBoxLayout* radio_layout = new QHBoxLayout;                                 // NOLINT

    light_mode_radio_ = new QRadioButton("Режим света", mode_box);               // NOLINT
    polygons_mode_radio_ = new QRadioButton("Режим многоугольников", mode_box);  // NOLINT

    light_mode_radio_->setMinimumWidth(150);
    polygons_mode_radio_->setMinimumWidth(180);

    light_mode_radio_->setChecked(true);
    mode_ = "light";

    mode_group_ = new QButtonGroup(this);  // NOLINT
    mode_group_->addButton(light_mode_radio_, 0);
    mode_group_->addButton(polygons_mode_radio_, 1);

    connect(  // NOLINT
        mode_group_, &QButtonGroup::idClicked, this, &RaycasterWidget::OnModeChanged);

    radio_layout->addWidget(light_mode_radio_);
    radio_layout->addWidget(polygons_mode_radio_);
    radio_layout->addStretch();
    mode_box->setLayout(radio_layout);

    QVBoxLayout* mainLayout = new QVBoxLayout(parent);  // NOLINT
    mainLayout->addWidget(mode_box);
    mainLayout->setContentsMargins(10, 5, 10, 5);
    mode_box->setStyleSheet(
        "QGroupBox { border: 1px solid gray; border-radius: 3px; margin-top: 10px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; }"
        "QRadioButton { padding: 3px; }");
}

void RaycasterWidget::UpdateBorderPolygon() {
    QRect drawingRect = drawingArea_->geometry(); // NOLINT
    const std::vector border = {
      QPointF(0, 0), QPointF(drawingRect.width(), 0),
      QPointF(drawingRect.width(), drawingRect.height()), QPointF(0, drawingRect.height())};

    if (controller_.GetPolygons().empty()) {
        controller_.AddPolygon(Polygon(border));
    } else {
        controller_.GetPolygons()[0] = Polygon(border);
    }
}

void RaycasterWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect drawing_rect = drawingArea_->geometry();
    painter.save();
    painter.translate(drawing_rect.topLeft());

    DrawPolygons(painter);
    DrawLightSources(painter);
    if (mode_ == "light") {
        DrawLightArea(painter);
    }
    painter.restore();

    painter.setPen(QPen(QColor(150, 150, 150), 2));
    painter.drawRect(drawing_rect);
    painter.setPen(QPen(Qt::white, 1));
    painter.drawRect(drawing_rect.adjusted(1, 1, -1, -1));
}

void RaycasterWidget::mousePressEvent(QMouseEvent* event) {
    if (mode_ == "polygons") {
        if (event->button() == Qt::LeftButton) {
            if (!creating_polygon_) {
                controller_.AddPolygon(Polygon({event->pos()}));
                creating_polygon_ = true;
            } else {
                controller_.AddVertexToLastPolygon({event->pos()});
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
        constexpr int kStep = 5;
        switch (event->key()) {
            case Qt::Key_Left:
                light.rx() -= kStep;
                break;
            case Qt::Key_Right:
                light.rx() += kStep;
                break;
            case Qt::Key_Up:
                light.ry() -= kStep;
                break;
            case Qt::Key_Down:
                light.ry() += kStep;
                break;
            default:
                return;
        }
        controller_.SetLightSource(light);
        update();
    }
}

void RaycasterWidget::DrawLightArea(QPainter& painter) {
}

void RaycasterWidget::DrawPolygons(QPainter& painter) {
}

void RaycasterWidget::DrawLightSources(QPainter& painter){

}