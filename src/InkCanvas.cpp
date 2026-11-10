#include "InkCanvas.h"
#include "Tools.h"
#include "PageRenderer.h"
#include <QPainter>
#include <QPainterPath>
#include <QTabletEvent>
#include <QMouseEvent>
#include <cmath>

InkCanvas::InkCanvas(QQuickItem *parent) : QQuickPaintedItem(parent) {
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptTouchEvents(false);
    setAntialiasing(true);
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    connect(this, &QQuickItem::widthChanged, this, [this] { update(); });
}

void InkCanvas::setPage(InkPage *p) {
    if (m_page == p) return;
    if (m_page) disconnect(m_page, nullptr, this, nullptr);
    m_page = p;
    if (m_page) {
        connect(m_page, &InkPage::changed, this, [this] { update(); });
        connect(m_page, &InkPage::templateChanged, this, [this] { update(); });
    }
    emit pageChanged(); update();
}

void InkCanvas::begin(const QPointF &vp, float pr) {
    if (!m_page) return;
    const QPointF p = vp / scaleFactor();
    m_drawing = true; m_last = p;
    Tools *t = Tools::instance();
    if (t->m_tool == "eraser") { m_page->eraseAt(p, 10); return; }
    const bool hl = t->m_tool == "highlighter";
    Stroke s;
    s.color = t->m_color; if (hl) s.color.setAlphaF(0.35);
    s.width = float(t->m_width * (hl ? 4 : 1));
    s.highlighter = hl;
    s.pts.push_back({p, pr});
    m_page->beginStroke(s);
}

void InkCanvas::extend(const QPointF &vp, float pr) {
    if (!m_drawing || !m_page) return;
    const QPointF p = vp / scaleFactor();
    if (Tools::instance()->m_tool == "eraser") { m_page->eraseAt(p, 10); return; }
    const QPointF q = m_last * 0.35 + p * 0.65; // light smoothing
    m_last = q;
    m_page->appendPoint({q, pr});
}

bool InkCanvas::event(QEvent *e) {
    switch (e->type()) {
    case QEvent::TabletPress: case QEvent::TabletMove: case QEvent::TabletRelease: {
        auto *t = static_cast<QTabletEvent *>(e);
        Tools::instance()->setPenNear(true);
        const float pr = std::max(0.05, t->pressure());
        if (e->type() == QEvent::TabletPress) begin(t->position(), pr);
        else if (e->type() == QEvent::TabletMove) extend(t->position(), pr);
        else end();
        e->accept(); return true;
    }
    case QEvent::MouseButtonPress: case QEvent::MouseMove: case QEvent::MouseButtonRelease: {
        auto *m = static_cast<QMouseEvent *>(e);
        // finger (touch-synthesized mouse) must not draw: let Flickable pan/zoom
        const bool fromTouch = m->source() != Qt::MouseEventNotSynthesized;
        if (fromTouch && !Tools::instance()->m_finger) { e->ignore(); return false; }
        if (e->type() == QEvent::MouseButtonPress) begin(m->position(), 0.6f);
        else if (e->type() == QEvent::MouseMove) extend(m->position(), 0.6f);
        else end();
        e->accept(); return true;
    }
    default: return QQuickPaintedItem::event(e);
    }
}

void InkCanvas::paint(QPainter *p) {
    if (!m_page) return;
    PageRenderer::render(p, *m_page, size(), m_paper, m_line);
}
