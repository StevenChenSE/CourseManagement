#include "coursetablemodel.h"



CourseTableModel::CourseTableModel(QObject *parent, const QString &studentno):
    QAbstractTableModel(parent),studentNo(studentno)
{
    initQuery();
}

int CourseTableModel::rowCount(const QModelIndex &) const
{
    return 6;
}

int CourseTableModel::columnCount(const QModelIndex &) const
{
    return 6;
}

QVariant CourseTableModel::data(const QModelIndex &index, int role) const
{
    if(index.row()<0||index.row()>=6)
        return QVariant();
    if(index.column()<0||index.column()>=6)
        return QVariant();
    int r=index.row(),c=index.column();
    if(role==Qt::DisplayRole)
    {
        return coursename[r][c];
    }
    else
        if(role==Qt::EditRole)
        {
            return courseno[r][c];
        }
    return QVariant();
}

void CourseTableModel::initQuery()
{
    for(int i=0;i<6;i++)
        for(int j=0;j<6;j++)
        {
            courseno[i][j]=QString();
            coursename[i][j]=QString();
        }
    QSqlQuery query;
    query.prepare(tr("select coursename,course.courseno,timeofclass from courseselect,course,courseapply where courseapply.applyno=course.applyno and course.courseno=courseselect.courseno and studentno='%1'").arg(studentNo));
    if(!query.exec())
        QMessageBox::critical(NULL,tr("错误"),tr("数据库查询失败，请重试或联系管理员！\n错误信息：").arg(query.lastError().text()));
    while(query.next())
    {
        QString time=query.value(2).toString();
        int weekday,classtime;
        getTime(time,weekday,classtime);
        courseno[classtime][weekday]=query.value(1).toString();
        coursename[classtime][weekday]=query.value(0).toString();
    }
}

QVariant CourseTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role!=Qt::DisplayRole)
        return QVariant();
    if(orientation==Qt::Vertical)
        return QString::number(section+1);
    else
    {
        switch (section)
        {
        case 0:
            return tr("周一");
            break;
        case 1:
            return tr("周二");
            break;
        case 2:
            return tr("周三");
            break;
        case 3:
            return tr("周四");
            break;
        case 4:
            return tr("周五");
            break;
        case 5:
            return tr("周六");
            break;
        }
    }
    return QVariant();
}

void CourseTableModel::getTime(const QString &time, int &weekday, int &classtime)
{
    if(QString(time[1])==QString("一"))
        weekday=0;
    if(QString(time[1])==QString("二"))
        weekday=1;
    if(QString(time[1])==QString("三"))
        weekday=2;
    if(QString(time[1])==QString("四"))
        weekday=3;
    if(QString(time[1])==QString("五"))
        weekday=4;
    if(QString(time[1])==QString("六"))
        weekday=5;
    classtime=QString(time[2]).toInt()-1;
}

