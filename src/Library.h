#pragma once
#include <QAbstractListModel>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QUrl>
#include <QDateTime>
#include <QFileInfo>
#include <QtQml/qqmlregistration.h>

// Folder/notebook browser over ~/.local/share/inkwell/notebooks (folders = directories, notebooks = *.inkwell).
class Library : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)   // non-empty => recursive title search
    Q_PROPERTY(QString breadcrumb READ breadcrumb NOTIFY locationChanged)
    Q_PROPERTY(bool canGoUp READ canGoUp NOTIFY locationChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Roles { NameRole = Qt::UserRole + 1, PathRole, IsFolderRole, ModifiedTextRole, ModifiedMsRole, ItemCountRole, LocationRole };
    explicit Library(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = {}) const override { return parent.isValid() ? 0 : m_items.size(); }
    QVariant data(const QModelIndex &idx, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString query() const { return m_query; }
    void setQuery(const QString &q);
    QString breadcrumb() const;
    bool canGoUp() const { return m_dir != m_root; }
    int count() const { return m_items.size(); }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void enter(const QString &folderPath);
    Q_INVOKABLE void up();
    Q_INVOKABLE QString createNotebook(const QString &name = QString()); // returns path
    Q_INVOKABLE bool createFolder(const QString &name);
    Q_INVOKABLE bool rename(const QString &path, const QString &newName);
    Q_INVOKABLE bool duplicate(const QString &path);
    Q_INVOKABLE bool remove(const QString &path);
    Q_INVOKABLE QUrl fileUrl(const QString &path) const { return QUrl::fromLocalFile(path); }
signals:
    void queryChanged();
    void locationChanged();
    void countChanged();
    void error(const QString &message);
private:
    struct Entry { QString name, path, location; bool isFolder; QDateTime modified; int itemCount; };
    Entry makeEntry(const QFileInfo &fi, bool withLocation) const;
    QString uniquePath(const QString &dir, const QString &base, const QString &ext) const;
    static QString sanitize(const QString &name);
    bool insideRoot(const QString &path) const;
    QList<Entry> m_items;
    QString m_root, m_dir, m_query;
    QFileSystemWatcher m_watch;
    QTimer m_debounce;
};
