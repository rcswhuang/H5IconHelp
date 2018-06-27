#if defined(_MSC_VER) &&(_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "hiconproperty.h"
#include "ui_iconproperty.h"
#include "hiconapi.h"
#include <QColorDialog>
#include <QFontDialog>
#include <QVariant>
#include <QFile>
#include <QFileDialog>
#include <math.h>

HPropertyDlg::HPropertyDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IconProperty)
{
    ui->setupUi(this);

    /*QFile qss(":/qss/propertystylesheet.qss");
    if( qss.open(QFile::ReadOnly))
    {
       qDebug("open success");
       QString style = QLatin1String(qss.readAll());
       setStyleSheet(style);
       qss.close();
    }*/
    //setStyleSheet();
    setWindowTitle(QStringLiteral("test1"));

}

HPropertyDlg::HPropertyDlg(HBaseObj* pObj,QWidget *parent):
    QDialog(parent),ui(new Ui::IconProperty)
{
    ui->setupUi(this);
    btnGroup = new QButtonGroup;
    pCurObj = pObj;
    connect(ui->okBtn,SIGNAL(clicked(bool)),this,SLOT(ok_clicked()));
    connect(ui->noBtn,SIGNAL(clicked(bool)),this,SLOT(no_clicked()));
    initTab();

}


HPropertyDlg::~HPropertyDlg()
{
    delete ui;
}

void HPropertyDlg::setIconObj(HBaseObj* obj)
{
    btnGroup = new QButtonGroup;
    pCurObj = obj;
    connect(ui->okBtn,SIGNAL(clicked(bool)),this,SLOT(ok_clicked()));
    connect(ui->noBtn,SIGNAL(clicked(bool)),this,SLOT(no_clicked()));
    initTab();
}


void HPropertyDlg::initTab()
{
    ui->propertyTab->setMovable(false);
    initTextTab();
    initBaseTab();
    initLineTab();
    initShapeTab();
    DRAWSHAPE drawShape = pCurObj->getShapeType();
    ui->verticalLayout_18->setStretch(0,3);
    ui->verticalLayout_18->setStretch(1,3);
    ui->verticalLayout_18->setStretch(2,3);
    ui->verticalLayout_18->setStretch(3,3);
    ui->verticalLayout_18->setStretch(4,1);
    if(drawShape == enumText)
    {
        ui->groupBox_2->hide();
        ui->groupBox_8->hide();
        ui->verticalLayout_18->setStretch(0,3);
        ui->verticalLayout_18->setStretch(1,3);
        ui->verticalLayout_18->setStretch(4,7);
    }
    if(drawShape == enumLine)
    {
        ui->propertyTab->removeTab(1);//文字
        ui->propertyTab->removeTab(3);//形状
        ui->xCoord_width->setEnabled(false);
        ui->yCoord_height->setEnabled(false);
        ui->groupBox_8->hide();
        ui->groupBox_10->hide();
        ui->verticalLayout_18->setStretch(0,3);
        ui->verticalLayout_18->setStretch(1,3);
        ui->verticalLayout_18->setStretch(4,7);
    }
    else if(drawShape == enumRectangle)
    {
        ui->propertyTab->removeTab(1);//文字
        ui->groupBox_2->hide();
        ui->groupBox_8->hide();
        ui->verticalLayout_18->setStretch(0,3);
        ui->verticalLayout_18->setStretch(3,3);
        ui->verticalLayout_18->setStretch(4,7);
    }
    else if(drawShape == enumEllipse ||drawShape == enumCircle || drawShape == enumPolyline || drawShape == enumPolygon)
    {
        ui->propertyTab->removeTab(1);//文字
        ui->groupBox_2->hide();
        ui->groupBox_8->hide();
        ui->groupBox_10->hide();
        ui->verticalLayout_18->setStretch(0,3);
        ui->verticalLayout_18->setStretch(4,10);
        //ui->verticalLayout_18->setStretch(4,7);
    }
    else if(drawShape == enumArc || drawShape == enumPie)
    {
       ui->propertyTab->removeTab(1);//文字
       ui->groupBox_2->hide();
       ui->groupBox_10->hide();
       ui->verticalLayout_18->setStretch(0,3);
       ui->verticalLayout_18->setStretch(2,3);
       ui->verticalLayout_18->setStretch(4,7);
    }
    else if(drawShape == enumGroup)
    {
        ui->propertyTab->removeTab(3);
        ui->propertyTab->removeTab(2);
        ui->propertyTab->removeTab(1);
    }
}

