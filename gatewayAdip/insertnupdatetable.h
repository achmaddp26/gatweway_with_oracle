#ifndef INSERTNUPDATETABLE_H
#define INSERTNUPDATETABLE_H

#include <QDialog>

namespace Ui {
class insertNUpdateTable;
}

class insertNUpdateTable : public QDialog
{
    Q_OBJECT

public:
    explicit insertNUpdateTable(QWidget *parent = nullptr);
    ~insertNUpdateTable();

private:
    Ui::insertNUpdateTable *ui;
};

#endif // INSERTNUPDATETABLE_H
