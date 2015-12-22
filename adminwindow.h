#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H
#include "qtheaders.h"
namespace Ui {
class AdminWindow;
}

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = 0);
    ~AdminWindow();
private slots:
    void updateApplyCourseInfo();
    void updateCourseInfo();
    void on_rejectPushButton_clicked();
    void on_passPushButton_clicked();
    void on_refreshApplyPushButton_clicked();
    void on_refreshCoursePushButton_clicked();
    void on_executePushButton_clicked();

private:
    Ui::AdminWindow *ui;
    QSqlQueryModel *applyModel;
    QSqlQueryModel *courseModel;
    QSqlQueryModel *SQLModel;
    void initApply();
    void initCourse();
    void initCommandWindow();
};

#endif // ADMINWINDOW_H
