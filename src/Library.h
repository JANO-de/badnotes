#pragma once
#include <QAbstractListModel>
#include <QFile>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <vector>
#include <string>
#include "Notebook.h"

struct NotebookInfo {
    QString title;
    QString path;
    int pageCount = 0;
    bool modified = false;
};

class Library : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QList<NotebookInfo> notebooks READ notebooks NOTIFY notebooksChanged)
    Q_PROPERTY(NotebookInfo current READ current NOTIFY currentChanged)
public:
    enum Roles {
        NotebookRole = Qt::UserRole + 1,
        TitleRole,
        PathRole,
        PageCountRole,
        ModifiedRole
    };

    explicit Library(QObject *parent = nullptr);
    ~Library() override;

    QList<NotebookInfo> notebooks() const;
    NotebookInfo current() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE int addNotebook(const QString &title, const QString &after = "");
    Q_INVOKABLE int addNotebookAfter(const QString &title, const QString &afterTitle);
    Q_INVOKABLE bool removeNotebook(int index);
    Q_INVOKABLE bool renameNotebook(int index, const QString &newTitle);
    Q_INVOKABLE Notebook *notebookAt(int index);
    Q_INVOKABLE void setCurrent(int index);
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();

    Q_INVOKABLE bool saveCurrent();
    Q_INVOKABLE bool loadNotebook(int index);
    Q_INVOKABLE bool loadNotebook(const QString &title);
    Q_INVOKABLE void newNotebook();

    QString storagePath() const { return m_storagePath; }

signals:
    void notebooksChanged();
    void currentChanged();
    void error(const QString &message);

private:
    QList<NotebookInfo> m_notebooks;
    NotebookInfo m_current;
    QList<Notebook *> m_ownedNotebooks;
    QString m_storagePath;
    QTimer m_autosave;

    void loadStorage();
    void saveStorage();
    QString storageFile() const;
    Notebook *findNotebook(const QString &title) const;
};

struct FolderInfo {
    QString title;
    QList<int> notebookIndices; // indices into m_notebooks
};