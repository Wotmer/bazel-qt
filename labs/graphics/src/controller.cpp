#include "../include/controller.h"

#include <algorithm>
#include <cmath>

std::vector<Polygon> Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& a) {
    polygons_.push_back(a);
}
void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().AddVertex(new_vertex);
    }
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().UpdateLastVertex(new_vertex);
    }
}

QPointF Controller::GetLightSource() const {
    return light_source_;
}
void Controller::SetLightSource(const QPointF& new_light_source) {
    light_source_ = new_light_source;
}

std::vector<Ray> Controller::CastRays() const {
    std::vector<Ray> rays;

    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {

}

Polygon Controller::CreateLightArea() const {

    return Polygon({});
}

double Controller::Distance(QPointF a, QPointF b) {
    double const dx = a.x() - b.x();
    double const dy = a.y() - b.y();
    return std::hypot(dx, dy);
}