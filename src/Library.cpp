#include "Library.h"
#include <QSaveFile>
#include <QDataStream>
#include <QDir>
#include <QFileInfo>
#include <algorithm>

Library::Library(QObject *parent) : QAbstractListModel(parent) {
    m_storagePath = QDir::homePath() + "/.local/share/inkwell";
    QDir dir(m_storagePath);
    if (!dir.exists()) dir.mkpath(".");
    loadStorage();
    if (m_notebooks.isEmpty()) {
        newNotebook();
    } else {
        setCurrent(0);
    }
}

Library::~Library() {
    // Notebooks are owned elsewhere (QML or Notebook model)
}

void Library::loadStorage() {
    QString file = storageFile();
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly)) {
        m_notebooks.clear();
        m_current = {};
        return;
    }
    QDataStream ds(&f);
    ds.setVersion(QDataStream::Qt_6_0);
    char magic[4] = {};
    ds.readRawData(magic, 4);
    if (memcmp(magic, "LBRY", 4) != 0) {
        m_notebooks.clear();
        m_current = {};
        f.close();
        return;
    }
    quint32 version = 0;
    ds >> version;
    if (version != 1) {
        m_notebooks.clear();
        m_current = {};
        f.close();
        return;
    }
    quint32 count = 0;
    ds >> count;
    if (count > 100) {
        m_notebooks.clear();
        m_current = {};
        f.close();
        return;
    }
    m_notebooks.resize(count);
    for (quint32 i = 0; i < count; ++i) {
        ds >> m_notebooks[i].title >> m_notebooks[i].path >> m_notebooks[i].pageCount >> m_notebooks[i].modified;
    }
    quint32 curIdx = 0;
    ds >> curIdx;
    if (curIdx < count && m_notebooks.size() > 0) {
        m_current = m_notebooks[curIdx];
    } else if (!m_notebooks.isEmpty()) {
        m_current = m_notebooks.first();
    }
    f.close();
}

void Library::saveStorage() {
    QString file = storageFile();
    QSaveFile sf(file);
    if (!sf.open(QIODevice::WriteOnly)) {
        emit error(tr("Cannot write library storage to %1").arg(file));
        return;
    }
    QDataStream ds(&sf);
    ds.setVersion(QDataStream::Qt_6_0);
    ds.writeRawData("LBRY", 4);
    ds << quint32(1);
    ds << quint32(m_notebooks.size());
    for (const NotebookInfo &ni : m_notebooks) {
        ds << ni.title << ni.path << ni.pageCount << ni.modified;
    }
    int curIdx = -1;
    for (int i = 0; i < m_notebooks.size(); ++i) {
        if (m_notebooks[i].path == m_current.path && 
            m_notebooks[i].title == m_current.title) {
            curIdx = i;
            break;
        }
    }
    if (curIdx < 0 && !m_notebooks.isEmpty()) curIdx = 0;
    ds << quint32(curIdx >= 0 ? curIdx : 0);
    if (!sf.commit()) {
        emit error(tr("Failed to save library storage"));
        return;
    }
}

QString Library::storageFile() const {
    return m_storagePath + "/library.inkwell";
}

QList<NotebookInfo> Library::notebooks() const { return m_notebooks; }

NotebookInfo Library::current() const { return m_current; }

int Library::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return m_notebooks.size();
}

QVariant Library::data(const QModelIndex &idx, int role) const {
    if (!idx.isValid() || idx.row() >= m_notebooks.size()) return {};
    const NotebookInfo &ni = m_notebooks[idx.row()];
    switch (role) {
        case NotebookRole: return QVariant::fromValue(static_cast<QObject *>(nullptr));
        case TitleRole: return ni.title;
        case PathRole: return ni.path;
        case PageCountRole: return ni.pageCount;
        case ModifiedRole: return ni.modified;
        default: return {};
    }
}

QHash<int, QByteArray> Library::roleNames() const {
    return {
        {NotebookRole, "notebook"},
        {TitleRole, "title"},
        {PathRole, "path"},
        {PageCountRole, "pageCount"},
        {ModifiedRole, "modified"}
    };
}

int Library::addNotebook(const QString &title, const QString &afterTitle) {
    int at = m_notebooks.size(); // append by default
    if (!afterTitle.isEmpty()) {
        for (int i = 0; i < m_notebooks.size(); ++i) {
            if (m_notebooks[i].title == afterTitle) {
                at = i + 1;
                break;
            }
        }
    }
    
    beginInsertRows({}, at, at);
    
    NotebookInfo ni;
    ni.title = title;
    ni.path = QString();
    ni.pageCount = 0;
    ni.modified = false;
    
    m_notebooks.insert(at, ni);
    endInsertRows();
    emit notebooksChanged();
    saveStorage();
    return at;
}

int Library::addNotebookAfter(const QString &title, const QString &afterTitle) {
    return addNotebook(title, afterTitle);
}

bool Library::removeNotebook(int index) {
    if (index < 0 || index >= m_notebooks.size()) return false;
    if (m_notebooks.size() == 1) return false;
    
    beginRemoveRows({}, index, index);
    m_notebooks.removeAt(index);
    endRemoveRows();
    // Set current to previous or next notebook
    if (m_current.path == m_notebooks.value(index).path && 
        m_current.title == m_notebooks.value(index).title) {
        int newIdx = qMax(0, qMin(m_notebooks.size() - 1, index));
        m_current = m_notebooks[newIdx];
        emit currentChanged();
    }
    emit notebooksChanged();
    saveStorage();
    return true;
}

bool Library::renameNotebook(int index, const QString &newTitle) {
    if (index < 0 || index >= m_notebooks.size()) return false;
    m_notebooks[index].title = newTitle;
    emit notebooksChanged();
    saveStorage();
    return true;
}

void Library::setCurrent(int index) {
    if (index < 0 || index >= m_notebooks.size()) return;
    m_current = m_notebooks[index];
    emit currentChanged();
}

Notebook *Library::notebookAt(int index) {
    Q_UNUSED(index)
    return nullptr;
}

bool Library::loadNotebook(int index) {
    if (index < 0 || index >= m_notebooks.size()) return false;
    // The actual notebook loading is handled by the Notebook model
    // This just emits a signal that the current notebook changed
    setCurrent(index);
    return true;
}

bool Library::loadNotebook(const QString &title) {
    for (int i = 0; i < m_notebooks.size(); ++i) {
        if (m_notebooks[i].title == title) {
            return loadNotebook(i);
        }
    }
    emit error(tr("Notebook '%1' not found").arg(title));
    return false;
}

void Library::newNotebook() {
    addNotebook(tr("Untitled"));
    // The QML will create the actual Notebook instance
}

void Library::undo() {
    // Could implement global undo across notebooks
}

void Library::redo() {
    // Could implement global redo across notebooks
}

bool Library::saveCurrent() {
    saveStorage();
    return true;
}
