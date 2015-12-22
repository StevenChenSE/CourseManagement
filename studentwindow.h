#ifndef STUDENTWINDOW_H
#define STUDENTWINDOW_H

#include "qtheaders.h"
#include "coursetablemodel.h"
namespace Ui {
class StudentWindow;
}

class StudentWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StudentWindow(QWidget *parent = 0,const QString & str="");
    ~StudentWindow();

private slots:
    void on_queryRefreshPushButton_clicked();
    void on_viewAllPushButton_clicked();
    void on_queryPushButton_clicked();
    void updateCourseInfo();
    void updateDetails();
    void on_submitPushButton_clicked();
    void on_scoreRefreshPushButton_clicked();
    void on_detailRefreshPushButton_clicked();
    void on_disselectPushButton_clicked();

private:
    Ui::StudentWindow *ui;
    QString studentNo;
    QString courseQuery;
    QSqlQueryModel* queryModel;
    QSqlQueryModel* scoreModel;
    QSqlTableModel* personalInfoModel;
    CourseTableModel* courseModel;
    void initQuery();
    void initCourse();
    void initScore();
    void initPersonalInfo();
    bool hasTimeConflict(QString time);
    bool hasCreditConflict(QString courseno);
    bool canDisselect(const QString & courseno);
    int idGenerate();
};

#endif // STUDENTWINDOW_H
