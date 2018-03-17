#include "hrelaypage.h"
#include "ui_relaypage.h"
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QColorDialog>
#include <QFontDialog>
#include "hiconsymbol.h"

HRelayPage::HRelayPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::relayPage)
{
    ui->setupUi(this);
}

HRelayPage::~HRelayPage()
{
    delete ui;
}

HRelayPage::HRelayPage(HBaseObj* pObj,QWidget *parent):
    QDialog(parent),ui(new Ui::relayPage)
{
    ui->setupUi(this);
    wStation = (ushort)-1;
    wPoint = (ushort)-1;
    wAttrib = 0;
    pCurObj = (HIconObj*)pObj;

    connect(ui->okBtn,SIGNAL(clicked()),this,SLOT(onOk()));
    connect(ui->cancelBtn,SIGNAL(clicked()),this,SLOT(onCancel()));

    initBaseProperty();
    initRelayPorperty();
    //遥控属性设置--{uuid}遥控
    setWindowTitle(QStringLiteral("遥控属性设置"));
}

void HRelayPage::initBaseProperty()
{
    //connect
    connect(ui->textClrBtn,&QPushButton::clicked,this,&HRelayPage::onTextClrBtn_clicked);
    connect(ui->fontBtn,&QPushButton::clicked,this,&HRelayPage::onFontBtn_clicked);
    connect(ui->moreBtn,&QPushButton::clicked,this,&HRelayPage::onMoreBtn_clicked);
    connect(ui->styleComboBox,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&HRelayPage::onStyleCombo_clicked);
    connect(ui->noFillRadio,&QRadioButton::clicked,this,&HRelayPage::onNoFillRadio_clicked);
    connect(ui->clrFillRadio,&QRadioButton::clicked,this,&HRelayPage::onClrFillRadio_clicked);
    connect(ui->picFillRadio,&QRadioButton::clicked,this,&HRelayPage::onPicFillRadio_clicked);
    connect(ui->transHSlider,&QSlider::valueChanged,ui->transSpinBox,&QSpinBox::setValue);
    connect(ui->transSpinBox,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),ui->transHSlider,&QSlider::setValue);
    //connect(ui->fillStyleCombo,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&HRelayPage::onFillStyleCombo_clicked);
    connect(ui->fillClrBtn,&QPushButton::clicked,this,&HRelayPage::onFillClrBtn_clicked);
    connect(ui->picBtn,&QPushButton::clicked,this,&HRelayPage::onPicSelect_clicked);
    //connect(ui->alignPicCombo,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this,&HRelayPage::onAlignPicCombo_clicked);

    //默认配置
    ui->textLineEdit->setText(QStringLiteral("Control Point"));

    /*************************颜色设置************************************/
    QString strColor = "#000000"; //文字颜色
    strTextColor = QString("background-color:")+ strColor; //文字颜色
    ui->textClrBtn->setStyleSheet(strTextColor);


    /*************************边框设置***************************************/
    ui->seeFrameCheck->setChecked(false);
    ui->styleComboBox->setEnabled(false);

    /*************************填充设置*************************************/
    //填充方式
    QButtonGroup* btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->noFillRadio,0);
    btnGroup->addButton(ui->clrFillRadio,1);
    btnGroup->addButton(ui->picFillRadio,2);
    ui->noFillRadio->setChecked(true);
    ui->groupBox_7->setVisible(false);
    ui->groupBox_9->setVisible(false);
    ui->groupBox_10->setVisible(false);


    //填充颜色
    strColor = "#000000"; //文字颜色
    strFillColor = QString("background-color:")+ strColor; //文字颜色
    ui->fillClrBtn->setStyleSheet(strTextColor);

    //填充方式
    ui->fillStyleCombo->clear();
    ui->fillStyleCombo->setIconSize(QSize(30,16));
    QStringList fillStyleList;
    fillStyleList<<QStringLiteral("无填充")<<QStringLiteral("实填充")<<QStringLiteral("密度1")<<QStringLiteral("密度2")
                 <<QStringLiteral("密度3")<<QStringLiteral("密度4")<<QStringLiteral("密度5")<<QStringLiteral("密度6")
                 <<QStringLiteral("密度7")<<QStringLiteral("水平线")<<QStringLiteral("垂直线")<<QStringLiteral("横平竖直线")
                 <<QStringLiteral("斜线")<<QStringLiteral("反斜线")<<QStringLiteral("交叉斜线")<<QStringLiteral("线性渐变")
                 <<QStringLiteral("径向渐变")<<QStringLiteral("锥形渐变");
    for(int i = 0; i < fillStyleList.count();i++)
    {
        int brushStyle = Qt::BrushStyle(i);
        ui->fillStyleCombo->addItem(createBrushStyleIcon(Qt::BrushStyle(i)),fillStyleList[i],brushStyle);
    }

    /***************************图片设置*************************************/
    ui->keepPicCheck->setChecked(false);
    ui->alignPicCombo->setEnabled(false);

    //实际情况设置
  /*  if(pCurObj)
    {
        HTextObj* pTextObj = pCurObj->getIconSymbol()->getFirstTextObj();
        ui->textLineEdit->setText(pTextObj->getTextContent());
        strTextColor = pTextObj->getTextColorName(); //文字颜色
        QString strColor = QString("background-color:")+ strTextColor;
        ui->textClrBtn->setStyleSheet(strColor);
        ui->seeFrameCheck->setChecked(pCurObj->getFrameSee());
        quint8 id = pCurObj->getFillWay();
        if(id == 0){
            ui->noFillRadio->setChecked(true);
            ui->groupBox_7->setVisible(false);
            ui->groupBox_9->setVisible(false);
            ui->groupBox_10->setVisible(false);
        }
        else if(id == 1){
            ui->clrFillRadio->setChecked(true);
            ui->groupBox_7->setVisible(true);
            ui->groupBox_9->setVisible(true);
            ui->groupBox_10->setVisible(false);
        }
        else if(id == 2){
            ui->picFillRadio->setChecked(true);
            ui->groupBox_7->setVisible(true);
            ui->groupBox_9->setVisible(false);
            ui->groupBox_10->setVisible(true);
            ui->groupBox_10->move(ui->groupBox_9->x(),ui->groupBox_9->y());
        }
        //emit btnGroup->buttonClicked(id);

        ui->transHSlider->setValue(pCurObj->getTransparency());
        int nFillStyle = ui->fillStyleCombo->findData(int(pCurObj->getFillStyle()));
        ui->fillStyleCombo->setCurrentIndex(nFillStyle);

        strFillColor = pCurObj->getFillColorName();
        strColor = QString("background-color:")+ strFillColor;
        ui->fillClrBtn->setStyleSheet(strColor);
    }*/

}