void HPropertyDlg::initBaseTab()
{

    if(!pCurObj)
        return;
    ui->xCoord->setDecimals(2);
    ui->xCoord->setMinimum(-9999.99);
    ui->xCoord->setMaximum(9999.99);
    ui->yCoord->setDecimals(2);
    ui->yCoord->setMinimum(-9999.99);
    ui->yCoord->setMaximum(9999.99);
    ui->xCoord_width->setMaximum(9999.99);
    ui->yCoord_height->setMaximum(9999.99);

    //角度
    ui->x_rotate->setMinimum(-360);
    ui->x_rotate->setMaximum(360);
    ui->x_rotate->setSuffix(QStringLiteral("°"));
    ui->x_rotate->setValue(0);
    ui->objName->setText("");
    ui->objType->setText("无");

    if(pCurObj)
    {
        QString strObjName = pCurObj->getObjName();
        ui->objName->setText(strObjName);
        ui->x_rotate->setValue(pCurObj->getRotateAngle());
        ui->xCoord->setValue(pCurObj->getOX());
        ui->yCoord->setValue(pCurObj->getOY());
        ui->horizontalFlip->setChecked(pCurObj->bHorizonTurn);
        ui->verticalFlip->setChecked(pCurObj->bVerticalTurn);

        if(pCurObj->getShapeType() == DRAWSHAPE::enumLine)
        {
            ui->objType->setText(QStringLiteral("直线"));
            HLine* pObj = (HLine*)pCurObj;
        }
        else if(pCurObj->getShapeType() == DRAWSHAPE::enumRectangle)
        {
            ui->objType->setText(QStringLiteral("矩形"));
            HRectangle* pObj = (HRectangle*)pCurObj;

            ui->xCoord_width->setValue(pObj->getRectWidth());
            ui->yCoord_height->setValue(pObj->getRectHeight());
        }
        else if(pCurObj->getShapeType() == DRAWSHAPE::enumEllipse)
        {
            ui->objType->setText(QStringLiteral("椭圆"));
            HEllipse* pObj = (HEllipse*)pCurObj;

            ui->xCoord_width->setValue(pObj->getRectWidth());
            ui->yCoord_height->setValue(pObj->getRectHeight());
        }
        else if(pCurObj->getShapeType() == DRAWSHAPE::enumCircle)
        {
            ui->objType->setText(QStringLiteral("圆"));
            HCircle* pObj = (HCircle*)pCurObj;

            ui->xCoord_width->setValue(pObj->getRectWidth());
            ui->yCoord_height->setValue(pObj->getRectHeight());
        }
        else if(pCurObj->getShapeType() == DRAWSHAPE::enumPolygon)
        {
            ui->objType->setText(QStringLiteral("多边形"));
            HPolygon* pObj = (HPolygon*)pCurObj;

            ui->xCoord_width->setValue(pObj->width);
            ui->yCoord_height->setValue(pObj->height);
        }
        else if(pCurObj->getShapeType() == DRAWSHAPE::enumPolyline)
        {
            ui->objType->setText(QStringLiteral("折线"));
            HPolyline* pObj = (HPolyline*)pCurObj;

            ui->xCoord_width->setValue(pObj->width);
            ui->yCoord_height->setValue(pObj->height);
        }
        else if(pCurObj->getShapeType() == DRAWSHAPE::enumArc)
        {
            ui->objType->setText(QStringLiteral("弧线"));
            HArc* pObj = (HArc*)pCurObj;

            ui->xCoord_width->setValue(pObj->getRectWidth());
            ui->yCoord_height->setValue(pObj->getRectHeight());
        }
        else if(pCurObj->getShapeType() == DRAWSHAPE::enumPie)
        {
            ui->objType->setText(QStringLiteral("饼型"));
            HPie* pObj = (HPie*)pCurObj;

            ui->xCoord_width->setValue(pObj->getRectWidth());
            ui->yCoord_height->setValue(pObj->getRectHeight());
        }
        else if(pCurObj->getShapeType() == DRAWSHAPE::enumText)
        {
            ui->objType->setText(QStringLiteral("文字"));
            HText* pObj = (HText*)pCurObj;

            ui->xCoord_width->setValue(pObj->getRectWidth());
            ui->yCoord_height->setValue(pObj->getRectHeight());
        }
        else if(pCurObj->getShapeType() == DRAWSHAPE::enumGroup)
        {
            ui->objType->setText(QStringLiteral("组合"));
            HGroupObj* pObj = (HGroupObj*)pCurObj;

            ui->xCoord_width->setValue(pObj->getGroupWidth());
            ui->yCoord_height->setValue(pObj->getGroupHeight());
        }
    }
}

