#include "studentwindow.h"
#include "ui_studentwindow.h"


StudentWindow::StudentWindow(QWidget *parent, const QString &str):
    QMainWindow(parent),
    ui(new Ui::StudentWindow),studentNo(str)
{
    ui->setupUi(this);
    initQuery();
    initCourse();
    initScore();
    connect(ui->queryTableView,&QTableView::clicked,this,&StudentWindow::updateCourseInfo);
    connect(ui->listWidget,&QListWidget::currentRowChanged,ui->stackedWidget,&QStackedWidget::setCurrentIndex);
}

StudentWindow::~StudentWindow()
{
    delete ui;
}

void StudentWindow::initQuery()
{
    queryModel=new QSqlQueryModel(this);
    query=("select courseno,coursename,timeofclass,courselength,courseproperty,teachername,department,"
           "cappacity-currentnumber remain,courseinfo from teacher,course,courseapply "
           "where teacher.teacherno=courseapply.teacherno and courseapply.applyno=course.applyno");
    queryModel->setQuery(query);
    if(!queryModel->query().isActive())
    {
        QMessageBox::critical(this,tr("错误"),tr("查询失败，请重试或联系管理员！\n错误信息：%1").arg(QSqlDatabase::database().lastError().text()));
    }
    queryModel->setHeaderData(0,Qt::Horizontal,tr("课程编号"));
    queryModel->setHeaderData(1,Qt::Horizontal,tr("课程名称"));
    queryModel->setHeaderData(2,Qt::Horizontal,tr("上课时间"));
    queryModel->setHeaderData(3,Qt::Horizontal,tr("课时数"));
    queryModel->setHeaderData(4,Qt::Horizontal,tr("课程性质"));
    queryModel->setHeaderData(5,Qt::Horizontal,tr("教师姓名"));
    queryModel->setHeaderData(6,Qt::Horizontal,tr("开课院系"));
    queryModel->setHeaderData(7,Qt::Horizontal,tr("课余量"));
    ui->queryTableView->setModel(queryModel);
    ui->queryTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->queryTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->queryTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->queryTableView->setColumnHidden(8,true);
    ui->queryTableView->resizeColumnsToContents();
}

void StudentWindow::initCourse()
{

}

void StudentWindow::initScore()
{
    scoreModel=new QSqlQueryModel(this);
    scoreModel->setQuery(tr("select coursename,score from course,courseselect,courseapply where course.applyno=courseapply.applyno and course.courseno=courseselect.courseno and studentno='%1' and score is not null").arg(studentNo));
    if(scoreModel->query().lastError().isValid())
        QMessageBox::critical(this,tr("错误"),tr("查询失败，请重试或联系管理员！\n错误信息：%1").arg(scoreModel->query().lastError().text()));
    scoreModel->setHeaderData(0,Qt::Horizontal,tr("课程"));
    scoreModel->setHeaderData(1,Qt::Horizontal,tr("成绩"));
    ui->scoreTableView->setModel(scoreModel);
    ui->queryTableView->setModel(queryModel);
    ui->queryTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->queryTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->queryTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->queryTableView->resizeColumnsToContents();
}

bool StudentWindow::hasTimeConflict(QString time)
{
    QSqlQuery query;
    query.prepare(tr("select count(course.courseno) from courseselect,course where studentno='%1' and course.courseno=courseselect.courseno and timeofclass='%2'").arg(studentNo).arg(time));
    if(!query.exec()||!query.next())
    {
        QMessageBox::critical(this,tr("错误"),tr("查询失败，请重试或联系管理员！\n错误信息：%1").arg(QSqlDatabase::database().lastError().text()));
        return true;
    }
    if(query.value(0).toString()==tr("1"))
    {
        QMessageBox::warning(this,tr("提示"),tr("时间冲突！"));
        return true;
    }
    else
        return false;
}

bool StudentWindow::hasCreditConflict(QString courseno)
{
    QSqlQuery query;
    query.prepare(tr("select sum(credit) from courseselect,course where courseselect.courseno=course.courseno and studentno='%1'").arg(studentNo));
    if(!query.exec()||!query.next())
    {
        QMessageBox::critical(this,tr("错误"),tr("查询失败，请重试或联系管理员！\n错误信息：%1").arg(QSqlDatabase::database().lastError().text()));
        return true;
    }
    int currentCredit=query.value(0).toInt();
    query.prepare(tr("select credit from course where courseno='%1'").arg(courseno));
    if(!query.exec()||!query.next())
    {
        QMessageBox::critical(this,tr("错误"),tr("查询失败，请重试或联系管理员！\n错误信息：%1").arg(QSqlDatabase::database().lastError().text()));
        return true;
    }
    int theCredit=query.value(0).toInt();
    if(currentCredit+theCredit>32)
    {
        QMessageBox::warning(this,tr("提示"),tr("学分超限！"));
        return true;
    }
    else
        return false;


}

void StudentWindow::on_queryRefreshPushButton_clicked()
{
    queryModel->query().exec();
}

void StudentWindow::on_viewAllPushButton_clicked()
{
    queryModel->setQuery(query);
}

void StudentWindow::on_queryPushButton_clicked()
{
    QString tmpQuery=query;
    if(ui->courseNameLineEdit->text().count())
        tmpQuery+=tr(" and coursename like '\%%1\%'").arg(ui->courseNameLineEdit->text());
    if(ui->courseNoLineEdit->text().count())
        tmpQuery+=tr(" and courseno like '\%%1\%'").arg(ui->courseNoLineEdit->text());
    if(ui->teacherNameLineEdit->text().count())
        tmpQuery+=tr(" and teachername like '\%%1\%'").arg(ui->teacherNameLineEdit->text());
    if(ui->departmentLineEdit->text().count())
        tmpQuery+=tr(" and department like '\%%1\%'").arg(ui->departmentLineEdit->text());
    tmpQuery+=tr(" and courseproperty='%1'").arg(ui->propertyComboBox->currentText());
    queryModel->setQuery(tmpQuery);
    if(!queryModel->query().isActive())
    {
        QMessageBox::critical(this,tr("错误"),tr("查询失败，请重试或联系管理员！\n错误信息：%1").arg(QSqlDatabase::database().lastError().text()));
    }
}

void StudentWindow::updateCourseInfo()
{
    QModelIndex index=ui->queryTableView->currentIndex();
    index=index.sibling(index.row(),8);
    if(index.isValid())
        ui->courseInfoTextBrowser->setText(queryModel->data(index).toString());

    else
        ui->courseInfoTextBrowser->clear();
}

void StudentWindow::on_submitPushButton_clicked()
{
    QModelIndex index=ui->queryTableView->currentIndex();
    index=index.sibling(index.row(),0);
    if(!index.isValid())
        return;
    if(hasCreditConflict(queryModel->data(index).toString())||
            hasTimeConflict(queryModel->data(index.sibling(index.row(),2)).toString()))
        return;
    QSqlQuery query;
    query.prepare(tr("insert into courseselect(studentno,courseno) values('%1','%2')").arg(studentNo).arg(queryModel->data(index).toString()));
    if(!query.exec())
    {
        QMessageBox::critical(this,tr("错误"),tr("查询失败，请重试或联系管理员！\n错误信息：%1").arg(QSqlDatabase::database().lastError().text()));
        statusBar()->showMessage(tr("选课提交失败！"));
    }
    else
        statusBar()->showMessage(tr("选课提交成功！"));
}

void StudentWindow::on_scoreRefreshPushButton_clicked()
{
    scoreModel->query().exec();
}
