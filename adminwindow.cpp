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
    index=index.sibling(index.row(),0);
    QSqlQuery query;
    if(index.isValid())
        query.prepare(tr("select courseinfo from course,courseapply where course.applyno=courseapply.applyno and courseno='%1'").arg(courseModel->data(index).toString()));
    if(query.exec()&&query.next())
    {
        ui->CourseInfoTextBrowser->setPlainText(query.value(0).toString());
    }
    else
    {
        QMessageBox::critical(this,tr("错误"),query.lastError().text());
        ui->CourseInfoTextBrowser->clear();
    }


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
        QMessageBox::critical(this,tr("错误"),tr("查询申请失败，请重试或联系管理员！\n错误信息：%1").arg(applyModel->query().lastError().text()));
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
    ui->applyTableView->setSortingEnabled(true);
}

void AdminWindow::initCourse()
{
    courseModel=new QSqlQueryModel(this);
    QSqlQuery query("select course.courseno,coursename,timeofclass,courselength,courseproperty,credit,teachername,department,cappacity,count(courseselect.studentno)"
            " from course left outer join courseselect on courseselect.courseno=course.courseno,courseapply,teacher where  course.applyno=courseapply.applyno"
            " and courseapply.teacherno=teacher.teacherno group by course.courseno,coursename,timeofclass,courselength,courseproperty,teachername,department,cappacity,credit");
    courseModel->setQuery(query);
    if(!courseModel->query().isActive())
    {
        QMessageBox::critical(this,tr("错误"),tr("查询申请失败，请重试或联系管理员！\n错误信息：%1").arg(courseModel->query().lastError().text()));
    }
    courseModel->setHeaderData(0,Qt::Horizontal,tr("课程编号"));
    courseModel->setHeaderData(1,Qt::Horizontal,tr("课程名称"));
    courseModel->setHeaderData(2,Qt::Horizontal,tr("上课时间"));
    courseModel->setHeaderData(3,Qt::Horizontal,tr("课时数"));
    courseModel->setHeaderData(4,Qt::Horizontal,tr("课程性质"));
    courseModel->setHeaderData(5,Qt::Horizontal,tr("学分"));
    courseModel->setHeaderData(6,Qt::Horizontal,tr("老师姓名"));
    courseModel->setHeaderData(7,Qt::Horizontal,tr("开课院系"));
    courseModel->setHeaderData(8,Qt::Horizontal,tr("课容量"));
    courseModel->setHeaderData(9,Qt::Horizontal,tr("当前选课人数"));
    ui->courseTableView->setModel(courseModel);
    ui->courseTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->courseTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->courseTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->courseTableView->setColumnHidden(11,true);
    ui->courseTableView->resizeColumnsToContents();
    ui->courseTableView->setSortingEnabled(true);
}

void AdminWindow::initCommandWindow()
{
    SQLModel=new QSqlQueryModel(this);
    ui->dataTableView->setModel(SQLModel);
    ui->dataTableView->setSortingEnabled(true);
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
            QMessageBox::critical(this,tr("错误"),tr("提交审核失败，请重试或联系管理员！\n错误信息：%1").arg(query.lastError().text()));
        }
    }
    applyModel->query().exec();
    ui->applyTableView->reset();
    on_refreshApplyPushButton_clicked();
}

void AdminWindow::on_passPushButton_clicked()
{
    QModelIndex index=ui->applyTableView->currentIndex();
    if(!index.isValid())
        return;
    index=index.sibling(index.row(),0);
    initCourseDialog(this,applyModel->data(index).toString()).exec();
    applyModel->query().exec();
    ui->applyTableView->reset();
    on_refreshApplyPushButton_clicked();
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
        QMessageBox::critical(this,tr("错误"),tr("查询申请失败，请重试或联系管理员！\n错误信息：QSqlDatabase::database()->lastError()"));
    }
    ui->courseTableView->reset();
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
