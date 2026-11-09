#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QFile>
#include <QtQml/qqmlregistration.h>

// Self-update for the AppImage: compares the build id with the one in the GitHub "nightly" release body,
// downloads the new AppImage and atomically replaces $APPIMAGE. Applies on next launch.
class Updater : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString status READ status NOTIFY statusChanged) // idle|checking|downloading|ready|unsupported|error
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
public:
    explicit Updater(QObject *parent = nullptr);
    QString status() const { return m_status; }
    qreal progress() const { return m_progress; }
    bool autoUpdate() const;
    void setAutoUpdate(bool on);
    Q_INVOKABLE void checkNow();
signals:
    void statusChanged();
    void progressChanged();
    void autoUpdateChanged();
    void updated();                 // new version installed, restart to apply
private:
    void setStatus(const QString &s);
    void download(const QUrl &url);
    void fail(const QString &why);
    QNetworkAccessManager m_nam;
    QTimer m_timer;
    QString m_status = "idle";
    qreal m_progress = 0;
    QFile m_part;
};
