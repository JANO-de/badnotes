#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QEvent>
#include "Tools.h"

// Palm rejection: track pen proximity app-wide.
class ProximityFilter : public QObject {
public:
    using QObject::QObject;
    bool eventFilter(QObject *, QEvent *e) override {
        if (e->type() == QEvent::TabletEnterProximity) Tools::instance()->setPenNear(true);
        else if (e->type() == QEvent::TabletLeaveProximity) Tools::instance()->setPenNear(false);
        return false;
    }
};

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setApplicationName("Inkwell");
    app.setDesktopFileName("inkwell");
    QQuickStyle::setStyle("Basic");
    ProximityFilter filter;
    app.installEventFilter(&filter);
    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, [] { QCoreApplication::exit(1); }, Qt::QueuedConnection);
    engine.loadFromModule("Inkwell", "Main");
    return app.exec();
}
