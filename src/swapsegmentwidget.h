#ifndef _SwapSegmentProfile_H_
#define _SwapSegmentProfile_H_

#include <QWidget>
#include <QThread>
#include <QPaintEvent>
#include <QList>

namespace Ui {
class SwapSegmentWidget;
}

class SwapSegmentWidget : public QWidget {
    Q_OBJECT
  public:
    SwapSegmentWidget(QWidget *parent = nullptr);
   ~SwapSegmentWidget();

    QList<int> ratios() const;
    void setRatios(const QList<int>&);

  public slots:

  private slots:
    void    recalculate();

  signals:
    void    segmentsChanged();

  private:
    Ui::SwapSegmentWidget *ui;

};

#endif // _SwapSegmentProfile_H_
