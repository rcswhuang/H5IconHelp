#ifndef HICONPROPERTY_H
#define HICONPROPERTY_H

#include <QDialog>

namespace Ui {
class HIconProperty;
}

class HIconProperty : public QDialog
{
    Q_OBJECT

public:
    explicit HIconProperty(QWidget *parent = 0);
    ~HIconProperty();

private:
    Ui::HIconProperty *ui;
};

#endif // HICONPROPERTY_H
