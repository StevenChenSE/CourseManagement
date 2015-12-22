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
    initPersonalInfo();
    connect(ui->queryTableView->selectionModel(),&QItemSelectionModel::currentRowChanged,this,&StudentWindow::updateCourseInfo);
    connect(ui->listWidget,&QListWidget::currentRowChanged,ui->stackedWidget,&QStackedWidget::setCurrentIndex);
    connect(ui->courseView->selectionModel(),&QItemSelectionModel::currentChanged,this,&StudentWindow::updateDetails);
}

StudentWindow::~StudentWindow()
{
    delete ui;
}

void StudentWindow::initQuery()
{
    queryModel=new QSqlQueryModel(this);
    courseQuery=("select course.courseno,coursename,timeofclass,courselength,courseproperty,teachername,department,cappacity-count(courseselect.studentno) cnt"
           " from course left outer join courseselect on courseselect.courseno=course.courseno,courseapply,teacher where  course.applyno=courseapply.applyno"
           " and courseapply.teacherno=teacher.teacherno group by course.courseno,coursename,timeofclass,courselength,courseproperty,teachername,department,cappacity"
                 " order by cnt desc");
    queryModel->setQuery(courseQuery);
    if(!queryModel->query().isActive())
    {
        QMessageBox::critical(this,tr("错误"),tr("查询失败，请重试或联系管理员！\n错误信息：%1").arg(queryModel->query().lastError().text()));
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
    ui->queryTableView->setSortingEnabled(true);
}

void StudentWindow::initCourse()
{
    courseModel=new CourseTableModel(this,studentNo);
    for(int i=0;i<6;i++)
        courseModel->setHeaderData(i,Qt::Vertical,QString::number(i+1),Qt::DisplayRole);
    ui->courseView->setModel(courseModel);
    ui->courseView->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->courseView->setSelectionMode
            (QAbstractItemView::SingleSelection);
    ui->courseView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->courseView->resizeColumnsToContents();
}

void StudentWindow::initScore()
{
    scoreModel=new QSqlQueryModel(this);
    scoreModel->setQuery(tr("select coursename,score from course,courseselect,courseapply where course.applyno=courseapply.applyno and course.courseno=courseselect.courseno and studentno='%1' and score is not null").arg(studentNo));
    if(scoreModel->query().lastError().isValid())
        QMessageBox::critical(this,tr("错误"),tr("查询失败，请重试或联系管理员！\n错误信息：%1").arg(scoreModel->query().lastError().text()));
    scoreModel->setHeaderData(0,Qt::Horizontal,tr("课程"));
    scoreModel->setHeaderData(1,Qt::Horizontal,tr("成绩"));
    QSortFilterProxyModel* scoreProxy=new QSortFilterProxyModel(this);
    scoreProxy->setSourceModel(scoreModel);

    ui->scoreTableView->setModel(scoreProxy);
    ui->scoreTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->scoreTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->scoreTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->scoreTableView->resizeColumnsToContents();
    ui->scoreTableView->setSortingEnabled(true);
}

void StudentWindow::initPersonalInfo()
{
    personalInfoModel=new QSqlTableModel(this);
    personalInfoModel->setTable("student");
    personalInfoModel->setFilter(tr("studentno='%1'").arg(studentNo));
    personalInfoModel->select();
    qDebug()<<personalInfoModel->lastError();

    QDataWidgetMapper* mapper=new QDataWidgetMapper(this);
    mapper->setModel(personalInfoModel);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->addMapping(ui->studentNoLineEdit,0);
    mapper->addMapping(ui->studentNameLineEdit,1);
    mapper->addMapping(ui->studentSexLineEdit,3);
    mapper->addMapping(ui->studentEmailLineEdit,4);
    mapper->addMapping(ui->studentDepartmentLineEdit,6);
    mapper->addMapping(ui->studentClassLineEdit,5);
    mapper->setCurrentIndex(0);

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
    qDebug()<<query.lastQuery();
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

bool StudentWindow::canDisselect(const QString &courseno)
{
    QSqlQuery query;
    query.prepare(tr("select selectid from courseselect where studentno='%1' and courseno='%2' and score is not null").arg(studentNo).arg(courseno));
    if(!query.exec())
    {
        QMessageBox::critical(this,tr("错误"),tr("查询失败，请重试或联系管理员！\n错误信息：%1").arg(query.lastError().text()));
        return false;
    }
    return !query.next();
}

int StudentWindow::idGenerate()
{
    QSqlQuery query;
    query.prepare("select selectid from courseselect order by selectid");
    if(!query.exec())
    {
        QMessageBox::critical(this,tr("错误"),tr("查询失败，请重试或联系管理员！\n错误信息：%1").arg(query.lastError().text()));
        return -1;
    }
    if(query.last())
        return query.value(0).toInt()+1;
    else
        return 1;
}

void StudentWindow::on_queryRefreshPushButton_clicked()
{
    queryModel->query().exec();
    ui->queryTableView->reset();
}

void StudentWindow::on_viewAllPushButton_clicked()
{
    queryModel->setQuery(courseQuery);
}

void StudentWindow::on_queryPushButton_clicked()
{
    QString filter,tmpQuery=courseQuery;
    filter+=tr("and courseproperty='%1'").arg(ui->propertyComboBox->currentText());
    if(ui->courseNameLineEdit->text().count())
        filter+=tr(" and coursename like '\%%1\%'").arg(ui->courseNameLineEdit->text());
    if(ui->courseNoLineEdit->text().count())
        filter+=tr(" and course.courseno like '\%%1\%'").arg(ui->courseNoLineEdit->text());
    if(ui->teacherNameLineEdit->text().count())
        filter+=tr(" and teachername like '\%%1\%'").arg(ui->teacherNameLineEdit->text());
    if(ui->departmentLineEdit->text().count())
        filter+=tr(" and department like '\%%1\%'").arg(ui->departmentLineEdit->text());
    filter+=" ";
    tmpQuery.insert(tmpQuery.indexOf("group"),filter);
    queryModel->setQuery(tmpQuery);
    if(!queryModel->query().isActive())
    {
        QMessageBox::critical(this,tr("错误"),tr("查询失败，请重试或联系管理员！\n错误信息：%1").arg(queryModel->query().lastError().text()));
    }
}

void StudentWindow::updateCourseInfo()
{
    QModelIndex index=ui->queryTableView->currentIndex();
    index=index.sibling(index.row(),0);
    QSqlQuery query;
    query.prepare(tr("select courseinfo from course,courseapply where course.applyno=courseapply.applyno and courseno='%1'").arg(queryModel->data(index).toString()));
    if(query.exec()&&query.next())
        ui->courseInfoTextBrowser->setText(query.value(0).toString());
    else
        QMessageBox::critical(this,tr("错误"),tr("数据库错误，找不到对应的数据!"));
}

void StudentWindow::updateDetails()
{
    if(!ui->courseView->currentIndex().isValid())
    {
        ui->courseNoInfo->setText(QString());
        ui->courseNameInfo->setText(QString());
        ui->teacherNameInfo->setText(QString());
        ui->departmentInfo->setText(QString());
        ui->timeOfclassInfo->setText(QString());
        ui->locationInfo->setText(QString());
        ui->creditInfo->setText(QString());
        ui->courseLengthInfo->setText(QString());
        return;
    }
    QSqlQuery tmp(tr("select courseno,coursename,teachername,department,timeofclass,classroom,credit,courselength,courseinfo"
                     " from course,courseapply,teacher"
                     " where course.applyno=courseapply.applyno and teacher.teacherno=courseapply.teacherno"
                     " and courseno='%1'").
                  arg(courseModel->data(ui->courseView->currentIndex(),Qt::EditRole).toString()));
    if(!tmp.exec())
        QMessageBox::critical(this,tr("错误"),tr("查询失败，请重试或联系管理员！\n错误信息：%1").arg(tmp.lastError().text()));
    if(tmp.next())
    {
        ui->courseNoInfo->setText(tmp.value(0).toString());
        ui->courseNameInfo->setText(tmp.value(1).toString());
        ui->teacherNameInfo->setText(tmp.value(2).toString());
        ui->departmentInfo->setText((tmp.value(3).toString()));
        ui->timeOfclassInfo->setText((tmp.value(4).toString()));
        ui->locationInfo->setText((tmp.value(5).toString()));
        ui->creditInfo->setText(tmp.value(6).toString());
        ui->courseLengthInfo->setText(tmp.value(7).toString());
    }
    else
    {
        ui->courseNoInfo->setText(QString());
        ui->courseNameInfo->setText(QString());
        ui->teacherNameInfo->setText(QString());
        ui->departmentInfo->setText(QString());
        ui->timeOfclassInfo->setText(QString());
        ui->locationInfo->setText(QString());
        ui->creditInfo->setText(QString());
        ui->courseLengthInfo->setText(QString());
    }
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
    int id=idGenerate();
    if(id==-1)
        return;
    query.prepare(tr("insert into courseselect(selectid,studentno,courseno) values(%1,'%2','%3')").arg(id).arg(studentNo).arg(queryModel->data(index).toString()));
    if(!query.exec())
    {
        QMessageBox::critical(this,tr("错误"),tr("查询失败，请重试或联系管理员！\n错误信息：%1").arg(QSqlDatabase::database().lastError().text()));
        statusBar()->showMessage(tr("选课提交失败！"));
    }
    else
    {
        QMessageBox::warning(this,tr("提示"),tr("提交选课成功！"));
        statusBar()->showMessage(tr("选课提交成功！"));
    }
    qDebug()<<query.lastQuery();
}

void StudentWindow::on_scoreRefreshPushButton_clicked()
{
    scoreModel->query().exec();
    ui->scoreTableView->reset();
}

void StudentWindow::on_detailRefreshPushButton_clicked()
{
    courseModel->initQuery();
   ui->courseView->reset();
    ui->courseView->resizeColumnsToContents();
    updateDetails();
}

void StudentWindow::on_disselectPushButton_clicked()
{
    QModelIndex index=ui->courseView->currentIndex();
    if(!index.isValid())
    {
        statusBar()->showMessage(tr("您没有选择任何课程！"));
        return;
    }
    QString courseno=courseModel->data(index,Qt::EditRole).toString();
    if(courseno.isEmpty())
    {
        statusBar()->showMessage(tr("您没有选择任何课程！"));
        return;
    }
    QSqlQuery query;
    if(QMessageBox::question(this,tr("提示"),tr("您确定要退课吗?"))==QMessageBox::No)
        return;
    if(!canDisselect(courseno))
    {
        QMessageBox::warning(this,tr("错误"),tr("此课程已出成绩，无法退课！"));
        return;
    }
    query.prepare(tr("delete from courseselect where courseno='%1' and studentno='%2'").arg(courseno).arg(studentNo));
    if(!query.exec())
    {
        QMessageBox::critical(this,tr("错误"),tr("退课失败，请重试或联系管理员！\n错误信息：%1").arg(query.lastError().text()));
        statusBar()->showMessage(tr("退课提交失败！"));
    }
    else
    {
        QMessageBox::warning(this,tr("提示"),tr("退课成功!"));
        statusBar()->showMessage(tr("退课提交成功！"));
        courseModel->initQuery();
        ui->courseView->reset();
    }
    on_detailRefreshPushButton_clicked();

}