QIcon HRelayPage::createBrushStyleIcon(Qt::BrushStyle brushStyle)
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

void HRelayPage::initRelayPorperty()
{
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setIconSize(QSize(45,45));
    ui->listWidget->setMovement(QListView::Static);
    //ui->listWidget->setMaximumWidth(60);
    ui->listWidget->setSpacing(10);
    createIcons();
}

void HRelayPage::createIcons()
{
    QListWidgetItem* operBtn = new QListWidgetItem(ui->listWidget);
    operBtn->setText(QStringLiteral("画面操作"));
    operBtn->setIcon(QIcon(":/images/picoperator.png"));
    operBtn->setTextAlignment(Qt::AlignCenter);
    operBtn->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->listWidget->insertItem(0,operBtn);

    QListWidgetItem* relayBtn = new QListWidgetItem(ui->listWidget);
    relayBtn->setText(QStringLiteral("遥控操作"));
    relayBtn->setIcon(QIcon(":/images/runcontrol.png"));
    relayBtn->setTextAlignment(Qt::AlignCenter);
    relayBtn->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->listWidget->insertItem(1,relayBtn);
    connect(ui->listWidget,&QListWidget::currentItemChanged,this,&HRelayPage::changePage);

}

void HRelayPage::changePage(QListWidgetItem* curItem,QListWidgetItem* preItem)
{
    if(!curItem)
        curItem = preItem;
    ui->stackedWidget->setCurrentIndex(ui->listWidget->row(curItem));
}

void HRelayPage::onTextClrBtn_clicked()
{
    QColor curColor = QColor(Qt::black);
    curColor = QColor(strTextColor);
    const QColor color = QColorDialog::getColor(curColor, this, QStringLiteral("选择颜色"));
    if(!color.isValid()) return;
    strTextColor = color.name(QColor::HexRgb);
    ui->textClrBtn->setStyleSheet(QString("background-color:")+strTextColor);
}

void HRelayPage::onFontBtn_clicked()
{
    bool ok;
    HText* pTextObj = pCurObj->getIconSymbol()->getFirstTextObj();//获取文字信息
    font.setFamily(pTextObj->getTextFontName());
    font.setPointSize(pTextObj->getPointSize());
    font.setWeight(pTextObj->getWeigth());
    font.setItalic(pTextObj->getItalic());
    QFont newFont = QFontDialog::getFont(&ok,font,this,QStringLiteral("选择字体"));
    if(ok)
    {
        font = newFont;
    }
}

void HRelayPage::onMoreBtn_clicked()
{

}


void HRelayPage::onStyleCombo_clicked(int index)
{

}


void HRelayPage::onNoFillRadio_clicked()
{
    ui->groupBox_7->setVisible(false);
    ui->groupBox_9->setVisible(false);
    ui->groupBox_10->setVisible(false);
    ui->horizontalLayout_20->addItem(ui->verticalSpacer_6);

}

void HRelayPage::onClrFillRadio_clicked()
{
    ui->groupBox_7->setVisible(true);
    ui->groupBox_9->setVisible(true);
    ui->groupBox_10->setVisible(false);
    ui->horizontalLayout_20->removeItem(ui->verticalSpacer_6);
    ui->horizontalLayout_19->addItem(ui->verticalSpacer_5);
}

void HRelayPage::onPicFillRadio_clicked()
{
    ui->groupBox_7->setVisible(true);
    ui->groupBox_9->setVisible(true);
    ui->groupBox_10->setVisible(true);
    ui->horizontalLayout_20->removeItem(ui->verticalSpacer_6);
    ui->horizontalLayout_19->removeItem(ui->verticalSpacer_5);
    //ui->groupBox_10->move(ui->groupBox_9->x(),ui->groupBox_9->y());
}

/*
void HRelayPage::onTransHSlider_clicked()
{

}

void HRelayPage::onTransSpinBox_clicked()
{

}
*/

//填充方式
/*
void HRelayPage::onFillStyleCombo_clicked(int index)
{

}
*/

//填充颜色
void HRelayPage::onFillClrBtn_clicked()
{
    QColor curColor = QColor(Qt::black);
    curColor = QColor(strFillColor);
    const QColor color = QColorDialog::getColor(curColor, this, QStringLiteral("选择颜色"));
    if(!color.isValid()) return;
    strFillColor = color.name(QColor::HexRgb);
    ui->fillClrBtn->setStyleSheet(QString("background-color:")+strFillColor);
}

//图片选择
void HRelayPage::onPicSelect_clicked()
{

}

//图片显示方式
/*
void HRelayPage::onAlignPicCombo_clicked(int index)
{

}
*/
