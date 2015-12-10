#include "teacherwindow.h"
#include "ui_teacherwindow.h"
TeacherWindow::TeacherWindow(QWidget *parent, const QString &teacherId) :
    QMainWindow(parent),
    ui(new Ui::TeacherWindow),teacherID(teacherId)
{
    ui->setupUi(this);
    initInfo();
    initCourseApply();
    connect(ui->listWidget,&QListWidget::currentRowChanged,
            ui->stackedWidget,&QStackedWidget::setCurrentIndex);
}

TeacherWindow::~TeacherWindow()
{
    delete ui;
}

void TeacherWindow::initInfo()
{
    teacherModel=new QSqlTableModel(this);
    teacherModel->setTable("TEACHER");
    teacherModel->setFilter(tr("TEACHERNO='%1'").arg(teacherID));
    teacherModel->select();
    mapper=new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setModel(teacherModel);
    mapper->addMapping(ui->teacherNoLineEdit,teacher_no);
    mapper->addMapping(ui->teacherNameLineEdit,teacher_name);
    mapper->addMapping(ui->sexLineEdit,teacher_sex);
    mapper->addMapping(ui->departmentLineEdit,teacher_department);
    mapper->addMapping(ui->emailLineEdit,teacher_email);
    mapper->addMapping(ui->personalInfoTextEdit,teacher_info);
    mapper->setCurrentIndex(0);
    ui->savePushButton->setEnabled(false);
    connect(ui->emailLineEdit,&QLineEdit::editingFinished,
            this,&TeacherWindow::personalInfoChanged);
    connect(ui->personalInfoTextEdit,&QPlainTextEdit::textChanged,
            this,&TeacherWindow::personalInfoChanged);
}

void TeacherWindow::closeEvent(QCloseEvent *event)
{
    if(ui->savePushButton->isEnabled())
    { QMessageBox::StandardButton bt=QMessageBox::warning(this,tr("警告"),
                                                          tr("您的个人信息有未保存的变更，是否保存？"),
                                                          QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if(bt==QMessageBox::Yes)
        {
            on_savePushButton_clicked();
            event->accept();
        }
        else if(bt==QMessageBox::No)
            event->accept();
        else
            event->ignore();
    }
    event->accept();

}

void TeacherWindow::submitCourseApplication()
{
    QSqlQuery query;
    QString applyno;
    query.exec("select count(applyno) from courseapply");
    if(!query.isActive())
        QMessageBox::critical(this,tr("错误"),tr("申请提交失败，请重试或联系管理员！"));
    if(query.next())
    {
        int currentIndex=query.value(0).toInt()+1;
        applyno=tr("A%1").arg(QString::number(currentIndex));
        while(applyno.count()<6)
            applyno.insert(1,tr("0"));
    }
    else
    {
        QMessageBox::critical(this,tr("错误"),tr("申请提交失败，请重试或联系管理员！"));
    }
    query.prepare("insert into courseapply values("
                  ":applyno,:teacherno,"
                  ":coursename,:courseproperty,:courselength,'待审核',:courseinfo)");
    query.bindValue(":teacherno",teacherID);
    query.bindValue(":applyno",applyno);
    query.bindValue(":courseinfo",ui->CourseInfoPlainTextEdit->toPlainText());
    query.bindValue(":courseproperty",ui->CoursePropertyComboBox->currentText());
    query.bindValue(":coursename",ui->CourseNameineEdit->text());
    query.bindValue(":courselength",ui->courseLengthComboBox->currentText().toInt());
    if(!query.exec())
        QMessageBox::critical(this,tr("错误"),tr("申请提交失败，请重试或联系管理员！"));
}

void TeacherWindow::initCourseApply()
{
    applyModel=new QSqlTableModel(this);
    applyModel->setTable("courseapply");
    applyModel->setSort(apply_no,Qt::AscendingOrder);
    applyModel->setHeaderData(apply_no,Qt::Horizontal,tr("申请号"));
    applyModel->setHeaderData(apply_coursename,Qt::Horizontal,tr("课程名称"));
    applyModel->setHeaderData(apply_property,Qt::Horizontal,tr("课程属性"));
    applyModel->setHeaderData(apply_status,Qt::Horizontal,tr("审核状态"));
    applyModel->setHeaderData(apply_info,Qt::Horizontal,tr("课程介绍"));
    applyModel->setHeaderData(apply_courselength,Qt::Horizontal,tr("课时数"));
    applyModel->select();
    ui->applyTableView->setModel(applyModel);
    ui->applyTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->applyTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->applyTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->applyTableView->setColumnHidden(apply_teacherno,true);
    ui->applyTableView->resizeColumnsToContents();

}


void TeacherWindow::on_savePushButton_clicked()
{
    if(!mapper->submit())
        QMessageBox::critical(this,"错误","个人信息保存失败,请重试或联系管理员！");
    else
        ui->savePushButton->setEnabled(false);
}

void TeacherWindow::personalInfoChanged()
{
    ui->savePushButton->setEnabled(true);
}

void TeacherWindow::on_submitPushButton_clicked()
{
    if(ui->CourseNameineEdit->text().isEmpty())
    {
        QMessageBox::warning(this,"错误",tr("课程名称不能为空！"));
        return;
    }
    QMessageBox::StandardButton bt=QMessageBox::warning(this,tr("提示"),tr("提交后不能修改申请，是否确认提交"),
                                                        QMessageBox::Yes|QMessageBox::No);
    if(bt==QMessageBox::Yes)
    {
        submitCourseApplication();
    }
}

void TeacherWindow::on_refreshApplypushButton_clicked()
{
    applyModel->select();
}

void TeacherWindow::on_CancelApplyPushButton_clicked()
{
    if(applyModel->data(ui->applyTableView->selectionModel()->currentIndex())==tr("通过"))
    {
        QMessageBox::critical(this,tr("错误"),tr("此课程已审核通过，不能撤销！请联系教务处！"));
        return;
    }
    applyModel->removeRow(ui->applyTableView->currentIndex().row());
    int ok = QMessageBox::warning(this,tr("确认"),tr("你确定撤销此申请吗？"),
                                  QMessageBox::Yes,QMessageBox::No);
    if(ok == QMessageBox::No)
        applyModel->revertAll();
    else
        applyModel->submitAll();
    on_refreshApplypushButton_clicked();
}
