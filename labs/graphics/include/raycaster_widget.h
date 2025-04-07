#ifndef RAYCASTER_WIDGET_H
#define RAYCASTER_WIDGET_H

#include "controller.h"

#include <QButtonGroup>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QRadioButton>
#include <QWidget>

class RaycasterWidget : public QMainWindow {
    Q_OBJECT

   public:
    explicit RaycasterWidget(QWidget* parent = nullptr);
    void OnModeChanged(int mode);

   protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

   private:
    static bool CheckSelfIntersection(
        const Polygon& poly, const QPointF& new_edge_start, const QPointF& new_edge_end) {
        const auto& vertices = poly.GetVertices();
        const size_t n = vertices.size();

        if (n < 2) {
            return false;
        }

        for (size_t i = 0; i < n - 1; ++i) {
            const QPointF& p1 = vertices[i];
            const QPointF& p2 = vertices[i + 1];

            // Пропускаем соседние стороны (которые имеют общую точку)
            if (i == n - 2 && p2 == new_edge_start) {
                continue;
            }
            if (i == 0 && p1 == new_edge_start) {
                continue;
            }

            if (Polygon::LineIntersection(p1, p2, new_edge_start, new_edge_end)) {
                return true;
            }
        }
        if (n >= 3 && Polygon::LineIntersection(
                          vertices.back(), vertices.front(), new_edge_start, new_edge_end)) {
            return true;
        }

        return false;
    }

    static bool CheckOtherPolygonsIntersection(const std::vector<Polygon>& polygons, size_t current_poly_index,
                                      const QPointF& new_edge_start, const QPointF& new_edge_end) {
        for (size_t i = 0; i < polygons.size(); ++i) {
            if (i == current_poly_index) continue;

            const auto& poly = polygons[i];
            const auto& vertices = poly.GetVertices();
            const size_t n = vertices.size();

            if (n < 2) continue;

            // Проверяем все стороны многоугольника
            for (size_t j = 0; j < n; ++j) {
                const QPointF& p1 = vertices[j];
                const QPointF& p2 = vertices[(j+1)%n];

                if (Polygon::LineIntersection(p1, p2, new_edge_start, new_edge_end)) {
                    return true;
                }
            }
        }
        return false;
    }
    QWidget* drawing_area_;
    Controller controller_;
    QString mode_ = "light";
    bool creating_polygon_ = false;

    QButtonGroup* mode_group_;
    QRadioButton* light_mode_radio_;
    QRadioButton* polygons_mode_radio_;

    void DrawLightArea(QPainter& painter);
    std::vector<QPointF> GetLights() const;
    void DrawPolygons(QPainter& painter) const;
    void DrawLightSource(QPainter& painter) const;
    void CreateModeSelector(QWidget* parent);
    void UpdateBorderPolygon();
};
#endif  // RAYCASTER_WIDGET_H
