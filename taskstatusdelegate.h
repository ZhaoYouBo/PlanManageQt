#ifndef TASKSTATUSDELEGATE_H
#define TASKSTATUSDELEGATE_H
#include <QStyledItemDelegate>

class TaskStatusDelegate : public QStyledItemDelegate
{
public:
    TaskStatusDelegate(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // TASKSTATUSDELEGATE_H
