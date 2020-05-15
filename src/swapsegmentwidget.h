#ifndef _SwapSegmentProfile_H_
#define _SwapSegmentProfile_H_

#include <QWidget>
#include <QThread>
#include <QPaintEvent>

namespace Ui {
class SwapSegmentWidget;
}

class SwapSegmentWidget : public QWidget {
    Q_OBJECT
  public:
    SwapSegmentWidget(QWidget *parent = nullptr);
   ~SwapSegmentWidget();

  public slots:

  private slots:
    void    recalculate();

  signals:

  private:
    Ui::SwapSegmentWidget *ui;

};

#endif // _SwapSegmentProfile_H_