void HPropertyDlg::initTextTab()
{
    connect(ui->textFontBtn,SIGNAL(clicked(bool)),this,SLOT(textFont_clicked()));
    connect(ui->textColorBtn,SIGNAL(clicked(bool)),this,SLOT(textColor_clicked()));

    ui->horizAlignComboBox->clear();
    ui->horizAlignComboBox->addItem(QStringLiteral("左对齐"),Qt::AlignLeft);
    ui->horizAlignComboBox->addItem(QStringLiteral("居中对齐"),Qt::AlignHCenter);
    ui->horizAlignComboBox->addItem(QStringLiteral("右对齐"),Qt::AlignRight);

    ui->vertiAlignComboBox->addItem(QStringLiteral("顶端对齐"),Qt::AlignTop);
    ui->vertiAlignComboBox->addItem(QStringLiteral("垂直居中"),Qt::AlignVCenter);
    ui->vertiAlignComboBox->addItem(QStringLiteral("底部对齐"),Qt::AlignBottom);

    ui->directComboBox->addItem(QStringLiteral("不调整"),0);

    ui->layoutComboBox->addItem(QStringLiteral("空"),LAYOUT_TEXT_NULL);
    ui->layoutComboBox->addItem(QStringLiteral("充满"),LAYOUT_TEXT_FULL);
    //ui->layoutComboBox->addItem(QStringLiteral("横向"),LAYOUT_TEXT_HORIZON);
    //ui->layoutComboBox->addItem(QStringLiteral("纵向"),LAYOUT_TEXT_VERTICAL);

    font.setFamily(QStringLiteral("微软雅黑"));
    font.setPointSize(12);
    ui->textFontBtn->setFont(font);
    ui->textFontBtn->setText(QStringLiteral("示范"));

    strTextColor = "#000000";
    QString strbkColor = QString("background-color:")+ strTextColor;
    ui->textColorBtn->setStyleSheet(strbkColor);

    if(pCurObj)
    {
        DRAWSHAPE drawShape = pCurObj->getShapeType();
        if(drawShape == enumText)
        {
            HText* pTextObj = (HText*)pCurObj;
            ui->horizAlignComboBox->setCurrentIndex(ui->horizAlignComboBox->findData(pTextObj->getHorizontalAlign()));
            ui->vertiAlignComboBox->setCurrentIndex(ui->vertiAlignComboBox->findData(pTextObj->getVerticalAlign()));
            ui->layoutComboBox->setCurrentIndex(ui->layoutComboBox->findData(pTextObj->getLayout()));
            QString strFontName = pTextObj->getTextFontName();
            int pointSize = pTextObj->getPointSize();
            int weight = pTextObj->getWeigth();
            bool italic = pTextObj->getItalic();
            QFont font1(strFontName,pointSize,weight,italic);
            font.setFamily(strFontName);
            font.setPointSize(pointSize);
            font.setWeight(weight);
            font.setItalic(italic);
            ui->textFontBtn->setFont(font);
            ui->textFontBtn->setText(QStringLiteral("示范"));
            strTextColor = pTextObj->getTextColorName();
            QString textClr = QString("background-color:")+ strTextColor;
            ui->textColorBtn->setStyleSheet(textClr);

            ui->textEdit->setText(pTextObj->getTextContent());
        }
    }
}

