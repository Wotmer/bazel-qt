#include "../include/raycaster_widget.h"

#include <QPainterPath>
#include <cstddef>
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
    // qDebug() << this->size() << "Размер окна";
    // qDebug() << drawing_area_->size() << "Размер поля";

    UpdateBorderPolygon();
}

void RaycasterWidget::OnModeChanged(const int mode) {
    if (mode == 0) {
        mode_ = "light";
    } else if (mode == 1) {
        mode_ = "polygons";
    } else if (mode == 2) {
        mode_ = "static-lights";
    }
    update();
}

void RaycasterWidget::CreateModeSelector(QWidget* parent) {
    QGroupBox* mode_box = new QGroupBox("", parent);                             // NOLINT
    QHBoxLayout* radio_layout = new QHBoxLayout;                                 // NOLINT

    light_mode_radio_ = new QRadioButton("Режим света", mode_box);               // NOLINT
    polygons_mode_radio_ = new QRadioButton("Режим многоугольников", mode_box);  // NOLINT
    static_lights_radio_ = new QRadioButton("Статичные источники", mode_box);    // NOLINT

    light_mode_radio_->setMinimumWidth(150);
    polygons_mode_radio_->setMinimumWidth(180);
    static_lights_radio_->setMinimumWidth(180);

    light_mode_radio_->setChecked(true);
    mode_ = "light";

    mode_group_ = new QButtonGroup(this);  // NOLINT
    mode_group_->addButton(light_mode_radio_, 0);
    mode_group_->addButton(polygons_mode_radio_, 1);
    mode_group_->addButton(static_lights_radio_, 2);

    connect(  // NOLINT
        mode_group_, &QButtonGroup::idClicked, this, &RaycasterWidget::OnModeChanged);

    radio_layout->addWidget(light_mode_radio_);
    radio_layout->addWidget(polygons_mode_radio_);
    radio_layout->addWidget(static_lights_radio_);
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
    // qDebug() << drawing_area_->size() << "UpdateBorder";
    const std::vector border = {
      QPointF(0, 0), QPointF(this->width(), 0), QPointF(this->width(), this->height() - 65),
      QPointF(0, this->height() - 65)};

    // qDebug() << border[2] << "Размер border";
    // qDebug() << this->size() << "Размер this";
    if (controller_.GetPolygons().empty()) {
        controller_.AddPolygon(Polygon(border));
    } else {
        controller_.GetPolygons()[0] = Polygon(border);
        // qDebug() << "Замена границы";
    }
    // qDebug() << controller_.GetPolygons().size() << "Вектор";
    // qDebug() << controller_.GetPolygons()[0].GetVertices()[2] << "Вершина 0";
}

void RaycasterWidget::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    UpdateBorderPolygon();
    // qDebug() << "11111111111111";
    update();
}

void RaycasterWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect drawing_rect = drawing_area_->geometry();
    painter.fillRect(drawing_rect, Qt::black);
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

    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(drawing_rect);
}

void RaycasterWidget::DrawLightSource(QPainter& painter) const {
    painter.save();

    const QRect drawing_rect = drawing_area_->geometry();
    const std::vector<QPointF> lights = GetLights();

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 150, 150));

    for (const QPointF light : lights) {
        const QPointF light_pos_rel = light;
        const QPointF light_pos_abs = drawing_rect.topLeft() + light_pos_rel;

        painter.drawEllipse(light_pos_abs, 2, 2);
    }

    painter.setBrush(QColor(0, 255, 0));

    for (const auto& light : static_lights_) {
        painter.drawEllipse(drawing_rect.topLeft() + light, 4, 4);
    }
    painter.restore();
}

