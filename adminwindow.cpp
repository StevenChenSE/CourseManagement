#include "adminwindow.h"
#include "ui_adminwindow.h"
#include "initcoursedialog.h"

AdminWindow::AdminWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);
    connect(ui->listWidget,&QListWidget::currentRowChanged,
            ui->stackedWidget,&QStackedWidget::setCurrentIndex);
    connect(ui->applyTableView,&QTableView::clicked,
            this,&AdminWindow::updateApplyCourseInfo);
    connect(ui->courseTableView,&QTableView::clicked,
            this,&AdminWindow::updateCourseInfo);
    initApply();
    initCourse();
    initCommandWindow();
}

AdminWindow::~AdminWindow()
{
    delete ui;
}

void AdminWindow::updateApplyCourseInfo()
{
    QModelIndex index=ui->applyTableView->currentIndex();
    index=index.sibling(index.row(),6);
    if(index.isValid())
        ui->applyCourseInfoTextBrowser->setPlainText(applyModel->data(index).toString());
    else
        ui->applyCourseInfoTextBrowser->clear();

}

void AdminWindow::updateCourseInfo()
{
    QModelIndex index=ui->courseTableView->currentIndex();
    index=index.sibling(index.row(),11);
    if(index.isValid())
    {
        ui->CourseInfoTextBrowser->setPlainText(courseModel->data(index).toString());
    }
    else
        ui->CourseInfoTextBrowser->clear();

}

void AdminWindow::initApply()
{
    applyModel=new QSqlQueryModel(this);
    applyModel->setQuery(QSqlQuery(tr("select applyno,coursename,courseproperty,courselength,"
                                      "teachername,department,courseinfo,status from teacher,"
                                      "courseapply where status='待审核' and "
                                      "teacher.teacherno=courseapply.teacherno order by applyno")));
    if(!applyModel->query().isActive())
    {
        qDebug()<<applyModel->lastError();
        QMessageBox::critical(this,tr("错误"),tr("查询申请失败，请重试或联系管理员！\n错误信息：QSqlDatabase::database()->lastError()"));
    }

    applyModel->setHeaderData(0,Qt::Horizontal,tr("申请编号"));
    applyModel->setHeaderData(1,Qt::Horizontal,tr("课程名称"));
    applyModel->setHeaderData(2,Qt::Horizontal,tr("课程性质"));
    applyModel->setHeaderData(3,Qt::Horizontal,tr("课时数"));
    applyModel->setHeaderData(4,Qt::Horizontal,tr("老师姓名"));
    applyModel->setHeaderData(5,Qt::Horizontal,tr("开课院系"));
    ui->applyTableView->setModel(applyModel);
    ui->applyTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->applyTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->applyTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->applyTableView->setColumnHidden(6,true);
    ui->applyTableView->setColumnHidden(7,true);
    ui->applyTableView->resizeColumnsToContents();
}

void AdminWindow::initCourse()
{
    courseModel=new QSqlQueryModel(this);
    courseModel->setQuery(tr("select courseno,coursename,teachername,department,"
                             "courseproperty,courselength,timeofclass,credit,"
                             "classroom,cappacity,currentnumber,courseinfo "
                             "from course,courseapply,teacher where "
                             "course.applyno=courseapply.applyno and "
                             "teacher.teacherno=courseapply.teacherno"));
    if(!courseModel->query().isActive())
    {
        qDebug()<<applyModel->lastError();
        QMessageBox::critical(this,tr("错误"),tr("查询申请失败，请重试或联系管理员！\n错误信息：QSqlDatabase::database()->lastError()"));
    }
    courseModel->setHeaderData(0,Qt::Horizontal,tr("课程编号"));
    courseModel->setHeaderData(1,Qt::Horizontal,tr("课程名称"));
    courseModel->setHeaderData(2,Qt::Horizontal,tr("老师姓名"));
    courseModel->setHeaderData(3,Qt::Horizontal,tr("开课院系"));
    courseModel->setHeaderData(4,Qt::Horizontal,tr("课程性质"));
    courseModel->setHeaderData(5,Qt::Horizontal,tr("课时数"));
    courseModel->setHeaderData(6,Qt::Horizontal,tr("上课时间"));
    courseModel->setHeaderData(7,Qt::Horizontal,tr("学分"));
    courseModel->setHeaderData(8,Qt::Horizontal,tr("教室"));
    courseModel->setHeaderData(9,Qt::Horizontal,tr("课容量"));
    courseModel->setHeaderData(10,Qt::Horizontal,tr("当前选课人数"));
    ui->courseTableView->setModel(courseModel);
    ui->courseTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->courseTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->courseTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->courseTableView->setColumnHidden(11,true);
    ui->courseTableView->resizeColumnsToContents();
}

void AdminWindow::initCommandWindow()
{
    SQLModel=new QSqlQueryModel(this);
    ui->dataTableView->setModel(SQLModel);

}

void AdminWindow::on_rejectPushButton_clicked()
{
    QModelIndex index=ui->applyTableView->currentIndex();
    if(!index.isValid())
        return;
    index=index.sibling(index.row(),0);
    QString applyno=applyModel->data(index).toString();
    qDebug()<<applyno;
    QMessageBox::StandardButton bt=QMessageBox::warning(this,tr("提示"),
                                                        tr("确认拒绝此申请吗？"),
                                                        QMessageBox::Yes|QMessageBox::No);
    if(bt==QMessageBox::Yes)
    {
        QSqlQuery query;
        query.exec(tr("update courseapply set status='未通过' where applyno='%1'").arg(applyno));
        if(!query.isActive())
        {
            QMessageBox::critical(this,tr("错误"),tr("提交审核失败，请重试或联系管理员！\n错误信息：QSqlDatabase::database()->lastError()"));
            qDebug()<<query.lastError();
        }
    }
    applyModel->query().exec();
    ui->applyTableView->reset();
}

void AdminWindow::on_passPushButton_clicked()
{
    QModelIndex index=ui->applyTableView->currentIndex();
    if(!index.isValid())
        return;
    index=index.sibling(index.row(),0);
    initCourseDialog(this,applyModel->data(index).toString()).exec();
    applyModel->query().exec();
}

void AdminWindow::on_refreshApplyPushButton_clicked()
{
    applyModel->query().exec();
}

void AdminWindow::on_refreshCoursePushButton_clicked()
{
    courseModel->query().exec();
    if(!courseModel->query().isActive())
    {
        qDebug()<<applyModel->lastError();
        QMessageBox::critical(this,tr("错误"),tr("查询申请失败，请重试或联系管理员！\n错误信息：QSqlDatabase::database()->lastError()"));
    }
}

void AdminWindow::on_executePushButton_clicked()
{
    if(!ui->SQLTextEdit->toPlainText().count())
        return;
    SQLModel->setQuery(ui->SQLTextEdit->toPlainText());
    ui->dataTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->dataTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->dataTableView->resizeColumnsToContents();
    if(!SQLModel->query().isActive())
    {
        QMessageBox::critical(this,tr("错误"),SQLModel->lastError().text());
        statusBar()->showMessage(tr("执行出现错误！"));
    }
    else
        statusBar()->showMessage(tr("SQL语句成功执行！"));
}
