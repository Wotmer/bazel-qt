#ifndef RAY_H
#define RAY_H

#include <QPointF>
#include <cmath>

class Ray {
    public:
    Ray(const QPointF& begin, const QPointF& end, double angle);
    QPointF begin() const;
    QPointF end() const;
    double Angle() const;
    void SetBegin(const QPointF& begin);
    void SetEnd(const QPointF& end);
    void SetAngle(double angle);
    Ray Rotate(double angle) const;
    private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};
#endif //RAY_H
