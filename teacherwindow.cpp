#include "teacherwindow.h"
#include "ui_teacherwindow.h"
TeacherWindow::TeacherWindow(QWidget *parent, const QString &teacherId) :
    QMainWindow(parent),
    ui(new Ui::TeacherWindow),teacherID(teacherId)
{
    ui->setupUi(this);
    initInfo();
    initCourseApply();
    initStudent();
    connect(ui->listWidget,&QListWidget::currentRowChanged,
            ui->stackedWidget,&QStackedWidget::setCurrentIndex);
    connect(ui->courseTableView,&QTableView::clicked,
            this,&TeacherWindow::updateStudentScore);
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

bool TeacherWindow::submitCourseApplication()
{
    QSqlQuery query;
    QString applyno;
    query.prepare("insert into courseapply values("
                  ":applyno,:teacherno,"
                  ":coursename,:courseproperty,:courselength,'待审核',:courseinfo)");
    query.bindValue(":teacherno",teacherID);
    query.bindValue(":applyno",getCurrentIndex());
    query.bindValue(":courseinfo",ui->CourseInfoPlainTextEdit->toPlainText());
    query.bindValue(":courseproperty",ui->CoursePropertyComboBox->currentText());
    query.bindValue(":coursename",ui->CourseNameineEdit->text());
    query.bindValue(":courselength",ui->courseLengthComboBox->currentText().toInt());
    if(!query.exec())
    {
        QMessageBox::critical(this,tr("错误"),tr("申请提交失败，请重试或联系管理员！\n错误信息：").arg(query.lastError().text()));
        return false;
    }
    else
        return true;
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

void TeacherWindow::initStudent()
{
    courseModel=new QSqlQueryModel(this);
    courseModel->setQuery(tr("select coursename,count(studentno) a from (select coursename,studentno,course.courseno from course,courseapply,courseselect where courseapply.applyno=course.applyno and course.courseno=courseselect.courseno and teacherno='%1') group by coursename ").arg(teacherID));
    if(courseModel->query().lastError().isValid())
        QMessageBox::critical(this,tr("错误"),tr("数据库查询失败，请重试或联系管理员！\n错误信息：%1").arg(courseModel->query().lastError().text()));

    courseModel->setHeaderData(0,Qt::Horizontal,tr("课程名称"));
    courseModel->setHeaderData(1,Qt::Horizontal,tr("选课人数"));

    ui->courseTableView->setModel(courseModel);
    ui->courseTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->courseTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->courseTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->courseTableView->setColumnHidden(2,true);
    ui->courseTableView->horizontalHeader()->setStretchLastSection(true);

    studentModel=new QSqlRelationalTableModel(this);
    studentModel->setTable("courseselect");
    studentModel->setFilter(tr("teacherno='%1'").arg(teacherID));
    studentModel->setRelation(0,QSqlRelation("student","studentno","studentname"));
    studentModel->setSort(2,Qt::AscendingOrder);
    studentModel->setHeaderData(0,Qt::Horizontal,tr("学生姓名"));
    studentModel->setHeaderData(2,Qt::Horizontal,tr("成绩"));
    studentModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    ui->studentTableView->setModel(studentModel);
    ui->studentTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->studentTableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->studentTableView->horizontalHeader()->setStretchLastSection(true);
    ui->studentTableView->setColumnHidden(1,true);

}

QString TeacherWindow::getCurrentIndex()
{
    QSqlQuery query("select applyno from courseapply order by applyno");
    if(query.isActive())
    {
        int currentIndex=query.last()?query.value(0).toString().remove(0,1).toInt()+1:1;
        QString str=tr("A%1").arg(QString::number(currentIndex));
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


void TeacherWindow::on_savePushButton_clicked()
{
    if(!mapper->submit())
        QMessageBox::critical(this,tr("错误"),tr("保存提交失败，请重试或联系管理员！\n错误信息：").arg(teacherModel->lastError().text()));
    else
        ui->savePushButton->setEnabled(false);
}

void TeacherWindow::personalInfoChanged()
{
    ui->savePushButton->setEnabled(true);
}

void TeacherWindow::updateStudentScore()
{
    QModelIndex index=ui->courseTableView->currentIndex();
    index=index.sibling(index.row(),0);
    QSqlQuery query(tr("select courseno from course,courseapply where course.applyno=courseapply.applyno and coursename='%1' and teacherno='%2'").arg(courseModel->data(index).toString()).arg(teacherID));
    if(!query.exec()||!query.next())
    {
        QMessageBox::critical(this,tr("错误"),tr("查询提交失败，请重试或联系管理员！\n错误信息：%1").arg(query.lastError().text()));
        return;
    }
    QString courseno=query.value(0).toString();
    studentModel->setFilter(tr("courseno='%1'").arg(courseno));
    studentModel->select();
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
        if(submitCourseApplication())
            statusBar()->showMessage(tr("成功提交申请！"));
        else
            statusBar()->showMessage(tr("申请提交失败！"));
    }
}

void TeacherWindow::on_refreshApplypushButton_clicked()
{
    applyModel->select();
}

void TeacherWindow::on_CancelApplyPushButton_clicked()
{
    QModelIndex index=ui->applyTableView->currentIndex();
    index=index.sibling(index.row(),apply_status);
    if(applyModel->data(index)!=tr("待审核"))
    {
        QMessageBox::critical(this,tr("错误"),tr("此课程已经过审核，不能撤销！请联系教务处！"));
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

void TeacherWindow::on_submitScorePushButton_clicked()
{
    QMessageBox::StandardButton bt=QMessageBox::question(this,tr("提示"),tr("是否确认提交成绩？"));
    if(bt==QMessageBox::Yes)
    {
        if(!studentModel->submitAll())
                QMessageBox::critical(this,tr("错误"),tr("成绩提交失败，请重试或联系管理员！\n错误信息：%1").arg(studentModel->lastError().text()));
    }
    else
        studentModel->revertAll();
}
