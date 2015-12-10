#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode{admin,teacher,student}mode;
    explicit LoginDialog(QWidget *parent,Mode md);
    ~LoginDialog();

private slots:
    void on_buttonBox_accepted();


private:
    Ui::LoginDialog *ui;
    bool createConnection();
};

#endif // LOGINDIALOG_H
