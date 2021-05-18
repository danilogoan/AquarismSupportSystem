#ifndef CONNECTION_H
#define CONNECTION_H

#include <QDialog>

namespace Ui {
class Connection;
}

class Connection : public QDialog
{
    Q_OBJECT

public:
    explicit Connection(QWidget *parent = 0);
    ~Connection();
    void FillComboBox();

signals:
    void update_parent_values(quint16,quint16);

private slots:
    void on_buttonBox_accepted();

    void on_comboBox_currentTextChanged(const QString &arg1);

private:
    Ui::Connection *ui;
    QString description;
    quint16 vendorid;
    quint16 productid;
};

#endif // CONNECTION_H
