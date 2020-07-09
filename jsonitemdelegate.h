#ifndef JSONITEMDELEGATE_H
#define JSONITEMDELEGATE_H

#include <QObject>
#include <QItemDelegate>
#include <QLineEdit>
#include <QComboBox>

class JSONItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit JSONItemDelegate(QObject *parent = 0);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setDataEnum(QHash<int, QString> *dE);
    QHash<int, QString> *dataEnum;
    void setDataSources(QHash<int, QString> *dS);
    QHash<int, QString> *dataSources;
};

#endif // JSONITEMDELEGATE_H