void HPropertyDlg::initLineTab()
{
    connect(ui->lineColor,SIGNAL(clicked(bool)),this,SLOT(lineColor_clicked()));
    connect(ui->lineWidth,SIGNAL(currentIndexChanged(int)),this,SLOT(lineWidth_clicked()));
    connect(ui->lineStyle,SIGNAL(currentIndexChanged(int)),this,SLOT(lineStyle_clicked()));
    connect(ui->lineCapStyle,SIGNAL(currentIndexChanged(int)),this,SLOT(lineJoinStyle_clicked()));
    connect(ui->lineStartArrow,SIGNAL(currentIndexChanged(int)),this,SLOT(arrowHead_clicked()));
    connect(ui->lineTailArrow,SIGNAL(currentIndexChanged(int)),this,SLOT(arrowTail_clicked()));
    connect(ui->rectRound,SIGNAL(stateChanged(int)),this,SLOT(round_clicked()));
    //线条颜色
    //QString strLineColor;
    strLineColor = "#FF0000";
    if(pCurObj)
        strLineColor = pCurObj->getLineColorName();
    QString strbkColor = QString("background-color:")+ strLineColor;
    ui->lineColor->setStyleSheet(strbkColor);

    //线宽
    ui->lineWidth->clear();
    ui->lineWidth->setIconSize(QSize(30,16));
    QStringList strLineWidthList;
    strLineWidthList<<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8";
    for(int i = 0; i < strLineWidthList.count();i++)
        ui->lineWidth->addItem(createPenWidthIcon(strLineWidthList[i].toInt()),strLineWidthList[i],strLineWidthList[i].toInt());
    ui->lineWidth->setCurrentIndex(0);
    if(pCurObj)
    {
        int nLineWidthIndex = ui->lineWidth->findData(pCurObj->getLineWidth());
        ui->lineWidth->setCurrentIndex(nLineWidthIndex);
    }

    //线型
    ui->lineStyle->clear();
    ui->lineStyle->setIconSize(QSize(30,16));
    QStringList strLineStyleList;
    strLineStyleList<<QStringLiteral("实线")<<QStringLiteral("虚线")<<QStringLiteral("点线")
                   <<QStringLiteral("点-虚线")<<QStringLiteral("点-点-虚线");
    for(int i = 0; i < strLineStyleList.count();i++)
    {
        int penStyle = Qt::PenStyle(i+1);
        ui->lineStyle->addItem(createPenStyleIcon(Qt::PenStyle(i+1)),strLineStyleList[i],penStyle);
    }
    ui->lineStyle->setCurrentIndex(0);
    if(pCurObj)
    {
        int nLineStyleIndex = ui->lineStyle->findData((quint8)pCurObj->getLineStyle());
        ui->lineStyle->setCurrentIndex(nLineStyleIndex);
    }

    //线角
    ui->lineCapStyle->clear();
    ui->lineCapStyle->setIconSize(QSize(30,16));
    ui->lineCapStyle->addItem(createPenCapStyleIcon(Qt::FlatCap),QStringLiteral("平角"),Qt::FlatCap);
    ui->lineCapStyle->addItem(createPenCapStyleIcon(Qt::SquareCap),QStringLiteral("方角"),Qt::SquareCap);
    ui->lineCapStyle->addItem(createPenCapStyleIcon(Qt::RoundCap),QStringLiteral("圆角"),Qt::RoundCap);
    ui->lineCapStyle->setCurrentIndex(0);

    //箭头部分
    ui->lineStartArrow->clear();
    ui->lineStartArrow->setIconSize(QSize(30,16));
    ui->lineStartArrow->addItem(createArrowIcon(0,false),QStringLiteral("无箭头"),0);
    ui->lineStartArrow->addItem(createArrowIcon(1,false),QStringLiteral("箭头"),1);
    ui->lineStartArrow->addItem(createArrowIcon(2,false),QStringLiteral("空心三角"),2);
    ui->lineStartArrow->addItem(createArrowIcon(3,false),QStringLiteral("空心三角"),3);
    ui->lineStartArrow->setCurrentIndex(0);

    //尾部箭头部分
    ui->lineTailArrow->clear();
    ui->lineTailArrow->setIconSize(QSize(30,16));
    ui->lineTailArrow->addItem(createArrowIcon(0,true),QStringLiteral("无箭头"),0);
    ui->lineTailArrow->addItem(createArrowIcon(1,true),QStringLiteral("箭头"),1);
    ui->lineTailArrow->addItem(createArrowIcon(2,true),QStringLiteral("空心三角"),2);
    ui->lineTailArrow->addItem(createArrowIcon(3,true),QStringLiteral("空心三角"),3);
    ui->lineTailArrow->setCurrentIndex(0);

    //角度部分
    ui->startAngle->setMinimum(0);
    ui->startAngle->setMaximum(360);
    ui->startAngle->setSuffix(QStringLiteral("°"));
    ui->startAngle->setValue(30);

    ui->spanAngle->setMinimum(0);
    ui->spanAngle->setMaximum(360);
    ui->spanAngle->setSuffix(QStringLiteral("°"));
    ui->spanAngle->setValue(240);

    ui->bCloseCheck->setVisible(false);
    ui->bCloseCheck->setChecked(false);

    //弯曲度
    ui->rectRound->setChecked(false);
    ui->xAxis->setMinimum(0);
    ui->xAxis->setMaximum(100);
    ui->yAxis->setMinimum(0);
    ui->yAxis->setMaximum(100);
    ui->xAxis->setEnabled(false);
    ui->yAxis->setEnabled(false);
    if(pCurObj)
    {
        DRAWSHAPE drawShape = pCurObj->getShapeType();
        bool bRound = pCurObj->getRound();
        ui->rectRound->setChecked(bRound);
        if(bRound)
        {
            ui->xAxis->setEnabled(true);
            ui->yAxis->setEnabled(true);
            ui->xAxis->setValue(pCurObj->getXAxis());
            ui->yAxis->setValue(pCurObj->getYAxis());
        }

        if(drawShape == enumLine)
        {
            HLine* pLineObj = (HLine*)pCurObj;
            int curIndex = ui->lineStartArrow->findData(pLineObj->getArrowStart());
            ui->lineStartArrow->setCurrentIndex(curIndex);
            curIndex = ui->lineTailArrow->findData(pLineObj->getArrowEnd());
            ui->lineTailArrow->setCurrentIndex(curIndex);
            int w = pLineObj->getArrowWidth();
            int h = pLineObj->getArrowHeight();
            ui->arrowWidth->setValue(w);
            ui->arrowHeight->setValue(h);
        }
        else if(pCurObj->getShapeType() == enumArc)
        {
            ui->bCloseCheck->setVisible(true);
            HArc* pArcObj = (HArc*)pCurObj;
            ui->startAngle->setValue(pArcObj->getStartAngle());
            ui->spanAngle->setValue(pArcObj->getSpanAngle());
            ui->bCloseCheck->setChecked(pArcObj->getCloseStatus());
        }
        else if(pCurObj->getShapeType() == enumPie)
        {
            HPie* pPieObj = (HPie*)pCurObj;
            ui->startAngle->setValue(pPieObj->getStartAngle());
            ui->spanAngle->setValue(pPieObj->getSpanAngle());
        }
    }
}

