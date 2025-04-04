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

    drawing_area_ = new QWidget();  // NOLINT
    drawing_area_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mainLayout->addWidget(controlPanel);
    mainLayout->addWidget(drawing_area_, 1);
    //qDebug() << this->size() << "Размер окна";
    //qDebug() << drawing_area_->size() << "Размер поля";

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
    //qDebug() << drawing_area_->size() << "UpdateBorder";
    const std::vector border = {
      QPointF(0, 0), QPointF(this->width(), 0),
      QPointF(this->width(), this->height() - 65),
      QPointF(0, this->height() - 65)};

    //qDebug() << border[2] << "Размер border";
    //qDebug() << this->size() << "Размер this";
    if (controller_.GetPolygons().empty()) {
        controller_.AddPolygon(Polygon(border));
    } else {
        controller_.GetPolygons()[0] = Polygon(border);
        //qDebug() << "Замена границы";
    }
    //qDebug() << controller_.GetPolygons().size() << "Вектор";
    //qDebug() << controller_.GetPolygons()[0].GetVertices()[2] << "Вершина 0";
}

void RaycasterWidget::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    UpdateBorderPolygon();
    //qDebug() << "11111111111111";
    update();
}

void RaycasterWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect drawing_rect = drawing_area_->geometry();
    painter.fillRect(drawing_rect, Qt::white);
    painter.save();
    painter.translate(drawing_rect.topLeft());

    DrawPolygons(painter);

    if (mode_ == "light") {
        DrawLightArea(painter);
    }
    painter.restore();

    if (mode_ == "light") {
        DrawLightSource(painter);
    }

    painter.setPen(QPen(Qt::gray, 2));
    painter.drawRect(drawing_rect);
}

void RaycasterWidget::DrawLightSource(QPainter& painter) const {
    painter.save();

    const QRect drawing_rect = drawing_area_->geometry();
    const QPointF light_pos_rel = controller_.GetLightSource();
    const QPointF light_pos_abs = drawing_rect.topLeft() + light_pos_rel;

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::red);
    painter.drawEllipse(light_pos_abs, 2, 2);

    painter.restore();
}

void RaycasterWidget::mousePressEvent(QMouseEvent* event) {
    const QRect drawing_rect = drawing_area_->geometry();
    if (!drawing_rect.contains(event->pos())) {
        return;
    }
    const QPointF adjusted_pos = event->pos() - drawing_rect.topLeft();
    if (mode_ == "polygons") {
        if (event->button() == Qt::LeftButton) {
            if (!creating_polygon_) {
                controller_.AddPolygon(Polygon({adjusted_pos}));
                creating_polygon_ = true;
            } else {
                controller_.AddVertexToLastPolygon(adjusted_pos);
            }
        } else if (event->button() == Qt::RightButton && creating_polygon_) {
            creating_polygon_ = false;
        }
    } else if (mode_ == "light") {
        controller_.SetLightSource(adjusted_pos);
    }
    update();
}

void RaycasterWidget::mouseMoveEvent(QMouseEvent* event) {
    if (mode_ == "light") {
        const QRect drawing_rect = drawing_area_->geometry();
        if (drawing_rect.contains(event->pos())) {
            const QPointF adjusted_pos = event->pos() - drawing_rect.topLeft();
            controller_.SetLightSource(adjusted_pos);
            update();
        }
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

void RaycasterWidget::DrawLightArea(QPainter& painter) const {
    if (mode_ != "light") {
        return;
    }

    const Polygon light_area = controller_.CreateLightArea();
    const auto& vertices = light_area.GetVertices();

    if (vertices.size() > 2) {
        QPainterPath path;
        path.moveTo(vertices[0]);
        for (size_t i = 1; i < vertices.size(); ++i) {
            path.lineTo(vertices[i]);
        }
        path.closeSubpath();

        painter.setPen(QPen(Qt::black, 1));
        painter.setBrush(QColor(240, 240, 240, 150));
        painter.drawPath(path);
    }
}

void RaycasterWidget::DrawPolygons(QPainter& painter) const {
    QBrush polygonBrush(Qt::NoBrush);   // NOLINT
    QPen polygonPen(Qt::black, 1.5);  // NOLINT
    painter.setBrush(polygonBrush);
    painter.setPen(polygonPen);

    const auto& polygons = controller_.GetPolygons();
    for (size_t i = 0; i < polygons.size(); ++i) {
        const auto& vertices = polygons[i].GetVertices();
        if (vertices.size() < 2) {
            continue;
        }
        QPolygonF poly;
        for (const auto& v : vertices) {
            poly << v;
        }

        if (i == 0) {
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
        } else {
            painter.setBrush(polygonBrush);
            painter.setPen(polygonPen);
        }

        painter.drawPolygon(poly);

        if (creating_polygon_ && i == polygons.size() - 1) {
            painter.setBrush(QBrush(QColor(200, 230, 255)));
            painter.drawPolygon(poly);
        }
    }
}