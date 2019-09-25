/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _About_H_
#define _About_H_

#include <QDialog>

namespace Ui {
class About;
}

class About : public QDialog {
    Q_OBJECT
public:
   ~About();
    About(QWidget *);

private:
    Ui::About *ui;    
};

#endif

