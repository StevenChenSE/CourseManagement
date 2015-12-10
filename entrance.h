#ifndef ENTRANCE_H
#define ENTRANCE_H
#include "qtheaders.h"
namespace Ui {
class Entrance;
}
class Entrance : public QDialog
{
    Q_OBJECT

public:
    explicit Entrance(QWidget *parent = 0);
    ~Entrance();

private slots:
    void on_studentButton_clicked();
    void on_teacherButton_clicked();
    void on_adminButton_clicked();

private:
    Ui::Entrance *ui;
    QSqlDatabase db;
    bool createConnection();
};

#endif // ENTRANCE_H
