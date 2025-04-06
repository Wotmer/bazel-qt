#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"

#include <QPointF>
#include <vector>

class Controller {
   public:
    [[nodiscard]] std::vector<Polygon>& GetPolygons();
    [[nodiscard]] const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& a);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);
    [[nodiscard]] QPointF GetLightSource() const;
    void SetLightSource(const QPointF& new_light_source);

    [[nodiscard]] std::vector<Ray> CastRays() const;
    void IntersectRays(std::vector<Ray>* rays) const;
    [[nodiscard]] Polygon CreateLightArea() const;
    void RemoveAdjacentRays(std::vector<Ray>* rays) const;

   private:
    std::vector<Polygon> polygons_;
    QPointF light_source_ = {100, 100};
    static double Distance(QPointF a, QPointF b);
    static double AngleBetween(const QPointF& center, const QPointF& point);
};
#endif  // CONTROLLER_H
