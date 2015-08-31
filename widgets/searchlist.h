#ifndef SEARCHLIST_H
#define SEARCHLIST_H

#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QPointer>
#include <QScrollArea>
#include <QDebug>

#include <libdui/dscrollarea.h>
#include <libdui/libdui_global.h>

#include "private/searchdbus.h"

DUI_USE_NAMESPACE

class SearchList;
class SearchItem
{
public:
    virtual ~SearchItem(){}
    virtual QStringList keyWords() const{return QStringList();}
    virtual void setData(const QVariant &){}
    virtual QVariant getData(){return QVariant();}
    virtual QWidget *widget() const = 0;

    SearchList *listWidget() const;
    virtual void setListWidget(SearchList* list);

protected:
    QPointer<SearchList> m_list;
};

class SearchList : public DScrollArea
{
    Q_OBJECT

    Q_PROPERTY(int checkedItem READ checkedItem WRITE setCheckedItem NOTIFY checkedItemChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int visibleCount READ visibleCount NOTIFY visibleCountChanged FINAL)
    Q_PROPERTY(bool checkable READ checkable WRITE setCheckable NOTIFY checkableChanged)
    Q_PROPERTY(QString keyWord READ keyWord WRITE setKeyWord NOTIFY keyWordChanged)

public:
    explicit SearchList(QWidget *parent = 0);

public slots:
    int addItem(SearchItem *data);
    void addItems(const QList<SearchItem*> &datas);
    void insertItem(int index, SearchItem *data);
    void insertItems(int index, const QList<SearchItem*> &datas);
    void setItemData(int index, const QVariant &data);
    void setItemSize(int w, int h);
    void clear();
    void removeItem(int index);
    void beginSearch();
    void endSearch();
    void setKeyWord(const QString &keyWord);
    void showItem(int index);
    void hideItem(int index);
    void setCheckedItem(int checkedItem);
    void setCheckable(bool checkable);
    void updateKeyWords();

public:
    int count() const;
    QVariant getItemData(int index) const;
    SearchItem *getItem(int index) const;
    int indexOf(SearchItem *item) const;
    int indexOf(QWidget *widget) const;
    bool isSearching() const;
    int checkedItem() const;
    bool checkable() const;
    int visibleCount() const;
    QString keyWord() const;

signals:
    void itemDataChanged(int index, const QVariant &data);
    void checkedItemChanged(int index);
    void countChanged();
    void checkableChanged(bool checkable);
    void visibleCountChanged(int visibleCount);
    void keyWordChanged(QString keyWord);

protected:
    bool eventFilter(QObject *, QEvent *) Q_DECL_OVERRIDE;

private:
    int m_itemWidth;
    int m_itemHeight;
    QVBoxLayout *m_layout;
    QList<SearchItem*> m_itemList;
    SearchDbus *m_dbus;
    QStringList m_keyWords;//all item keyword list
    QString m_dbusKey;
    bool m_searching;
    int m_checkedItem;
    bool m_checkable;
    QWidget *m_mainWidget;
    int m_visibleCount;
    QString m_keyWord;//current search keyword
    QStringList m_dbusKeyWords;//dbus return a list of keywords based on search keywords

    void setVisibleCount(int count);
};

#endif // SEARCHLIST_H
