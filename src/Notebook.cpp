#include "Notebook.h"
#include <QSaveFile>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>

Notebook::Notebook(QObject *parent) : QAbstractListModel(parent) {
    m_autosave.setSingleShot(true);
    m_autosave.setInterval(3000);
    connect(&m_autosave, &QTimer::timeout, this, [this] { if (!m_path.isEmpty() && m_modified) saveTo(m_path); });
    m_pages.append(makePage("lined"));
}

InkPage *Notebook::makePage(const QString &tmpl) {
    auto *p = new InkPage(this);
    p->setPageTemplate(tmpl);
    connect(p, &InkPage::edited, this, &Notebook::onEdited);
    return p;
}

QVariant Notebook::data(const QModelIndex &idx, int role) const {
    if (!idx.isValid() || idx.row() >= m_pages.size() || role != PageRole) return {};
    return QVariant::fromValue(static_cast<QObject *>(m_pages[idx.row()]));
}

QString Notebook::title() const {
    QString n = m_path.isEmpty() ? tr("Untitled") : QFileInfo(m_path).completeBaseName();
    return n + (m_modified ? " •" : "");
}

void Notebook::setModified(bool m) {
    if (m_modified == m) { if (m) m_autosave.start(); return; }
    m_modified = m; emit modifiedChanged(); emit titleChanged();
    if (m) m_autosave.start();
}

void Notebook::onEdited(InkPage *p) {
    m_undo.append(p); m_redo.clear();
    int i = m_pages.indexOf(p);
    if (i >= 0 && i != m_current) { m_current = i; emit currentIndexChanged(); }
    emit historyChanged();
    setModified(true);
}

void Notebook::undo() {
    if (m_undo.isEmpty()) return;
    InkPage *p = m_undo.takeLast();
    if (p->undo()) m_redo.append(p);
    emit historyChanged(); setModified(true);
}
void Notebook::redo() {
    if (m_redo.isEmpty()) return;
    InkPage *p = m_redo.takeLast();
    if (p->redo()) m_undo.append(p);
    emit historyChanged(); setModified(true);
}

int Notebook::addPage(int after) {
    if (after < 0 || after >= m_pages.size()) after = m_pages.size() - 1;
    const int at = after + 1;
    const QString tmpl = m_pages.isEmpty() ? "lined" : m_pages[after]->pageTemplate();
    beginInsertRows({}, at, at);
    m_pages.insert(at, makePage(tmpl));
    endInsertRows();
    emit countChanged(); setModified(true);
    return at;
}

void Notebook::removePage(int i) {
    if (i < 0 || i >= m_pages.size()) return;
    if (m_pages.size() == 1) addPage(0); // never leave the notebook empty
    InkPage *p = m_pages[i];
    beginRemoveRows({}, i, i);
    m_pages.removeAt(i);
    endRemoveRows();
    m_undo.removeAll(p); m_redo.removeAll(p);
    p->deleteLater();
    m_current = qBound(0, m_current, m_pages.size() - 1);
    emit countChanged(); emit historyChanged(); emit currentIndexChanged(); setModified(true);
}

void Notebook::movePage(int from, int to) {
    if (from < 0 || from >= m_pages.size() || to < 0 || to >= m_pages.size() || from == to) return;
    beginMoveRows({}, from, from, {}, to > from ? to + 1 : to);
    m_pages.move(from, to);
    endMoveRows();
    setModified(true);
}

void Notebook::clearHistory() { m_undo.clear(); m_redo.clear(); emit historyChanged(); }

void Notebook::newNotebook() {
    beginResetModel();
    qDeleteAll(m_pages); m_pages.clear();
    m_pages.append(makePage("lined"));
    endResetModel();
    m_path.clear(); m_current = 0; clearHistory();
    m_modified = false;
    emit countChanged(); emit fileChanged(); emit modifiedChanged(); emit titleChanged(); emit currentIndexChanged();
}

// ---- persistence (single binary file, format v1) ----
bool Notebook::save(const QUrl &url) {
    QString path = url.isLocalFile() ? url.toLocalFile() : url.toString();
    if (!path.endsWith(".inkwell")) path += ".inkwell";
    return saveTo(path);
}
bool Notebook::saveCurrent() { return !m_path.isEmpty() && saveTo(m_path); }

bool Notebook::saveTo(const QString &path) {
    QSaveFile f(path);
    if (!f.open(QIODevice::WriteOnly)) { emit error(tr("Cannot write %1").arg(path)); return false; }
    QDataStream ds(&f);
    ds.setVersion(QDataStream::Qt_6_0);
    ds.writeRawData("INKW", 4);
    ds << quint32(1) << quint32(m_pages.size());
    for (InkPage *p : std::as_const(m_pages)) p->write(ds);
    if (ds.status() != QDataStream::Ok || !f.commit()) { emit error(tr("Save failed: %1").arg(path)); return false; }
    if (m_path != path) { m_path = path; emit fileChanged(); }
    m_modified = false; emit modifiedChanged(); emit titleChanged();
    return true;
}

bool Notebook::load(const QUrl &url) {
    const QString path = url.isLocalFile() ? url.toLocalFile() : url.toString();
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) { emit error(tr("Cannot open %1").arg(path)); return false; }
    QDataStream ds(&f);
    ds.setVersion(QDataStream::Qt_6_0);
    char magic[4] = {};
    quint32 ver = 0, n = 0;
    ds.readRawData(magic, 4);
    ds >> ver >> n;
    if (memcmp(magic, "INKW", 4) != 0 || ver != 1 || n == 0 || n > 100000) { emit error(tr("Not a valid Inkwell file")); return false; }
    QList<InkPage *> pages;
    for (quint32 i = 0; i < n; ++i) {
        InkPage *p = makePage("lined");
        pages.append(p);
        if (!p->read(ds)) { qDeleteAll(pages); emit error(tr("File is corrupted")); return false; }
    }
    beginResetModel();
    qDeleteAll(m_pages);
    m_pages = pages;
    endResetModel();
    m_path = path; m_current = 0; clearHistory();
    m_modified = false;
    emit countChanged(); emit fileChanged(); emit modifiedChanged(); emit titleChanged(); emit currentIndexChanged();
    return true;
}

std::unique_ptr<InkPage> Notebook::readFirstPage(const QString &path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return nullptr;
    QDataStream ds(&f);
    ds.setVersion(QDataStream::Qt_6_0);
    char magic[4] = {};
    quint32 ver = 0, n = 0;
    ds.readRawData(magic, 4);
    ds >> ver >> n;
    if (memcmp(magic, "INKW", 4) != 0 || ver != 1 || n == 0) return nullptr;
    auto p = std::make_unique<InkPage>();
    return p->read(ds) ? std::move(p) : nullptr;
}
