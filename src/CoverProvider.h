#pragma once
#include <QQuickImageProvider>
#include <QUrl>
#include <QPainter>
#include "Notebook.h"
#include "PageRenderer.h"

// image://cover/<percent-encoded path>?<mtime>  -> first page of a .inkwell file
class CoverProvider : public QQuickImageProvider {
public:
    CoverProvider() : QQuickImageProvider(QQuickImageProvider::Image) {}
    QImage requestImage(const QString &id, QSize *size, const QSize &req) override {
        const QString path = QUrl::fromPercentEncoding(id.section('?', 0, 0).toUtf8());
        const int w = req.width() > 0 ? req.width() : 240;
        const QSize out(w, qRound(w * 1123.0 / 794.0));
        QImage img(out, QImage::Format_ARGB32_Premultiplied);
        img.fill(Qt::white);
        if (auto page = Notebook::readFirstPage(path)) {
            QPainter p(&img);
            PageRenderer::render(&p, *page, out, Qt::white, QColor("#cfd6e4"));
        }
        if (size) *size = out;
        return img;
    }
};