void HPropertyDlg::initShapeTab()
{
    connect(ui->transSlider,SIGNAL(valueChanged(int)),ui->transSpix,SLOT(setValue(int)));
    connect(ui->transSpix,SIGNAL(valueChanged(int)),ui->transSlider,SLOT(setValue(int)));
    connect(ui->fillStyle,SIGNAL(currentIndexChanged(int)),this,SLOT(fillStyle_clicked()));
    connect(ui->fillColor,SIGNAL(clicked(bool)),this,SLOT(fillColor_clicked()));
    connect(ui->fillDirection,SIGNAL(currentIndexChanged(int)),this,SLOT(fillDirection_clicked()));
    connect(ui->fillPercentage,SIGNAL(valueChanged(int)),this,SLOT(fillPercentage_clicked()));
    connect(btnGroup,SIGNAL(buttonClicked(int)),this,SLOT(btnGroup_clicked(int)));
    connect(ui->picBtn,SIGNAL(clicked(bool)),this,SLOT(picBtn_clicked()));
    //边框可见
    ui->frameSee->setChecked(true);

    //填充方式
    btnGroup->addButton(ui->noFill,0);
    btnGroup->addButton(ui->colorFill,1);
    btnGroup->addButton(ui->imageFill,2);
    ui->noFill->setChecked(true);

    //透明度
    ui->transSlider->setMinimum(0);
    ui->transSlider->setMaximum(100);
    ui->transSpix->setSuffix("%");
    ui->transSpix->setMinimum(0);
    ui->transSpix->setMaximum(100);
    ui->transSlider->setValue(0);

    //填充方式
    ui->fillStyle->clear();
    ui->fillStyle->setIconSize(QSize(30,16));
    QStringList fillStyleList;
    fillStyleList<<QStringLiteral("无填充")<<QStringLiteral("实填充")<<QStringLiteral("密度1")<<QStringLiteral("密度2")
                 <<QStringLiteral("密度3")<<QStringLiteral("密度4")<<QStringLiteral("密度5")<<QStringLiteral("密度6")
                 <<QStringLiteral("密度7")<<QStringLiteral("水平线")<<QStringLiteral("垂直线")<<QStringLiteral("横平竖直线")
                 <<QStringLiteral("斜线")<<QStringLiteral("反斜线")<<QStringLiteral("交叉斜线")<<QStringLiteral("线性渐变")
                 <<QStringLiteral("径向渐变")<<QStringLiteral("锥形渐变");
    for(int i = 0; i < fillStyleList.count();i++)
    {
        int brushStyle = Qt::BrushStyle(i);
        ui->fillStyle->addItem(createBrushStyleIcon(Qt::BrushStyle(i)),fillStyleList[i],brushStyle);
    }

    //填充颜色
    //QString strFillColor;
    strFillColor = "#FF0000";

    //填充比例
    ui->fillPercentage->setSuffix("%");
    ui->fillPercentage->setMinimum(0);
    ui->fillPercentage->setMaximum(100);
    ui->fillPercentage->setValue(100);

    //填充方向
    //ui->fillDirection->addItem(QStringLiteral("无方向"),0);
    ui->fillDirection->addItem(QStringLiteral("由底到顶"),DIRECT_BOTTOM_TO_TOP);
    ui->fillDirection->addItem(QStringLiteral("由顶到底"),DIRECT_TOP_TO_BOTTOM);
    ui->fillDirection->addItem(QStringLiteral("由左到右"),DIRECT_LEFT_TO_RIGHT);
    ui->fillDirection->addItem(QStringLiteral("由右到左"),DIRECT_RIGHT_TO_LEFT);
    ui->fillDirection->addItem(QStringLiteral("垂直到外"),DIRECT_VER_TO_OUT);
    ui->fillDirection->addItem(QStringLiteral("水平到外"),DIRECT_HORi_TO_OUT);
    ui->fillDirection->addItem(QStringLiteral("垂直到里"),DIRECT_VER_TO_IN);
    ui->fillDirection->addItem(QStringLiteral("水平到里"),DIRECT_HORI_TO_IN);
    ui->fillDirection->setEnabled(false);

    ui->alignPicCombo->addItem(QStringLiteral("左"),0);
    ui->alignPicCombo->addItem(QStringLiteral("中"),1);
    ui->alignPicCombo->addItem(QStringLiteral("右"),2);
    ui->keepPicCheck->setChecked(false);
    ui->alignPicCombo->setEnabled(false);
    //实际显示
    if(pCurObj)
    {
        ui->frameSee->setChecked(pCurObj->getFrameSee());
        quint8 id = pCurObj->getFillWay();
        if(id == 0)
            ui->noFill->setChecked(true);
        else if(id == 1)
            ui->colorFill->setChecked(true);
        else if(id == 2)
            ui->imageFill->setChecked(true);
        emit btnGroup->buttonClicked(id);

        ui->transSlider->setValue(pCurObj->getTransparency());
        int nFillStyle = ui->fillStyle->findData(int(pCurObj->getFillStyle()));
        ui->fillStyle->setCurrentIndex(nFillStyle);

        strFillColor = pCurObj->getFillColorName();
        QString strbgColor = QString("background-color:")+ strFillColor;
        ui->fillColor->setStyleSheet(strbgColor);
        ui->fillPercentage->setValue(pCurObj->getFillPercentage());
        if(nFillStyle == Qt::LinearGradientPattern)
        {
            ui->fillDirection->setEnabled(true);
            int nFillDirect = ui->fillDirection->findData(pCurObj->getFillDirection());
            ui->fillDirection->setCurrentIndex(nFillDirect);
        }

        //图片部分
        ui->picLineEdit->setText(pCurObj->getImagePath());
        ui->keepPicCheck->setChecked(pCurObj->getKeepImageRatio());
        if(pCurObj->getKeepImageRatio())
        {
            ui->alignPicCombo->setEnabled(true);
            ui->alignPicCombo->setCurrentIndex(ui->alignPicCombo->findData(pCurObj->getImageDirect()));
        }
    }
}


