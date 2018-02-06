#ifndef HPROPERTYDLG_H
#define HPROPERTYDLG_H

#include <QDialog>
#include <QAbstractItemDelegate>
#include <QGraphicsItem>
#include <QButtonGroup>
#include "hbaseobj.h"
#include "hiconobj.h"
#include "hiconrectobj.h"
#include "hgroupobj.h"
namespace Ui {
class PropertyDlg;
}

/*
class QPenStyleDelegate:public QAbstractItemDelegate
{
    Q_OBJECT
public:
    QPenStyleDelegate(QObject* parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};*/




class HPropertyDlg : public QDialog
{
    Q_OBJECT

public:
    HPropertyDlg(QWidget *parent = 0);
    HPropertyDlg(HBaseObj* pObj,QWidget *parent = 0);
    ~HPropertyDlg();
public:
    void setIconObj(HBaseObj* obj);
    void initTab();
    void initTextTab();
    void initBaseTab();
    void initLineTab();
    void initShapeTab();
    QIcon createPenWidthIcon(int width);
    QIcon createPenStyleIcon(Qt::PenStyle style);
    QIcon createPenCapStyleIcon(Qt::PenCapStyle capStyle);
    QIcon createBrushStyleIcon(Qt::BrushStyle brushStyle);
    QIcon createArrowIcon(quint8 style,bool head);
public slots:
    void ok_clicked();
    void no_clicked();
    void btnGroup_clicked(int id);

    void lineColor_clicked();
    void lineWidth_clicked();
    void lineStyle_clicked();
    void lineJoinStyle_clicked();
    void arrowHead_clicked();
    void arrowTail_clicked();

    void frameSee_clicked();
    void fillWay_clicked();
    void fillStyle_clicked();
    void fillColor_clicked();
    void fillDirection_clicked();
    void fillPercentage_clicked();
    void transSlider_clicked();
    void transSpix_clicked();

    void textFont_clicked();
    void textColor_clicked();

    void round_clicked();
    void xAxis_clicked();
    void yAxis_clicked();
private:
    QString strLineColor;//线条颜色
    QString strFillColor;//填充颜色
    QString strTextColor;//文字颜色
    QFont font;
    HBaseObj* pCurObj;
    Ui::PropertyDlg *ui;
    QButtonGroup *btnGroup;
};

#endif // HPROPERTYDLG_H
