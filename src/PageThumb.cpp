#include "PageThumb.h"
#include "PageRenderer.h"
#include <QPainter>
#include <QQuickWindow>

PageThumb::PageThumb(QQuickItem *parent) : QQuickPaintedItem(parent) {
    m_debounce.setSingleShot(true); m_debounce.setInterval(350);
    connect(&m_debounce, &QTimer::timeout, this, [this] { m_dirty = true; update(); });
    connect(this, &QQuickItem::widthChanged, this, [this] { m_dirty = true; update(); });
    connect(this, &QQuickItem::heightChanged, this, [this] { m_dirty = true; update(); });
    connect(this, &PageThumb::styleChanged, this, [this] { m_dirty = true; update(); });
}

void PageThumb::setPage(InkPage *p) {
    if (m_page == p) return;
    if (m_page) disconnect(m_page, nullptr, this, nullptr);
    m_page = p;
    if (m_page) {
        connect(m_page, &InkPage::changed, this, [this] { m_debounce.start(); });
        connect(m_page, &InkPage::templateChanged, this, [this] { m_dirty = true; update(); });
    }
    m_dirty = true; emit pageChanged(); update();
}

void PageThumb::paint(QPainter *p) {
    if (!m_page || width() < 1 || height() < 1) return;
    const qreal dpr = window() ? window()->effectiveDevicePixelRatio() : 1.0;
    if (m_dirty || m_cache.isNull() || m_cache.size() != (size() * dpr).toSize()) {
        m_cache = QImage((size() * dpr).toSize(), QImage::Format_ARGB32_Premultiplied);
        m_cache.setDevicePixelRatio(dpr);
        QPainter ip(&m_cache);
        PageRenderer::render(&ip, *m_page, size(), m_paper, m_line);
        m_dirty = false;
    }
    p->drawImage(QRectF(QPointF(0, 0), size()), m_cache);
}
