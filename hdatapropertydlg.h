#ifndef HDATAPROPERTYDLG_H
#define HDATAPROPERTYDLG_H

#include <QDialog>

namespace Ui {
class HDataPropertyDlg;
}

class HDataPropertyDlg : public QDialog
{
    Q_OBJECT

public:
    explicit HDataPropertyDlg(QWidget *parent = 0);
    ~HDataPropertyDlg();

private:
    Ui::HDataPropertyDlg *ui;
};

#endif // HDATAPROPERTYDLG_H
