#ifndef CONSTITUENTSMODEL_H
#define CONSTITUENTSMODEL_H

#include <QStandardItemModel>

class ConstituentsModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit ConstituentsModel(QObject *parent = nullptr);
    enum Columns {
        Id = 0,
        Name,
        NPka3, NPka2, NPka1,
        PPka1, PPka2, PPka3,
        NU3, NU2, NU1,
        PU1, PU2, PU3,
        LastCol
    };

signals:
    void loaded();

private slots:
    void loadData();

private:

    void createColumns();
};

#endif // CONSTITUENTSMODEL_H
