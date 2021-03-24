#ifndef EDIT_TEMPERATURE_DIALOG_H
#define EDIT_TEMPERATURE_DIALOG_H

#include <QDialog>

namespace Ui {
class Edit_Temperature_Dialog;
}

class Edit_Temperature_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Edit_Temperature_Dialog(QWidget *parent = 0);
    ~Edit_Temperature_Dialog();
    void set_Max_Min_Value(float,float);

signals:
    void update_parent_values(QString,QString);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Edit_Temperature_Dialog *ui;
};

#endif // EDIT_TEMPERATURE_DIALOG_H
