#include "PageRenderer.h"
#include <QPainterPath>

static void drawTemplate(QPainter *p, const InkPage &page, const QColor &line) {
    const QString t = page.pageTemplate();
    const qreal w = page.width(), h = page.height(), step = 32;
    if (t == "blank") return;
    p->setPen(QPen(line, 0)); // cosmetic 1px
    if (t == "lined") {
        for (qreal y = 96; y < h; y += step) p->drawLine(QPointF(0, y), QPointF(w, y));
    } else if (t == "grid") {
        for (qreal y = step; y < h; y += step) p->drawLine(QPointF(0, y), QPointF(w, y));
        for (qreal x = step; x < w; x += step) p->drawLine(QPointF(x, 0), QPointF(x, h));
    } else if (t == "dotted") {
        p->setPen(QPen(line.darker(110), 2.4, Qt::SolidLine, Qt::RoundCap));
        QVector<QPointF> pts;
        for (qreal y = step; y < h; y += step)
            for (qreal x = step; x < w; x += step) pts.append({x, y});
        p->drawPoints(pts.constData(), pts.size());
    }
}

static void drawStroke(QPainter *p, const Stroke &s) {
    if (s.pts.empty()) return;
    if (s.pts.size() == 1) {
        p->setPen(Qt::NoPen); p->setBrush(s.color);
        p->drawEllipse(s.pts[0].pos, s.width * 0.5, s.width * 0.5);
        return;
    }
    if (s.highlighter) { // one path => no darkening where it overlaps itself
        QPainterPath path(s.pts[0].pos);
        for (size_t i = 1; i < s.pts.size(); ++i) path.lineTo(s.pts[i].pos);
        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(s.color, s.width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p->drawPath(path);
        return;
    }
    QPen pen(s.color, s.width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    for (size_t i = 1; i < s.pts.size(); ++i) {
        const float pr = (s.pts[i-1].pressure + s.pts[i].pressure) * 0.5f;
        pen.setWidthF(s.width * (0.4 + 1.2 * pr));
        p->setPen(pen);
        p->drawLine(s.pts[i-1].pos, s.pts[i].pos);
    }
}

void PageRenderer::render(QPainter *p, const InkPage &page, const QSizeF &size, const QColor &paper, const QColor &line) {
    p->setRenderHint(QPainter::Antialiasing);
    p->fillRect(QRectF(QPointF(0, 0), size), paper);
    const qreal s = size.width() / page.width();
    p->save();
    p->scale(s, s);
    drawTemplate(p, page, line);
    for (const Stroke &st : page.strokes()) drawStroke(p, st);
    p->restore();
}
