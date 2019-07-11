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

private:
};

#endif // PARAMETERSMODEL_H
