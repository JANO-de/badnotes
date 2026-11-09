#include "Updater.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QDebug>
#include <filesystem>

#ifndef INKWELL_BUILD
#define INKWELL_BUILD "dev"
#endif
static const char *kApi = "https://api.github.com/repos/JANO-de/badnotes/releases/tags/nightly";

Updater::Updater(QObject *parent) : QObject(parent) {
    m_timer.setInterval(6 * 60 * 60 * 1000);
    connect(&m_timer, &QTimer::timeout, this, [this] { if (autoUpdate()) checkNow(); });
    m_timer.start();
    QTimer::singleShot(5000, this, [this] { if (autoUpdate()) checkNow(); });
}

bool Updater::autoUpdate() const { return QSettings().value("autoUpdate", true).toBool(); }
void Updater::setAutoUpdate(bool on) {
    if (on == autoUpdate()) return;
    QSettings().setValue("autoUpdate", on); emit autoUpdateChanged();
}
void Updater::setStatus(const QString &s) { if (m_status != s) { m_status = s; emit statusChanged(); } }
void Updater::fail(const QString &why) { qWarning() << "update:" << why; if (m_part.isOpen()) m_part.close(); QFile::remove(m_part.fileName()); setStatus("error"); }

void Updater::checkNow() {
    if (m_status == "checking" || m_status == "downloading") return;
    if (qEnvironmentVariableIsEmpty("APPIMAGE")) { setStatus("unsupported"); return; } // only AppImage self-updates
    setStatus("checking");
    const QUrl api(qEnvironmentVariable("INKWELL_UPDATE_URL", kApi));
    QNetworkRequest rq(api);
    rq.setHeader(QNetworkRequest::UserAgentHeader, "Inkwell-updater");
    rq.setRawHeader("Accept", "application/vnd.github+json");
    rq.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    auto *r = m_nam.get(rq);
    connect(r, &QNetworkReply::finished, this, [this, r] {
        r->deleteLater();
        if (r->error() != QNetworkReply::NoError) return fail(r->errorString());
        const QJsonObject o = QJsonDocument::fromJson(r->readAll()).object();
        const QString remote = o["body"].toString().trimmed();
        if (remote.isEmpty() || remote == QLatin1String(INKWELL_BUILD)) return setStatus("idle");
        for (const QJsonValue &a : o["assets"].toArray()) {
            const QJsonObject ao = a.toObject();
            if (ao["name"].toString().endsWith(".AppImage")) return download(QUrl(ao["browser_download_url"].toString()));
        }
        fail("no AppImage asset in release");
    });
}

void Updater::download(const QUrl &url) {
    const QString target = qEnvironmentVariable("APPIMAGE");
    m_part.setFileName(target + ".part");
    if (!m_part.open(QIODevice::WriteOnly)) return fail("cannot write next to " + target);
    setStatus("downloading"); m_progress = 0; emit progressChanged();
    QNetworkRequest rq(url);
    rq.setHeader(QNetworkRequest::UserAgentHeader, "Inkwell-updater");
    rq.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    auto *r = m_nam.get(rq);
    connect(r, &QNetworkReply::readyRead, this, [this, r] { m_part.write(r->readAll()); });
    connect(r, &QNetworkReply::downloadProgress, this, [this](qint64 got, qint64 total) {
        if (total > 0) { m_progress = qreal(got) / total; emit progressChanged(); }
    });
    connect(r, &QNetworkReply::finished, this, [this, r, target] {
        r->deleteLater();
        if (r->error() != QNetworkReply::NoError) return fail(r->errorString());
        m_part.write(r->readAll()); m_part.close();
        QFile f(m_part.fileName());
        if (!f.open(QIODevice::ReadOnly) || f.size() < 1'000'000 || f.read(4) != QByteArray("\x7f" "ELF", 4)) return fail("downloaded file is not a valid AppImage");
        f.close();
        QFile::setPermissions(m_part.fileName(), QFileDevice::Permissions(0755));
        std::error_code ec;
        std::filesystem::rename(m_part.fileName().toStdString(), target.toStdString(), ec); // atomic replace
        if (ec) return fail(QString::fromStdString(ec.message()));
        setStatus("ready"); emit updated();
    });
}
