#ifndef TEACHERWINDOW_H
#define TEACHERWINDOW_H
#include "qtheaders.h"
namespace Ui {
class TeacherWindow;
}

class TeacherWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TeacherWindow(QWidget *parent = 0,const QString & teacherId="T00001");
    ~TeacherWindow();
    enum{
        teacher_no=0,
        teacher_name=2,
        teacher_sex=3,
        teacher_email=4,
        teacher_info=5,
        teacher_department=6
    };
    enum{
        apply_no=0,
        apply_teacherno=1,
        apply_coursename=2,
        apply_property=3,
        apply_courselength=4,
        apply_status=5,
        apply_info=6
    };

private slots:
    void on_savePushButton_clicked();
    void personalInfoChanged();
    void on_submitPushButton_clicked();

    void on_refreshApplypushButton_clicked();

    void on_CancelApplyPushButton_clicked();

private:
    Ui::TeacherWindow *ui;
    QString teacherID;
    QSqlTableModel *teacherModel;
    QSqlTableModel *applyModel;
    QDataWidgetMapper *mapper;
    void initInfo();
    void closeEvent(QCloseEvent* event);
    void submitCourseApplication();
    void initCourseApply();
};

#endif // TEACHERWINDOW_H
