#ifndef DATABASEVIEW_H
#define DATABASEVIEW_H

#include <QTreeView>

class DatabaseView : public QTreeView
{
    Q_OBJECT

public:
    explicit DatabaseView(QWidget *parent = nullptr);

signals:
   void currentRowChanged(int);

private:
   void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;
};

#endif // DATABASEVIEW_H
