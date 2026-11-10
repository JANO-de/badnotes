#pragma once
#include <QQuickPaintedItem>
#include <QPointer>
#include <QImage>
#include <QTimer>
#include "InkPage.h"

// Cached small preview of a page; re-rendered 350ms after the page stops changing.
class PageThumb : public QQuickPaintedItem {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(InkPage *page READ page WRITE setPage NOTIFY pageChanged)
    Q_PROPERTY(QColor paperColor MEMBER m_paper NOTIFY styleChanged)
    Q_PROPERTY(QColor lineColor MEMBER m_line NOTIFY styleChanged)
public:
    explicit PageThumb(QQuickItem *parent = nullptr);
    void paint(QPainter *p) override;
    InkPage *page() const { return m_page; }
    void setPage(InkPage *p);
signals:
    void pageChanged();
    void styleChanged();
private:
    QPointer<InkPage> m_page;
    QColor m_paper{"#ffffff"}, m_line{"#cfd6e4"};
    QImage m_cache;
    bool m_dirty = true;
    QTimer m_debounce;
};
