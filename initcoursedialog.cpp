#include "initcoursedialog.h"
#include "ui_initcoursedialog.h"

initCourseDialog::initCourseDialog(QWidget *parent, const QString &str) :
    QDialog(parent),
    ui(new Ui::initCourseDialog),applyno(str)
{
    ui->setupUi(this);
    ui->cappacityLineEdit->setValidator(new QIntValidator(1,1000,this));
    ui->creditLineEdit->setValidator(new QIntValidator(1,30,this));
    QSqlQuery query(tr("select coursename,courseproperty,courselength,teachername,department,teacher.teacherno from teacher,"
                       " courseapply where status='待审核' and "
                       " teacher.teacherno=courseapply.teacherno"
                       " and applyno='%1'").arg(applyno));
    if(!query.isActive())
    {
        qDebug()<<query.lastError();
        qDebug()<<applyno;
        QMessageBox::critical(this,tr("错误"),tr("SQL查询失败，请重试或联系管理员！"));
    }
    if(query.next())
    {
        ui->courseNoLineEdit->setText(getCurrentIndex());
        ui->courseNameLineEdit->setText(query.value(0).toString());
        ui->coursePropertyLineEdit->setText(query.value(1).toString());
        ui->courseLengthLineEdit->setText(query.value(2).toString());
        ui->teacherNameLineEdit->setText(query.value(3).toString());
        ui->departmentLineEdit->setText(query.value(4).toString());
        teacherno=query.value(5).toString();
    }
    else
        QMessageBox::critical(this,tr("错误"),tr("SQL查询失败，请重试或联系管理员！"));
}

initCourseDialog::~initCourseDialog()
{
    delete ui;
}

void initCourseDialog::on_buttonBox_accepted()
{
    if(ui->creditLineEdit->text().isEmpty()||
            ui->classroomLineEdit->text().isEmpty()||
            ui->cappacityLineEdit->text().isEmpty()
            )
    {
        QMessageBox::warning(this,tr("错误"),tr("请完善所有信息再提交！"));
        return;
    }
    if(QMessageBox::warning(this,tr("提示"),tr("是否确定提交，提交后无法恢复！"),
                            QMessageBox::Yes|QMessageBox::No)==QMessageBox::No)
        return;
    QString str=tr("insert into course values('%1','%2%3',%4,'%5',%6,%7,'%8')")
                  .arg(ui->courseNoLineEdit->text())
                  .arg(ui->weekComboBox->currentText())
                  .arg(ui->timeComboBox->currentText())
                  .arg(ui->creditLineEdit->text())
                  .arg(ui->classroomLineEdit->text())
                  .arg(ui->cappacityLineEdit->text())
                  .arg(0)
                  .arg(this->applyno);
    qDebug()<<str;
    QSqlQuery query;
    query.prepare(str);
    if(!query.exec())
    {
        qDebug()<<query.lastError();
        QMessageBox::critical(this,tr("错误"),tr("提交课程失败，请重试或联系管理员！"));
    }
    else
    {
        query.prepare(tr("update courseapply set status='已通过' where applyno='%1'").arg(applyno));
        if(!query.exec())
            QMessageBox::critical(this,tr("错误"),tr("数据库更新失败，请联系管理员！"));

    }
}

QString initCourseDialog::getCurrentIndex()
{
    QSqlQuery query("select courseno from course order by courseno");
    if(query.isActive())
    {
        int currentIndex=query.last()?query.value(0).toString().remove(0,1).toInt()+1:1;
        QString str=tr("C%1").arg(QString::number(currentIndex));
        while(str.count()<6)
            str.insert(1,tr("0"));
        return str;
    }
    else
    {
        qDebug()<<query.lastError();
        QMessageBox::critical(this,tr("错误"),tr("SQL查询失败，请重试或联系管理员！"));
        return QString();
    }


}
