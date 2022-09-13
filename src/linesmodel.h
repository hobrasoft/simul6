/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _LinesModel_H_
#define _LinesModel_H_

#include <QStandardItemModel>

/**
 * @brief
 */
class LinesModel : public QStandardItemModel {
    Q_OBJECT
  public:
    explicit LinesModel(QObject *parent = nullptr);

    enum Columns {
        Color = 0,
        Position,
        LastCol
        };

};

#endif

