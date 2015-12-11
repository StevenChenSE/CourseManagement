#ifndef STUDENTWINDOW_H
#define STUDENTWINDOW_H

#include "qtheaders.h"
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

    void on_submitPushButton_clicked();

    void on_scoreRefreshPushButton_clicked();

private:
    Ui::StudentWindow *ui;
    QString studentNo;
    QString query;
    QSqlQueryModel* queryModel;
    QSqlQueryModel* courseModel;
    QSqlQueryModel* scoreModel;
    void initQuery();
    void initCourse();
    void initScore();
    bool hasTimeConflict(QString time);
    bool hasCreditConflict(QString courseno);
};

#endif // STUDENTWINDOW_H
