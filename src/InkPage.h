#pragma once
#include <QObject>
#include <QColor>
#include <QPointF>
#include <QByteArray>
#include <QDataStream>
#include <QtQml/qqmlregistration.h>
#include <vector>

struct InkPoint { QPointF pos; float pressure = 1.f; };
struct Stroke {
    std::vector<InkPoint> pts;
    QColor color;
    float width = 3.f;
    bool highlighter = false;
};

// One notebook page: template + strokes + own undo history (page units, A4 @96dpi).
class InkPage : public QObject {
    Q_OBJECT
    QML_UNCREATABLE("owned by Notebook")
    Q_PROPERTY(QString pageTemplate READ pageTemplate WRITE setPageTemplate NOTIFY templateChanged) // blank|lined|grid|dotted
    Q_PROPERTY(qreal width READ width CONSTANT)
    Q_PROPERTY(qreal height READ height CONSTANT)
public:
    explicit InkPage(QObject *parent = nullptr) : QObject(parent) {}
    QString pageTemplate() const { return m_template; }
    void setPageTemplate(const QString &t);
    qreal width() const { return m_w; }
    qreal height() const { return m_h; }
    const std::vector<Stroke> &strokes() const { return m_strokes; }

    void beginStroke(const Stroke &s);              // records an undo op
    void appendPoint(const InkPoint &p);            // extends last stroke
    bool eraseAt(const QPointF &p, qreal radius);   // removes hit strokes (undoable)
    bool undo();
    bool redo();

    void write(QDataStream &ds) const;
    bool read(QDataStream &ds);
signals:
    void templateChanged();
    void changed();              // repaint
    void edited(InkPage *self);  // a user edit was recorded
private:
    struct Op { bool added; int index; Stroke stroke; };
    QString m_template = "lined";
    qreal m_w = 794, m_h = 1123;
    std::vector<Stroke> m_strokes;
    std::vector<Op> m_undo, m_redo;
    QByteArray m_extra; // reserved for forward compat (PDF background etc.)
};