QIcon HPropertyDlg::createPenWidthIcon(int width)
{
    QPixmap pixmap(30,16);
    QPainter painter(&pixmap);
    painter.fillRect(QRect(0,0,30,16),Qt::white);
    QPen pen;
    pen.setWidthF(width);
    painter.setPen(pen);
    painter.drawLine(0,8,30,8);
    return QIcon(pixmap);
}

QIcon HPropertyDlg::createPenStyleIcon(Qt::PenStyle style)
{
    QPixmap pixmap(30,16);
    QPainter painter(&pixmap);
    painter.fillRect(QRect(0,0,30,16),Qt::white);
    QPen pen;
    pen.setStyle(style);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawLine(0,8,30,8);

    return QIcon(pixmap);
}


QIcon HPropertyDlg::createPenCapStyleIcon(Qt::PenCapStyle capStyle)
{
    QPixmap pixmap(30,16);
    QPainter painter(&pixmap);
    painter.fillRect(QRect(0,0,30,16),Qt::white);
    QPen pen(capStyle);
    pen.setWidth(5);
    pen.setCapStyle(capStyle);
    painter.setPen(pen);
    painter.drawLine(5,8,25,8);

    return QIcon(pixmap);
}

QIcon HPropertyDlg::createBrushStyleIcon(Qt::BrushStyle brushStyle)
{
    QPixmap pixmap(30,16);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    if(brushStyle == Qt::LinearGradientPattern)
    {
        QLinearGradient linearGradient(0,0,30,16);
        linearGradient.setColorAt(0.0,Qt::white);
        linearGradient.setColorAt(1.0,Qt::black);
        painter.setBrush(linearGradient);
    }
    else if(brushStyle == Qt::RadialGradientPattern)
    {
        QRadialGradient radialGradient(15,8,30,15,8);
        radialGradient.setColorAt(0.0,Qt::white);
        radialGradient.setColorAt(1.0,Qt::black);
        painter.setBrush(radialGradient);
    }
    else if(brushStyle == Qt::ConicalGradientPattern)
    {
        QConicalGradient conicalGradient(15,8,0);
        conicalGradient.setColorAt(0.0,Qt::white);
        conicalGradient.setColorAt(1.0,Qt::black);
        painter.setBrush(conicalGradient);
    }
    else
        painter.setBrush(brushStyle);
    painter.drawRect(0,0,29,15);
    return QIcon(pixmap);
}

//线条颜色
void HPropertyDlg::lineColor_clicked()
{
    QColor curColor = QColor(Qt::white);
    if(pCurObj)
    {
        strLineColor = pCurObj->getLineColorName();
        curColor = QColor(strLineColor);
    }
    const QColor color = QColorDialog::getColor(curColor, this, QStringLiteral("选择颜色"));
    if(!color.isValid()) return;
    strLineColor = color.name(QColor::HexRgb);
    ui->lineColor->setStyleSheet(QString("background-color:")+strLineColor);
}

//箭头
QIcon HPropertyDlg::createArrowIcon(quint8 style,bool head)
{
    QPixmap pixmap(30,16);
    QPainter painter(&pixmap);
    painter.fillRect(QRect(0,0,30,16),Qt::white);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QPoint arrowStart(2,8);
    QPoint arrowEnd(28,8);
    QLine line(arrowStart,arrowEnd);
    int len = 5;
    switch(style)
    {
    case 0:
     {
        painter.drawLine(line);
        break;
     }
    case 1: //单箭头
     {
        painter.drawLine(arrowStart,arrowEnd);
        painter.drawLine(arrowStart,QPoint(arrowStart.x()+2*len,arrowStart.y()-len));
        painter.drawLine(arrowStart,QPoint(arrowStart.x()+2*len,arrowStart.y()+len));
        break;
     }
    case 2: //空心箭头
     {
        painter.drawLine(QPoint(arrowStart.x()+2*len,arrowStart.y()),arrowEnd);
        painter.drawLine(arrowStart,QPoint(arrowStart.x()+2*len,arrowStart.y()-len));
        painter.drawLine(arrowStart,QPoint(arrowStart.x()+2*len,arrowStart.y()+len));
        painter.drawLine(QPoint(arrowStart.x()+2*len,arrowStart.y()-len),QPoint(arrowStart.x()+2*len,arrowStart.y()+len));
        break;
     }
    case 3: //实心箭头
     {
        painter.drawLine(arrowStart,arrowEnd);
        QPolygon points;
        points.append(arrowStart);
        points.append(QPoint(arrowStart.x()+2*len,arrowStart.y()-len));
        points.append(QPoint(arrowStart.x()+2*len,arrowStart.y()+len));
        painter.save();
        painter.setBrush(Qt::black);
        painter.drawPolygon(points);
        painter.restore();
        break;
     }
    }
    if(!head)
    {
        QTransform matrix;
        matrix.scale(-1,1);
        QPixmap pix = pixmap.transformed(matrix);
        return QIcon(pix);
    }
    return QIcon(pixmap);
}

