#include "InkCanvas.h"
#include <QPainter>
#include <QTabletEvent>
#include <QMouseEvent>
#include <cmath>

InkCanvas::InkCanvas(QQuickItem *parent) : QQuickPaintedItem(parent) {
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptTouchEvents(false); // touch = pan/zoom in QML (palm rejection)
    setAntialiasing(true);
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
}

void InkCanvas::begin(const QPointF &p, float pr) {
    m_drawing = true;
    if (m_tool == "eraser") { eraseAt(p); return; }
    bool hl = m_tool == "highlighter";
    QColor c = m_color; if (hl) c.setAlphaF(0.35);
    m_strokes.push_back({{{p, pr}}, c, float(hl ? m_width * 4 : m_width), hl});
    m_redo.clear(); emit historyChanged(); update();
}
void InkCanvas::extend(const QPointF &p, float pr) {
    if (!m_drawing) return;
    if (m_tool == "eraser") { eraseAt(p); return; }
    auto &s = m_strokes.back();
    QPointF q = s.pts.back().pos * 0.35 + p * 0.65; // light smoothing
    s.pts.push_back({q, pr}); update();
}
void InkCanvas::end() { m_drawing = false; }

void InkCanvas::eraseAt(const QPointF &p) {
    const qreal r = 12;
    for (auto it = m_strokes.begin(); it != m_strokes.end(); ++it)
        for (auto &pt : it->pts)
            if (std::hypot(pt.pos.x() - p.x(), pt.pos.y() - p.y()) < r) {
                m_strokes.erase(it); update(); emit historyChanged(); return;
            }
}

bool InkCanvas::event(QEvent *e) {
    switch (e->type()) {
    case QEvent::TabletPress: case QEvent::TabletMove: case QEvent::TabletRelease: {
        auto *t = static_cast<QTabletEvent *>(e);
        float pr = std::max(0.05, t->pressure());
        if (e->type() == QEvent::TabletPress) begin(t->position(), pr);
        else if (e->type() == QEvent::TabletMove) extend(t->position(), pr);
        else end();
        e->accept(); return true;
    }
    case QEvent::MouseButtonPress: begin(static_cast<QMouseEvent*>(e)->position(), 0.6f); e->accept(); return true;
    case QEvent::MouseMove: extend(static_cast<QMouseEvent*>(e)->position(), 0.6f); e->accept(); return true;
    case QEvent::MouseButtonRelease: end(); e->accept(); return true;
    default: return QQuickPaintedItem::event(e);
    }
}

void InkCanvas::paint(QPainter *p) {
    p->setRenderHint(QPainter::Antialiasing);
    for (const auto &s : m_strokes) {
        if (s.pts.size() < 2) continue;
        QPen pen(s.color, s.width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        for (size_t i = 1; i < s.pts.size(); ++i) {
            float pr = s.highlighter ? 1.f : (s.pts[i-1].pressure + s.pts[i].pressure) * 0.5f;
            pen.setWidthF(s.width * (0.4 + 1.2 * pr));
            p->setPen(pen);
            p->drawLine(s.pts[i-1].pos, s.pts[i].pos);
        }
    }
}
void InkCanvas::undo() { if (m_strokes.empty()) return; m_redo.push_back(m_strokes.back()); m_strokes.pop_back(); update(); emit historyChanged(); }
void InkCanvas::redo() { if (m_redo.empty()) return; m_strokes.push_back(m_redo.back()); m_redo.pop_back(); update(); emit historyChanged(); }
void InkCanvas::clear() { m_strokes.clear(); m_redo.clear(); update(); emit historyChanged(); }
