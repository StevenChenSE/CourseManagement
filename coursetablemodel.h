#ifndef COURSETABLEMODEL_H
#define COURSETABLEMODEL_H
#include "qtheaders.h"

class CourseTableModel:public QAbstractTableModel
{
    Q_OBJECT
public:
    CourseTableModel(QObject* parent=NULL,const QString & studentno="");
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index=QModelIndex(), int role=Qt::DisplayRole) const;
    void initQuery();
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
private:
    QString studentNo;
    QString courseno[6][6];
    QString coursename[6][6];
    void getTime(const QString & time,int & weekday,int & classtime);

};

#endif // COURSETABLEMODEL_H
