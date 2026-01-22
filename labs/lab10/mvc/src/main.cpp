// Qt6 example (works similarly in Qt5 with minor changes: use event->pos()).
// Creates several shapes using QPainterPath and detects which shape was clicked.

#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QVector>

struct Shape {
    int id{};
    QString name;
    QPainterPath path;
};

class CanvasWidget final : public QWidget {
    Q_OBJECT
public:
    explicit CanvasWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setMouseTracking(true);
        setMinimumSize(600, 400);
        buildShapes();
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);

        for (const auto& s : m_shapes) {
            // Fill highlight if selected, otherwise just draw outline.
            if (m_selectedId == s.id) {
                p.setPen(QPen(Qt::black, 2));
                p.setBrush(QBrush(QColor(120, 180, 255, 140)));
                p.drawPath(s.path);
            } else {
                p.setPen(QPen(Qt::black, 2));
                p.setBrush(Qt::NoBrush);
                p.drawPath(s.path);
            }

            // Draw label near shape
            QRectF r = s.path.boundingRect();
            p.drawText(r.topLeft() + QPointF(4, -4), s.name);
        }
    }

    void mousePressEvent(QMouseEvent* e) override {
        const QPointF pt = e->position(); // Qt6. For Qt5: QPointF pt = e->pos();

        // Hit-test from topmost to bottommost: reverse order of painting.
        int hitId = -1;
        QString hitName;

        for (int i = m_shapes.size() - 1; i >= 0; --i) {
            const auto& s = m_shapes[i];

            // Fast reject by bounding box first (optional but cheap).
            if (!s.path.boundingRect().contains(pt)) continue;

            // Exact check.
            if (s.path.contains(pt)) {
                hitId = s.id;
                hitName = s.name;
                break;
            }
        }

        m_selectedId = hitId;
        if (hitId >= 0) {
            setWindowTitle(QString("Clicked: %1 (id=%2)").arg(hitName).arg(hitId));
        } else {
            setWindowTitle("Clicked: none");
        }

        update(); // repaint to show selection
    }

private:
    void buildShapes() {
        m_shapes.clear();

        // 1) Rectangle
        {
            Shape s;
            s.id = 1;
            s.name = "Rect";
            QPainterPath path;
            path.addRect(QRectF(60, 60, 180, 120));
            s.path = path;
            m_shapes.push_back(s);
        }

        // 2) Ellipse
        {
            Shape s;
            s.id = 2;
            s.name = "Ellipse";
            QPainterPath path;
            path.addEllipse(QRectF(300, 70, 200, 120));
            s.path = path;
            m_shapes.push_back(s);
        }

        // 3) Triangle (polygon)
        {
            Shape s;
            s.id = 3;
            s.name = "Triangle";
            QPainterPath path;
            path.moveTo(140, 260);
            path.lineTo(60, 360);
            path.lineTo(220, 360);
            path.closeSubpath();
            s.path = path;
            m_shapes.push_back(s);
        }

        // 4) A "donut" (shows that contains() respects holes)
        {
            Shape s;
            s.id = 4;
            s.name = "Donut";
            QPainterPath path;
            path.setFillRule(Qt::OddEvenFill);     // important for holes
            path.addEllipse(QRectF(360, 230, 160, 160));
            path.addEllipse(QRectF(405, 275, 70, 70)); // inner hole
            s.path = path;
            m_shapes.push_back(s);
        }
    }

private:
    QVector<Shape> m_shapes;
    int m_selectedId = -1;
};

#include "main.moc"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    CanvasWidget w;
    w.show();

    return app.exec();
}
