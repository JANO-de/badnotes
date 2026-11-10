#include "Library.h"
#include "Notebook.h"
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QLocale>
#include <QStandardPaths>
#include <algorithm>

Library::Library(QObject *parent) : QAbstractListModel(parent) {
    m_root = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/inkwell/notebooks");
    QDir().mkpath(m_root);
    m_dir = m_root;
    m_debounce.setSingleShot(true); m_debounce.setInterval(300);
    connect(&m_debounce, &QTimer::timeout, this, &Library::refresh);
    connect(&m_watch, &QFileSystemWatcher::directoryChanged, this, [this] { m_debounce.start(); });
    refresh();
}

QHash<int, QByteArray> Library::roleNames() const {
    return {{NameRole, "name"}, {PathRole, "path"}, {IsFolderRole, "isFolder"}, {ModifiedTextRole, "modifiedText"},
            {ModifiedMsRole, "modifiedMs"}, {ItemCountRole, "itemCount"}, {LocationRole, "location"}};
}

QVariant Library::data(const QModelIndex &idx, int role) const {
    if (!idx.isValid() || idx.row() >= m_items.size()) return {};
    const Entry &e = m_items[idx.row()];
    switch (role) {
    case NameRole: return e.name;
    case PathRole: return e.path;
    case IsFolderRole: return e.isFolder;
    case ModifiedTextRole: return QLocale().toString(e.modified, QLocale::ShortFormat);
    case ModifiedMsRole: return e.modified.toMSecsSinceEpoch();
    case ItemCountRole: return e.itemCount;
    case LocationRole: return e.location;
    }
    return {};
}

Library::Entry Library::makeEntry(const QFileInfo &fi, bool withLocation) const {
    Entry e;
    e.isFolder = fi.isDir();
    e.name = e.isFolder ? fi.fileName() : fi.completeBaseName();
    e.path = fi.absoluteFilePath();
    e.modified = fi.lastModified();
    e.itemCount = 0;
    if (e.isFolder) e.itemCount = QDir(e.path).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot).size()
                               + QDir(e.path).entryList(QStringList{"*.inkwell"}, QDir::Files).size();
    if (withLocation) {
        const QString rel = QDir(m_root).relativeFilePath(fi.absolutePath());
        e.location = rel == "." ? QString() : rel;
    }
    return e;
}

void Library::refresh() {
    beginResetModel();
    m_items.clear();
    const QString q = m_query.trimmed();
    if (!q.isEmpty()) {
        QDirIterator it(m_root, {"*.inkwell"}, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            if (it.fileInfo().completeBaseName().contains(q, Qt::CaseInsensitive)) m_items.append(makeEntry(it.fileInfo(), true));
        }
    } else {
        const QDir d(m_dir);
        for (const QFileInfo &fi : d.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) m_items.append(makeEntry(fi, false));
        for (const QFileInfo &fi : d.entryInfoList({"*.inkwell"}, QDir::Files)) m_items.append(makeEntry(fi, false));
    }
    std::sort(m_items.begin(), m_items.end(), [](const Entry &a, const Entry &b) {
        if (a.isFolder != b.isFolder) return a.isFolder;
        if (a.isFolder) return a.name.compare(b.name, Qt::CaseInsensitive) < 0;
        return a.modified > b.modified;
    });
    endResetModel();
    if (!m_watch.directories().isEmpty()) m_watch.removePaths(m_watch.directories());
    m_watch.addPath(m_dir);
    emit countChanged();
}

void Library::setQuery(const QString &q) {
    if (q == m_query) return;
    m_query = q; emit queryChanged(); refresh();
}

QString Library::breadcrumb() const {
    QString rel = QDir(m_root).relativeFilePath(m_dir);
    return rel == "." ? tr("Library") : tr("Library") + " / " + rel.replace('/', " / ");
}

void Library::enter(const QString &folderPath) {
    if (!insideRoot(folderPath) || !QFileInfo(folderPath).isDir()) return;
    m_dir = QDir::cleanPath(folderPath);
    emit locationChanged(); refresh();
}
void Library::up() {
    if (m_dir == m_root) return;
    m_dir = QFileInfo(m_dir).absolutePath();
    emit locationChanged(); refresh();
}

bool Library::insideRoot(const QString &path) const {
    const QString p = QDir::cleanPath(QFileInfo(path).absoluteFilePath());
    return p == m_root || p.startsWith(m_root + "/");
}

QString Library::sanitize(const QString &name) {
    QString n = name.trimmed();
    for (QChar &c : n) if (QStringLiteral("/\\:%?*\"<>|").contains(c)) c = '_';
    while (n.startsWith('.')) n.remove(0, 1);
    return n;
}

QString Library::uniquePath(const QString &dir, const QString &base, const QString &ext) const {
    QString p = dir + "/" + base + ext;
    for (int i = 2; QFileInfo::exists(p); ++i) p = dir + "/" + base + " " + QString::number(i) + ext;
    return p;
}

QString Library::createNotebook(const QString &name) {
    QString base = sanitize(name);
    if (base.isEmpty()) base = tr("Untitled");
    const QString path = uniquePath(m_dir, base, ".inkwell");
    Notebook nb;
    if (!nb.save(QUrl::fromLocalFile(path))) { emit error(tr("Cannot create notebook")); return {}; }
    refresh();
    return path;
}

bool Library::createFolder(const QString &name) {
    QString base = sanitize(name);
    if (base.isEmpty()) base = tr("New folder");
    if (!QDir().mkdir(uniquePath(m_dir, base, QString()))) { emit error(tr("Cannot create folder")); return false; }
    refresh(); return true;
}

bool Library::rename(const QString &path, const QString &newName) {
    if (!insideRoot(path) || path == m_root) return false;
    const QString base = sanitize(newName);
    if (base.isEmpty()) return false;
    const QFileInfo fi(path);
    const QString target = fi.absolutePath() + "/" + base + (fi.isDir() ? QString() : ".inkwell");
    if (target == fi.absoluteFilePath()) return true;
    if (QFileInfo::exists(target)) { emit error(tr("“%1” already exists").arg(base)); return false; }
    if (!QDir().rename(path, target)) { emit error(tr("Rename failed")); return false; }
    refresh(); return true;
}

bool Library::duplicate(const QString &path) {
    const QFileInfo fi(path);
    if (!insideRoot(path) || !fi.isFile()) return false;
    const QString target = uniquePath(fi.absolutePath(), fi.completeBaseName() + tr(" copy"), ".inkwell");
    if (!QFile::copy(path, target)) { emit error(tr("Duplicate failed")); return false; }
    refresh(); return true;
}

bool Library::remove(const QString &path) {
    if (!insideRoot(path) || QDir::cleanPath(path) == m_root) return false;
    const QFileInfo fi(path);
    const bool ok = fi.isDir() ? QDir(path).removeRecursively() : QFile::remove(path);
    if (!ok) emit error(tr("Delete failed"));
    refresh(); return ok;
}