void RaycasterWidget::mousePressEvent(QMouseEvent* event) {
    const QRect drawing_rect = drawing_area_->geometry();
    if (!drawing_rect.contains(event->pos())) {
        return;
    }
    const QPointF adjusted_pos = event->pos() - drawing_rect.topLeft();
    if (mode_ == "static-lights" && event->button() == Qt::LeftButton) {
        bool inside_polygon = false;
        for (const auto& poly : controller_.GetPolygons()) {
            if (IsPointInPolygon(adjusted_pos, poly.GetVertices())) {
                inside_polygon = true;
                break;
            }
        }
        if (!inside_polygon) {
            static_lights_.push_back(adjusted_pos);
        }
    } else if (mode_ == "static-lights" && event->button() == Qt::RightButton) {
        const QPointF pos = event->pos() - drawing_rect.topLeft();
        static_lights_.erase(
            std::ranges::remove_if(
                static_lights_,
                [pos, this](const QPointF& p) { return controller_.Distance(p, pos) < 5.0; })
                .begin(),
            static_lights_.end());
        update();
    }
    if (mode_ == "polygons") {
        const auto& polygons = controller_.GetPolygons();
        if (event->button() == Qt::LeftButton) {
            if (!creating_polygon_) {
                controller_.AddPolygon(Polygon({adjusted_pos}));
                creating_polygon_ = true;
            } else {
                if (polygons.empty()) {
                    return;
                }

                const auto& current_poly = polygons.back();
                const auto& vertices = current_poly.GetVertices();
                const size_t current_idx = polygons.size() - 1;

                if (vertices.size() >= 2 &&
                    controller_.Distance(adjusted_pos, vertices.front()) < 10.0) {
                    if (!CheckPolygonIntersections(
                            polygons, current_idx, vertices.back(), vertices.front(), true)) {
                        controller_.AddVertexToLastPolygon(vertices.front());
                        creating_polygon_ = true;
                    } else {
                        if (!vertices.empty()) {
                            controller_.GetPolygons().back().GetVertices().pop_back();
                        }
                        creating_polygon_ = true;
                    }
                } else if (!vertices.empty()) {
                    const QPointF last_point = vertices.back();

                    if (!CheckPolygonIntersections(
                            polygons, current_idx, last_point, adjusted_pos)) {
                        controller_.AddVertexToLastPolygon(adjusted_pos);
                    } else {
                        controller_.GetPolygons().back().GetVertices().pop_back();
                    }
                }
            }
        } else if (event->button() == Qt::RightButton && creating_polygon_) {
            if (!polygons.empty() && polygons.back().GetVertices().size() < 2) {
                controller_.GetPolygons().pop_back();
            }
            creating_polygon_ = false;
        }
        controller_.AddVertexToLastPolygon(adjusted_pos);
    }

    else if (event->button() == Qt::RightButton && creating_polygon_) {
        if (controller_.GetPolygons().back().GetVertices().size() < 2) {
            controller_.GetPolygons().pop_back();
        }
        creating_polygon_ = false;
    } else if (mode_ == "light") {
        if (creating_polygon_) {
            if (controller_.GetPolygons().back().GetVertices().size() < 2) {
                controller_.GetPolygons().pop_back();
            }
            creating_polygon_ = false;
        }
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

std::vector<QPointF> RaycasterWidget::GetLights() const {
    std::vector<QPointF> lights;
    lights.push_back(controller_.GetLightSource());
    constexpr int kCount = 6;
    for (int i = 0; i < kCount; ++i) {
        constexpr int kRadius = 16;
        const double angle = 2 * M_PI * i / kCount;
        lights.emplace_back(
            lights[0].x() + (kRadius * cosl(angle)), lights[0].y() + (kRadius * sinl(angle)));
    }

    return lights;
}

void RaycasterWidget::DrawLightArea(QPainter& painter) {
    if (mode_ != "light" && mode_ != "static-lights") {
        return;
    }
    QPointF original_light = controller_.GetLightSource();
    std::vector<QPointF> points = GetLights();
    points.insert(points.end(), static_lights_.begin(), static_lights_.end());
    for (auto point : points) {
        controller_.SetLightSource(point);
        const auto rays = controller_.CastRays();
        const Polygon light_area = controller_.CreateLightArea();
        const auto& vertices = light_area.GetVertices();

        if (vertices.size() > 2) {
            QPainterPath path;
            path.moveTo(vertices[0]);
            for (size_t j = 1; j < vertices.size(); ++j) {
                path.lineTo(vertices[j]);
            }
            path.closeSubpath();

            painter.setPen(QPen(Qt::NoPen));
            painter.setBrush(QColor(255, 255, 255, 80));
            //if (std::find(static_lights_.begin(), static_lights_.end(), points) != static_lights_.end()) {
            //    painter.setBrush(QColor(255, 240, 150, 120));
            //}
            /*if (i == 0) {
                painter.setBrush(QColor(255, 255, 255, 120));
            }*/
            painter.drawPath(path);
        }
    }
    controller_.SetLightSource(original_light);}

void RaycasterWidget::DrawPolygons(QPainter& painter) const {
    QBrush polygonBrush(Qt::NoBrush);  // NOLINT
    QPen polygonPen(Qt::white, 1.5);   // NOLINT
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

bool RaycasterWidget::IsPointInPolygon(const QPointF& point, const std::vector<QPointF>& polygon) {
    if (polygon.size() < 3) {
        return false;
    }

    bool inside = false;
    for (size_t i = 1, j = polygon.size() - 1; i < polygon.size(); j = i++) {
        if (polygon[i].y() > point.y() != polygon[j].y() > point.y() &&
            point.x() < (polygon[j].x() - polygon[i].x()) * (point.y() - polygon[i].y()) /
                                (polygon[j].y() - polygon[i].y()) +
                            polygon[i].x()) {
            inside = !inside;
        }
    }
    return inside;
}