void HPropertyDlg::ok_clicked()
{
    DRAWSHAPE drawShape = pCurObj->getShapeType();
    pCurObj->setLineColorName(strLineColor);
    pCurObj->setLineWidth(ui->lineWidth->currentData().toUInt());
    pCurObj->setLineStyle(Qt::PenStyle(ui->lineStyle->currentData().toInt()));
    pCurObj->setLineCapStyle(Qt::PenCapStyle(ui->lineCapStyle->currentData().toInt()));
    bool bTurn = false;
    if(ui->verticalFlip->checkState() == Qt::Checked)
    {
        bTurn = true;
        pCurObj->setTurn(false,bTurn);
    }
    bTurn = false;
    if(ui->horizontalFlip->checkState() == Qt::Checked)
    {
        bTurn = true;
        pCurObj->setTurn(bTurn,false);
    }
    bool bFrameSee = false;
    if(ui->frameSee->checkState() == Qt::Checked)
        bFrameSee = true;
    pCurObj->setFrameSee(bFrameSee);
    pCurObj->setFillWay(btnGroup->checkedId());
    pCurObj->setTransparency(ui->transSlider->value());
    pCurObj->setFillPercentage(ui->fillPercentage->value());
    pCurObj->setFillStyle((Qt::BrushStyle)ui->fillStyle->currentData().toUInt());
    pCurObj->setFillColorName(strFillColor);
    //quint8 tt = ui->fillDirection->currentData().toUInt();
    pCurObj->setFillDirection(ui->fillDirection->currentData().toUInt());
    pCurObj->setRotateAngle(ui->x_rotate->value());
    bool bRound = false;
    if(ui->rectRound->checkState() == Qt::Checked)
        bRound = true;
    pCurObj->setRound(bRound);
    pCurObj->setXAxis(ui->xAxis->value());
    pCurObj->setYAxis(ui->yAxis->value());
    if(btnGroup->checkedId() == 1)//当前是颜色填充就不能用画面填充，当前是图片填充且如果两种填充都有画面填充优于颜色填充
        pCurObj->setImagePath("");
    else
        pCurObj->setImagePath(ui->picLineEdit->text());
    bool bKeep = false;
    if(ui->keepPicCheck->checkState() == Qt::Checked)
        bKeep = true;
    pCurObj->setKeepImageRatio(bKeep);
    pCurObj->setImageDirect(ui->alignPicCombo->currentData().toUInt());
    if(drawShape == enumLine)
    {
        HLine* pLineObj = (HLine*)pCurObj;
        pLineObj->setArrowStart(ui->lineStartArrow->currentData().toUInt());
        pLineObj->setArrowEnd(ui->lineTailArrow->currentData().toUInt());
        pLineObj->setArrowWidth(ui->arrowWidth->value());
        pLineObj->setArrowHeight(ui->arrowHeight->value());
    }
    else if(drawShape == enumArc)
    {
        HArc* pArcObj = (HArc*)pCurObj;
        pArcObj->setStartAngle(ui->startAngle->value());
        pArcObj->setSpanAngle(ui->spanAngle->value());
        pArcObj->setCloseStatus(false);
        if(ui->bCloseCheck->checkState() == Qt::Checked)
            pArcObj->setCloseStatus(true);
    }
    else if(drawShape == enumPie)
    {
        HPie* pPieObj = (HPie*)pCurObj;
        pPieObj->setStartAngle(ui->startAngle->value());
        pPieObj->setSpanAngle(ui->spanAngle->value());
    }
    else if(drawShape == enumText)
    {
        HText *pTextObj = (HText*)pCurObj;
        pTextObj->setHorizontalAlign(ui->horizAlignComboBox->currentData().toInt());
        pTextObj->setVerticalAlign(ui->vertiAlignComboBox->currentData().toInt());
        pTextObj->setLayout(ui->layoutComboBox->currentData().toUInt());
        pTextObj->setTextFontName(font.family());
        pTextObj->setPointSize(font.pointSize());
        pTextObj->setWeight(font.weight());
        pTextObj->setItalic(font.italic());
        pTextObj->setTextColorName(strTextColor);
        pTextObj->setTextContent(ui->textEdit->text());
    }
    pCurObj->setModify(true);

    QDialog::accept();
}

void HPropertyDlg::no_clicked()
{
    QDialog::reject();
}

void HPropertyDlg::btnGroup_clicked(int id)
{
    ui->verticalLayout_18->setStretch(0,3);
    ui->verticalLayout_18->setStretch(1,2);
    ui->verticalLayout_18->setStretch(2,3);
    ui->verticalLayout_18->setStretch(3,3);
    ui->verticalLayout_18->setStretch(4,1);
    if(id == 0)
    {
        ui->groupBox_6->setVisible(false);
        ui->groupBox_7->setVisible(false);
        ui->groupBox_12->setVisible(false);
        ui->verticalLayout_18->setStretch(0,3);
        ui->verticalLayout_18->setStretch(4,9);
    }
    else if(id == 1)//图片填充
    {
        ui->groupBox_6->setVisible(true);
        ui->groupBox_7->setVisible(true);
        ui->groupBox_12->setVisible(false);
        ui->verticalLayout_18->setStretch(0,3);
        ui->verticalLayout_18->setStretch(1,2);
        ui->verticalLayout_18->setStretch(2,3);
        ui->verticalLayout_18->setStretch(4,4);
    }
    else if(id == 2)
    {
        ui->groupBox_6->setVisible(true);
        ui->groupBox_7->setVisible(false);
        ui->groupBox_12->setVisible(true);
        ui->verticalLayout_18->setStretch(0,3);
        ui->verticalLayout_18->setStretch(1,2);
        ui->verticalLayout_18->setStretch(3,3);
        ui->verticalLayout_18->setStretch(4,4);
    }
}

