#ifndef _ImportSNA_H_
#define _ImportSNA_H_

#include <QFile>
#include <QObject>
#include <QVariant>

class ImportSna : public QObject {
    Q_OBJECT
  public:
    ImportSna(QObject *parent);

  public slots:
    QVariant readData(const QString& filename);

  private:

    QString readString();
    double readDouble();
    int readInt();
    bool readBool();
    void openFile();

    QFile  *m_file;
    QString m_filename;
};

#endif
