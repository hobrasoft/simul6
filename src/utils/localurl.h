/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _LocalUrl_H_
#define _LocalUrl_H_

#include <QUrl>
#include <QObject>
#include <QWidget>

/**
 * @brief
 */
class LocalUrl : public QObject {
    Q_OBJECT
  public:
    LocalUrl(QObject *parent = nullptr);

  public slots:
    void showUrl(const QUrl &url);

};

#endif

