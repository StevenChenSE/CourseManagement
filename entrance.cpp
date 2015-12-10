#include "entrance.h"
#include "ui_entrance.h"
#include "logindialog.h"
#include <QMessageBox>
#include <QSqlError>
Entrance::Entrance(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Entrance)
{
    ui->setupUi(this);
    if(!createConnection())
        emit reject();
}

Entrance::~Entrance()
{
    delete ui;
}

void Entrance::on_studentButton_clicked()
{
    LoginDialog* loginDialog=new LoginDialog(this,LoginDialog::student);
        hide();
    if(!loginDialog->exec())
        show();
    else
        emit accept();
}

void Entrance::on_teacherButton_clicked()
{
    LoginDialog* loginDialog=new LoginDialog(this,LoginDialog::teacher);
        hide();
    if(!loginDialog->exec())
        show();
    else
        emit accept();
}

void Entrance::on_adminButton_clicked()
{
    LoginDialog* loginDialog=new LoginDialog(this,LoginDialog::admin);
        hide();
    if(!loginDialog->exec())
        show();
    else
        emit accept();
}
bool Entrance::createConnection()
{
    db=QSqlDatabase::addDatabase("QOCI");
    db.setHostName("166.111.71.220");
    db.setDatabaseName("dbta");
    db.setUserName("s2013010225");
    db.setPassword("8380438");
    if(!db.open())
    {
        QMessageBox::critical(this,"错误",tr("数据库无法打开！")
                              +db.lastError().text());
        return false;
    }
    return true;
}
