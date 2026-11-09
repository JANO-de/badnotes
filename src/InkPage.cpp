#include "InkPage.h"
#include <cmath>

void InkPage::setPageTemplate(const QString &t) {
    if (t == m_template) return;
    m_template = t; emit templateChanged(); emit changed();
}

void InkPage::beginStroke(const Stroke &s) {
    m_strokes.push_back(s);
    m_undo.push_back({true, int(m_strokes.size()) - 1, {}});
    m_redo.clear();
    emit changed(); emit edited(this);
}
void InkPage::appendPoint(const InkPoint &p) {
    if (m_strokes.empty()) return;
    m_strokes.back().pts.push_back(p);
    emit changed();
}

static qreal distToSeg(const QPointF &p, const QPointF &a, const QPointF &b) {
    const QPointF ab = b - a; const qreal l2 = ab.x()*ab.x() + ab.y()*ab.y();
    qreal t = l2 > 0 ? ((p.x()-a.x())*ab.x() + (p.y()-a.y())*ab.y()) / l2 : 0;
    t = std::clamp<qreal>(t, 0, 1);
    const QPointF c = a + ab * t;
    return std::hypot(p.x() - c.x(), p.y() - c.y());
}

bool InkPage::eraseAt(const QPointF &p, qreal r) {
    bool any = false;
    for (int i = int(m_strokes.size()) - 1; i >= 0; --i) {
        const Stroke &s = m_strokes[i];
        const qreal lim = r + s.width * 0.5;
        bool hit = false;
        if (s.pts.size() == 1) hit = std::hypot(s.pts[0].pos.x()-p.x(), s.pts[0].pos.y()-p.y()) < lim;
        for (size_t k = 1; k < s.pts.size() && !hit; ++k)
            hit = distToSeg(p, s.pts[k-1].pos, s.pts[k].pos) < lim;
        if (!hit) continue;
        m_undo.push_back({false, i, s});
        m_strokes.erase(m_strokes.begin() + i);
        any = true;
    }
    if (any) { m_redo.clear(); emit changed(); emit edited(this); }
    return any;
}

bool InkPage::undo() {
    if (m_undo.empty()) return false;
    Op op = std::move(m_undo.back()); m_undo.pop_back();
    if (op.added) { op.stroke = std::move(m_strokes[op.index]); m_strokes.erase(m_strokes.begin() + op.index); }
    else m_strokes.insert(m_strokes.begin() + op.index, op.stroke);
    m_redo.push_back(std::move(op));
    emit changed(); return true;
}
bool InkPage::redo() {
    if (m_redo.empty()) return false;
    Op op = std::move(m_redo.back()); m_redo.pop_back();
    if (op.added) m_strokes.insert(m_strokes.begin() + op.index, op.stroke);
    else m_strokes.erase(m_strokes.begin() + op.index);
    m_undo.push_back(std::move(op));
    emit changed(); return true;
}

// ---- file format v1 (see handoffs/FORMAT.md) ----
void InkPage::write(QDataStream &ds) const {
    ds << m_template << float(m_w) << float(m_h) << quint32(m_strokes.size());
    for (const Stroke &s : m_strokes) {
        ds << quint32(s.color.rgba()) << float(s.width) << quint8(s.highlighter) << quint32(s.pts.size());
        for (const InkPoint &p : s.pts) ds << float(p.pos.x()) << float(p.pos.y()) << float(p.pressure);
    }
    ds << m_extra;
}
bool InkPage::read(QDataStream &ds) {
    float w, h; quint32 n;
    ds >> m_template >> w >> h >> n;
    if (ds.status() != QDataStream::Ok || !(w > 0 && h > 0 && w < 20000 && h < 20000)) return false;
    m_w = w; m_h = h; m_strokes.clear();
    for (quint32 i = 0; i < n; ++i) {
        quint32 rgba, np; float width; quint8 hl;
        ds >> rgba >> width >> hl >> np;
        if (ds.status() != QDataStream::Ok) return false;
        Stroke s; s.color = QColor::fromRgba(rgba); s.width = width; s.highlighter = hl;
        for (quint32 k = 0; k < np; ++k) {
            float x, y, pr; ds >> x >> y >> pr;
            if (ds.status() != QDataStream::Ok) return false;
            s.pts.push_back({QPointF(x, y), pr});
        }
        m_strokes.push_back(std::move(s));
    }
    ds >> m_extra;
    return ds.status() == QDataStream::Ok;
}
