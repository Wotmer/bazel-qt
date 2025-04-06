#include "../include/controller.h"

#include <algorithm>
#include <cmath>
#include <iostream>

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
            constexpr double kMaxRayLength = 10000.0;
            // Вместо того чтобы брать значение диагонали приложения, использую большую константу
            constexpr double kAngleIncrement = 0.0001;
            const double angle = AngleBetween(light_source_, vertex);
            rays.emplace_back(light_source_, vertex, angle);

            const double dx = cosl(angle) * kMaxRayLength;
            const double dy = -sinl(angle) * kMaxRayLength;
            const QPointF end_point = light_source_ + QPointF(dx, dy);

            const Ray main_ray(light_source_, end_point, angle);

            const Ray left_ray = main_ray.Rotate(-kAngleIncrement);
            const Ray right_ray = main_ray.Rotate(kAngleIncrement);

            auto process_ray = [this](const Ray& ray) {
                double min_dist = std::numeric_limits<double>::max();
                QPointF closest_intersection = ray.GetEnd();

                for (const auto& poly : polygons_) {
                    auto intersection = poly.IntersectRay(ray);
                    if (intersection) {
                        const double dist = Distance(light_source_, *intersection);
                        if (dist < min_dist) {
                            min_dist = dist;
                            closest_intersection = *intersection;
                        }
                    }
                }
                return Ray(ray.GetBegin(), closest_intersection, ray.GetAngle());
            };

            rays.push_back(process_ray(left_ray));
            rays.push_back(process_ray(right_ray));
        }
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
    for (auto& ray : *rays) {
        double min_dist = Distance(ray.GetBegin(), ray.GetEnd());
        QPointF closest_intersection = ray.GetEnd();

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
    auto normalize_angle = [](double angle) {
        if (angle < 0) {
            angle += 2 * M_PI;
        }
        return angle;
    };
    return normalize_angle(atan2(center.y() - point.y(), point.x() - center.x()));
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    if (rays->empty()) {
        return;
    }

    std::ranges::sort(
        *rays, [this](const Ray& a, const Ray& b) { return a.GetAngle() < b.GetAngle(); });

    std::vector<Ray> new_rays;
    new_rays.push_back(rays->front());

    for (size_t i = 1; i < rays->size(); ++i) {
        const Ray& prev = new_rays.back();
        const Ray& current = (*rays)[i];
        if (std::abs(Distance(current.GetEnd(), prev.GetEnd()) > 1.0)) {
            new_rays.push_back(current);
        }
    }
    *rays = std::move(new_rays);
}

Polygon Controller::CreateLightArea() const {
    auto rays = CastRays();
    IntersectRays(&rays);
    RemoveAdjacentRays(&rays);

    if (rays.empty()) {
        return Polygon({light_source_});
    }

    std::ranges::sort(rays, [this](const Ray& a, const Ray& b) {
        const double angle_a = AngleBetween(light_source_, a.GetEnd());
        const double angle_b = AngleBetween(light_source_, b.GetEnd());
        return angle_a < angle_b;
    });

    std::vector<QPointF> vertices;

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
    const double dy = b.y() - a.y();
    return std::hypot(dx, dy);
}