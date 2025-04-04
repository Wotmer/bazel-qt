#include "../include/controller.h"

#include <algorithm>
#include <cmath>

std::vector<Polygon>& Controller::GetPolygons() {
    return polygons_;
}

const std::vector<Polygon>& Controller::GetPolygons() const {
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

    for (const auto& polygon : polygons_) {
        for (const auto& vertex : polygon.GetVertices()) {
            constexpr double kAngleIncrement = 0.0001;
            const double angle = AngleBetween(light_source_, vertex);
            rays.emplace_back(light_source_, vertex, angle);
            rays.emplace_back(light_source_, vertex, angle - kAngleIncrement);
            rays.emplace_back(light_source_, vertex, angle + kAngleIncrement);
        }
    }

    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
    for (auto& ray : *rays) {
        const QPointF ray_end = ray.GetEnd();
        double min_dist = Distance(ray.GetBegin(), ray_end);
        QPointF closest_intersection = ray_end;

        for (const auto& polygon : polygons_) {
            auto intersection = polygon.IntersectRay(ray);
            if (intersection) {
                const double dist = Distance(ray.GetBegin(), *intersection);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_intersection = *intersection;
                }
            }
        }

        ray.SetEnd(closest_intersection);
    }
}

double Controller::AngleBetween(const QPointF& center, const QPointF& point) {
    return atan2(point.y() - center.y(), point.x() - center.x());
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    if (rays->empty()) {
        return;
    }

    std::ranges::sort(*rays, [this](const Ray& a, const Ray& b) {
        return AngleBetween(light_source_, a.GetEnd()) < AngleBetween(light_source_, b.GetEnd());
    });

    const auto new_end = std::ranges::unique(*rays, [this](const Ray& a, const Ray& b) {
                             return Distance(a.GetEnd(), b.GetEnd()) < 5.0;  // Порог = 5 пикселей
                         }).begin();

    rays->erase(new_end, rays->end());
}

Polygon Controller::CreateLightArea() const {
    auto rays = CastRays();
    IntersectRays(&rays);
    RemoveAdjacentRays(&rays);

    std::vector<QPointF> vertices;
    vertices.push_back(light_source_);

    std::sort(rays.begin(), rays.end(), [this](const Ray& a, const Ray& b) {
        return atan2(a.GetEnd().y() - light_source_.y(), a.GetEnd().x() - light_source_.x()) <
               atan2(b.GetEnd().y() - light_source_.y(), b.GetEnd().x() - light_source_.x());
    });

    for (const auto& ray : rays) {
        vertices.push_back(ray.GetEnd());
    }

    if (!rays.empty()) {
        vertices.push_back(rays.front().GetEnd());
    }

    return Polygon(vertices);
}

double Controller::Distance(const QPointF a, const QPointF b) {
    const double dx = a.x() - b.x();
    const double dy = a.y() - b.y();
    return std::hypot(dx, dy);
}