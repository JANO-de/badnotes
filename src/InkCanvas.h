#pragma once
#include <QQuickPaintedItem>
#include <QColor>
#include <QPointer>
#include "InkPage.h"

// Draws and edits one InkPage. Item size = page size * zoom; the painter is scaled so ink stays sharp.
class InkCanvas : public QQuickPaintedItem {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(InkPage *page READ page WRITE setPage NOTIFY pageChanged)
    Q_PROPERTY(QColor paperColor MEMBER m_paper NOTIFY styleChanged)
    Q_PROPERTY(QColor lineColor MEMBER m_line NOTIFY styleChanged)
public:
    explicit InkCanvas(QQuickItem *parent = nullptr);
    void paint(QPainter *p) override;
    InkPage *page() const { return m_page; }
    void setPage(InkPage *p);
signals:
    void pageChanged();
    void styleChanged();
protected:
    bool event(QEvent *e) override;
private:
    qreal scaleFactor() const { return m_page ? width() / m_page->width() : 1.0; }
    void begin(const QPointF &viewPos, float pr);
    void extend(const QPointF &viewPos, float pr);
    void end() { m_drawing = false; }
    void drawTemplate(QPainter *p) const;
    QPointer<InkPage> m_page;
    QColor m_paper{"#ffffff"}, m_line{"#d6dbe6"};
    bool m_drawing = false;
    QPointF m_last;
};