void HPropertyDlg::lineWidth_clicked()
{

}

void HPropertyDlg::lineStyle_clicked()
{

}

void HPropertyDlg::lineJoinStyle_clicked()
{

}

void HPropertyDlg::frameSee_clicked()
{

}

void HPropertyDlg::fillWay_clicked()
{
    if(ui->noFill->isChecked())
    {
        ui->groupBox_6->setVisible(false);
        ui->groupBox_7->setVisible(false);
        ui->groupBox_12->setVisible(false);
    }
}

void HPropertyDlg::fillStyle_clicked()
{
    int nCurrentIndex = ui->fillStyle->currentIndex();
    int nFillStyle = ui->fillStyle->itemData(nCurrentIndex).toInt();
    if(nFillStyle == Qt::LinearGradientPattern)
    {
        ui->fillDirection->setEnabled(true);
        int nFillDirect = ui->fillDirection->findData(pCurObj->getFillDirection());
        ui->fillDirection->setCurrentIndex(nFillDirect);
    }
    else
    {
        ui->fillDirection->setCurrentIndex(0);
        ui->fillDirection->setEnabled(false);
    }
}

void HPropertyDlg::fillColor_clicked()
{
    QColor curColor = QColor(Qt::white);
    if(pCurObj)
    {
        strFillColor = pCurObj->getFillColorName();
        curColor = QColor(strFillColor);
    }
    const QColor color = QColorDialog::getColor(curColor, this, QStringLiteral("选择颜色"));
    if(!color.isValid()) return;
    strFillColor = color.name(QColor::HexRgb);
    ui->fillColor->setStyleSheet(QString("background-color:")+strFillColor);
}

void HPropertyDlg::fillDirection_clicked()
{

}

void HPropertyDlg::fillPercentage_clicked()
{

}

void HPropertyDlg::transSlider_clicked()
{

}

void HPropertyDlg::transSpix_clicked()
{

}

void HPropertyDlg::arrowHead_clicked()
{
    int nStartIndex = ui->lineStartArrow->currentIndex();
    int nEndIndex = ui->lineTailArrow->currentIndex();
    if(nStartIndex == 0 && nEndIndex == 0)
    {
        ui->arrowWidth->setEnabled(false);
        ui->arrowHeight->setEnabled(false);
    }
    else
    {
        ui->arrowWidth->setEnabled(true);
        ui->arrowHeight->setEnabled(true);
    }
}

void HPropertyDlg::arrowTail_clicked()
{
    int nStartIndex = ui->lineStartArrow->currentIndex();
    int nEndIndex = ui->lineTailArrow->currentIndex();
    if(nStartIndex == 0 && nEndIndex == 0)
    {
        ui->arrowWidth->setEnabled(false);
        ui->arrowHeight->setEnabled(false);
    }
    else
    {
        ui->arrowWidth->setEnabled(true);
        ui->arrowHeight->setEnabled(true);
    }
}

void HPropertyDlg::textColor_clicked()
{
    QColor curColor = QColor(strTextColor);
    const QColor color = QColorDialog::getColor(curColor, this, QStringLiteral("选择颜色"));
    if(!color.isValid()) return;
    strTextColor = color.name(QColor::HexRgb);
    ui->textColorBtn->setStyleSheet(QString("background-color:")+strTextColor);
}

void HPropertyDlg::textFont_clicked()
{
    bool ok;
    QFont newFont = QFontDialog::getFont(&ok,font,this,QStringLiteral("选择字体"));
    if(ok)
    {
        font = newFont;
        ui->textFontBtn->setFont(font);
        ui->textFontBtn->setText(QStringLiteral("示范"));
    }
}

void HPropertyDlg::round_clicked()
{
    if(!ui->rectRound->isChecked())
    {
        ui->xAxis->setEnabled(false);
        ui->yAxis->setEnabled(false);
    }
    else
    {
        ui->xAxis->setEnabled(true);
        ui->yAxis->setEnabled(true);
    }
}

void HPropertyDlg::xAxis_clicked()
{

}

void HPropertyDlg::yAxis_clicked()
{

}

void HPropertyDlg::picBtn_clicked()
{
    static QString path;

    QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("打开图片"),path,"Images(*.png *.jpg *.bmp)",NULL,    QFileDialog::DontUseNativeDialog);
    if(!fileName.isEmpty() || !fileName.isNull())
    {
        ui->picLineEdit->setText(fileName);
        //画面路径要保存
    }
}
