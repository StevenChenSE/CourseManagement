#include "logindialog.h"
#include "ui_logindialog.h"
#include "teacherwindow.h"
#include "adminwindow.h"
LoginDialog::LoginDialog(QWidget *parent, Mode md) :
    QDialog(parent),mode(md),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    switch (mode) {
    case admin:
        setWindowTitle("管理员登陆");
        break;
    case teacher:
        setWindowTitle("教师登陆");
        break;
    case student:
        setWindowTitle("学生登陆");
        break;
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}


void LoginDialog::on_buttonBox_accepted()
{
    QSqlTableModel checkPass;
    QSqlRecord record;
    switch (mode) {
    case teacher:
    {
        checkPass.setTable("teacher");
        checkPass.setFilter("teacherno='"+ui->id->text()+"'");
        checkPass.select();
    }
        break;
    case student:
    {
        checkPass.setTable("student");
        checkPass.setFilter("studentid='"+ui->id->text()+"'");
        checkPass.select();
    }
        break;
    case admin:
    {
        checkPass.setTable("administrator");
        checkPass.setFilter("adminid='"+ui->id->text()+"'");
        checkPass.select();
    }
        break;
    }
    record=checkPass.record(0);
    if(ui->password->text()!=record.value("passwd").toString())
    {
        qDebug()<<ui->password->text();
        qDebug()<<record.value("passwd").toString();
        QMessageBox::warning(this,tr("错误"),tr("密码错误或账号不存在!"));
        emit reject();
    }
    else
    {
        switch (mode) {
        case teacher:
        {
            TeacherWindow* tw=new TeacherWindow(NULL,ui->id->text());
            this->hide();
            tw->show();
            break;
        }
        case admin:
        {
            AdminWindow* adw=new AdminWindow(NULL);
            this->hide();
            adw->show();
            break;
        }
        }
    }
}
