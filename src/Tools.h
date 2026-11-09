#pragma once
#include <QObject>
#include <QColor>
#include <QJSEngine>
#include <QQmlEngine>
#include <QtQml/qqmlregistration.h>

// Shared tool state (one instance for the whole app).
class Tools : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(QString tool MEMBER m_tool NOTIFY changed)          // pen | highlighter | eraser
    Q_PROPERTY(QColor penColor MEMBER m_color NOTIFY changed)
    Q_PROPERTY(qreal penWidth MEMBER m_width NOTIFY changed)
    Q_PROPERTY(bool fingerDraws MEMBER m_finger NOTIFY changed)    // default false: touch = pan/zoom
    Q_PROPERTY(bool penNear READ penNear NOTIFY penNearChanged)    // palm rejection
public:
    static Tools *instance() { static Tools t; return &t; }
    static Tools *create(QQmlEngine *, QJSEngine *) {
        QJSEngine::setObjectOwnership(instance(), QJSEngine::CppOwnership);
        return instance();
    }
    bool penNear() const { return m_near; }
    void setPenNear(bool n) { if (n != m_near) { m_near = n; emit penNearChanged(); } }
    QString m_tool = "pen";
    QColor m_color{0x1c1c1e};
    qreal m_width = 3.0;
    bool m_finger = false;
signals:
    void changed();
    void penNearChanged();
private:
    Tools() = default;
    bool m_near = false;
};
