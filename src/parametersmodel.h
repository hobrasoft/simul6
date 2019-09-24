#ifndef PARAMETERSMODEL_H
#define PARAMETERSMODEL_H

#include <QStandardItemModel>

class ParametersModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit ParametersModel(QObject *parent = nullptr);

    enum Rows {
        pKa = 0,
        U,
        LastRow
    };

    enum Columns {
        N3=0, N2, N1, P1, P2, P3
    };

private:
};

#endif // PARAMETERSMODEL_H
