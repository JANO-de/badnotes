#pragma once
#include <QQuickPaintedItem>
#include <QColor>
#include <QPointF>
#include <vector>

struct InkPoint { QPointF pos; float pressure; };
struct Stroke { std::vector<InkPoint> pts; QColor color; float width; bool highlighter; };

class InkCanvas : public QQuickPaintedItem {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QColor penColor MEMBER m_color NOTIFY toolChanged)
    Q_PROPERTY(qreal penWidth MEMBER m_width NOTIFY toolChanged)
    Q_PROPERTY(QString tool MEMBER m_tool NOTIFY toolChanged) // pen|highlighter|eraser
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY historyChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY historyChanged)
public:
    explicit InkCanvas(QQuickItem *parent = nullptr);
    void paint(QPainter *p) override;
    bool canUndo() const { return !m_strokes.empty(); }
    bool canRedo() const { return !m_redo.empty(); }
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
    Q_INVOKABLE void clear();
signals:
    void toolChanged();
    void historyChanged();
protected:
    bool event(QEvent *e) override;
private:
    void begin(const QPointF &p, float pr);
    void extend(const QPointF &p, float pr);
    void end();
    void eraseAt(const QPointF &p);
    QColor m_color{0x1c1c1e};
    qreal m_width = 3.0;
    QString m_tool = "pen";
    std::vector<Stroke> m_strokes, m_redo;
    bool m_drawing = false;
};
