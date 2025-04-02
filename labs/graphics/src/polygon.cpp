#include "../include/polygon.h"

#include <algorithm>
#include <limits>

Polygon::Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {
}

const std::vector<QPointF>& Polygon::GetVertexes() const {
    return vertices_;
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    if (!vertices_.empty()) {
        vertices_.back() = new_vertex;
    }
}
std::optional<QPointF> Polygon::LineIntersection(
        const QPointF& a1, const QPointF& a2, const QPointF& b1, const QPointF& b2) {
    const double denom =
        ((a1.x() - a2.x()) * (b1.y() - b2.y())) - ((a1.y() - a2.y()) * (b1.x() - b2.x()));

    if (denom == 0) {
        return std::nullopt;
    }

    const double t =
        ((a1.x() - b1.x()) * (b1.y() - b2.y()) - (a1.y() - b1.y()) * (b1.x() - b2.x())) / denom;

    const double u =
        -(((a1.x() - a2.x()) * (a1.y() - b1.y())) - ((a1.y() - a2.y()) * (a1.x() - b1.x()))) /
        denom;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        return QPointF(a1.x() + (t * (a2.x() - a1.x())), a1.y() + (t * (a2.y() - a1.y())));
    }

    return std::nullopt;
}

double Polygon::Distance(const QPointF& p1, const QPointF& p2) {
    const double dx = p1.x() - p2.x();
    const double dy = p1.y() - p2.y();
    return std::sqrt((dx * dx) + (dy * dy));
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    const QPointF ray_begin = ray.GetBegin();
    const QPointF ray_end = ray.GetEnd();

    double min = std::numeric_limits<double>::max();
    QPointF closest_intersection;
    bool found = false;

    for (size_t i = 0; i < vertices_.size(); ++i) {
        const QPointF p1 = vertices_[i];
        const QPointF p2 = vertices_[(i + 1) % vertices_.size()];

        auto intersection = LineIntersection(ray_begin, ray_end, p1, p2);

        if (intersection) {
            double const dist = Distance(ray_begin, *intersection);
            if (dist < min) {
                min = dist;
                closest_intersection = *intersection;
                found = true;
            }
        }
    }
    return found ? std::optional(closest_intersection) : std::nullopt;
}

// std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
//     std::optional<QPointF> closestIntersection;
//     double minDistance = std::numeric_limits<double>::max();
//
//     for (size_t i = 0; i < vertices.size(); ++i) {
//         QPointF p1 = vertices[i];
//         QPointF p2 = vertices[(i + 1) % vertices.size()];
//
//         double x1 = p1.x(), y1 = p1.y();
//         double x2 = p2.x(), y2 = p2.y();
//         double x3 = ray.getBegin().x(), y3 = ray.getBegin().y();
//         double x4 = ray.getEnd().x(), y4 = ray.getEnd().y();
//
//         double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
//         if (denom == 0) continue;
//
//         double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
//         double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denom;
//
//         if (t >= 0 && t <= 1 && u >= 0) {
//             QPointF intersection(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
//             double distance = std::hypot(intersection.x() - x3, intersection.y() - y3);
//
//             if (distance < minDistance) {
//                 minDistance = distance;
//                 closestIntersection = intersection;
//             }
//         }
//     }
//     return closestIntersection;
// }