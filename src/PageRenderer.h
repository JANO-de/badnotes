#pragma once
#include <QPainter>
#include <QColor>
#include <QSizeF>
#include "InkPage.h"

namespace PageRenderer {
// Paints paper + template + strokes into a device area of `deviceSize`, scaled from page units.
void render(QPainter *p, const InkPage &page, const QSizeF &deviceSize, const QColor &paper, const QColor &line);
}
