#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"

#include <QPointF>
#include <vector>

class Controller {
   public:
    const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& a);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);
    QPointF GetLightSource() const;
    void SetLightSource(const QPointF& new_light_source);

   private:
    std::vector<Polygon> polygons_;
    QPointF light_source_;
};
#endif  // CONTROLLER_H
