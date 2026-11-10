#pragma once
#include <QAbstractListModel>
#include <QTimer>
#include <QUrl>
#include <QtQml/qqmlregistration.h>
#include "InkPage.h"
#include <memory>

class Notebook : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int pageCount READ pageCount NOTIFY countChanged)
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged)  // last edited page
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY historyChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY historyChanged)
    Q_PROPERTY(QString filePath READ filePath NOTIFY fileChanged)
    Q_PROPERTY(bool modified READ modified NOTIFY modifiedChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
public:
    enum Roles { PageRole = Qt::UserRole + 1 };
    explicit Notebook(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = {}) const override { return parent.isValid() ? 0 : m_pages.size(); }
    QVariant data(const QModelIndex &idx, int role) const override;
    QHash<int, QByteArray> roleNames() const override { return {{PageRole, "page"}}; }

    int pageCount() const { return m_pages.size(); }
    int currentIndex() const { return m_current; }
    bool canUndo() const { return !m_undo.isEmpty(); }
    bool canRedo() const { return !m_redo.isEmpty(); }
    QString filePath() const { return m_path; }
    bool modified() const { return m_modified; }
    QString title() const;

    Q_INVOKABLE InkPage *pageAt(int i) const { return (i >= 0 && i < m_pages.size()) ? m_pages[i] : nullptr; }
    Q_INVOKABLE int addPage(int after = -1);            // returns new index
    Q_INVOKABLE void removePage(int i);
    Q_INVOKABLE void movePage(int from, int to);
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
    Q_INVOKABLE void newNotebook();
    Q_INVOKABLE bool save(const QUrl &url);
    Q_INVOKABLE bool saveCurrent();
    Q_INVOKABLE bool load(const QUrl &url);
    static std::unique_ptr<InkPage> readFirstPage(const QString &path); // for library covers
signals:
    void countChanged();
    void currentIndexChanged();
    void historyChanged();
    void fileChanged();
    void modifiedChanged();
    void titleChanged();
    void error(const QString &message);
private:
    InkPage *makePage(const QString &tmpl);
    void onEdited(InkPage *p);
    void setModified(bool m);
    bool saveTo(const QString &path);
    void clearHistory();
    QList<InkPage *> m_pages;
    QList<InkPage *> m_undo, m_redo; // global order of edits across pages
    int m_current = 0;
    QString m_path;
    bool m_modified = false;
    QTimer m_autosave;
};
