#ifndef INITCOURSEDIALOG_H
#define INITCOURSEDIALOG_H

#include "qtheaders.h"
namespace Ui {
class initCourseDialog;
}

class initCourseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit initCourseDialog(QWidget *parent = 0,const QString & str="");
    ~initCourseDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::initCourseDialog *ui;
    QString applyno,teacherno;
    QString getCurrentIndex();
};

#endif // INITCOURSEDIALOG_H
