#include "hrelaypage.h"
#include "ui_relaypage.h"
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>
#include "hiconsymbol.h"
#include "hiconapi.h"
#include "hgraphhelper.h"
HRelayPage::HRelayPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::relayPage)
{
    ui->setupUi(this);
}

HRelayPage::~HRelayPage()
{
    delete ui;
    while(m_pGraphList.isEmpty())
    {
        delete (HGraph*)m_pGraphList.takeFirst();
    }
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
    //connect(ui->noFillRadio,&QRadioButton::clicked,this,&HRelayPage::onNoFillRadio_clicked);
    //connect(ui->clrFillRadio,&QRadioButton::clicked,this,&HRelayPage::onClrFillRadio_clicked);
    //connect(ui->picFillRadio,&QRadioButton::clicked,this,&HRelayPage::onPicFillRadio_clicked);
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
    btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->noFillRadio,0);
    btnGroup->addButton(ui->clrFillRadio,1);
    btnGroup->addButton(ui->picFillRadio,2);
    ui->noFillRadio->setChecked(true);
    connect(btnGroup,SIGNAL(buttonClicked(int)),this,SLOT(btnGroup_clicked(int)));


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
    ui->alignPicCombo->addItem(QStringLiteral("左"),0);
    ui->alignPicCombo->addItem(QStringLiteral("中"),1);
    ui->alignPicCombo->addItem(QStringLiteral("右"),2);
    ui->keepPicCheck->setChecked(false);
    ui->alignPicCombo->setEnabled(false);

    //实际情况设置
    if(pCurObj)
    {
        int nPointType = pCurObj->getObjType();
        if(nPointType == TEMPLATE_TYPE_RELAY)
        {
            ui->tabWidget->removeTab(0);//遥控就不需要光敏点的设置
        }
        else
        {
            HText* pTextObj = pCurObj->getIconSymbol()->getFirstTextObj();
            ui->textLineEdit->setText(pTextObj->getTextContent());
            strTextColor = pTextObj->getTextColorName(); //文字颜色
            QString strColor = QString("background-color:")+ strTextColor;
            ui->textClrBtn->setStyleSheet(strColor);
            ui->seeFrameCheck->setChecked(pTextObj->getFrameSee());
            quint8 id = pTextObj->getFillWay();
            if(id == 0){
                ui->noFillRadio->setChecked(true);
            }
            else if(id == 1){
                ui->clrFillRadio->setChecked(true);
            }
            else if(id == 2){
                ui->picFillRadio->setChecked(true);

            }
            emit btnGroup->buttonClicked(id);

            //字体设置
            font.setFamily(pTextObj->getTextFontName());
            font.setPointSize(pTextObj->getPointSize());
            font.setWeight(pTextObj->getWeigth());
            font.setItalic(pTextObj->getItalic());

            ui->transHSlider->setValue(pTextObj->getTransparency());
            int nFillStyle = ui->fillStyleCombo->findData(int(pTextObj->getFillStyle()));
            ui->fillStyleCombo->setCurrentIndex(nFillStyle);

            strFillColor = pTextObj->getFillColorName();
            strColor = QString("background-color:")+ strFillColor;
            ui->fillClrBtn->setStyleSheet(strColor);

            //图片部分
            ui->picLineEdit->setText(pTextObj->getImagePath());
            ui->keepPicCheck->setChecked(pTextObj->getKeepImageRatio());
            if(pTextObj->getKeepImageRatio())
            {
                ui->alignPicCombo->setEnabled(true);
                ui->alignPicCombo->setCurrentIndex(ui->alignPicCombo->findData(pTextObj->getImageDirect()));
            }  
        }
    }
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

    //填充方式
    relayGroup = new QButtonGroup(this);
    relayGroup->addButton(ui->openGraphRadio,MODE_OPEN_GRAPH);
    relayGroup->addButton(ui->picOperRadio,MODE_OPERATOR_GRAPH);
    relayGroup->addButton(ui->relayRadio,MODE_RELAY_CONTROL);
    ui->openGraphRadio->setChecked(true);
    ui->groupBox->setEnabled(false);
    ui->groupBox_2->setEnabled(true);
    connect(relayGroup,SIGNAL(buttonClicked(int)),this,SLOT(relayGroup_clicked(int)));

    //获取画面
    loadAllGraph();
    ui->openPicCombo->addItem("",(int)-1);
    for(int i = 0; i < m_pGraphList.count();i++)
    {
        HGraph* pGraph = (HGraph*)m_pGraphList[i];
        if(!pGraph) continue;
        QString strGraphName = pGraph->getGraphName();
        int nGraphID = pGraph->getGraphID();
        ui->openPicCombo->addItem(strGraphName,QVariant(nGraphID));
    }

    //控制属性
    if(pCurObj)
    {
        int nGraphID = pCurObj->getGraphID();
        uchar btGraphOperator = pCurObj->getGraphOpeartor();
        uchar btGraphComfirm = pCurObj->getGraphComfirm();
        emit relayGroup->buttonClicked(btGraphOperator);

        if(MODE_OPEN_GRAPH == btGraphOperator)//打开画面
            ui->openPicCombo->setCurrentIndex(ui->openPicCombo->findData(nGraphID));
        else if(MODE_OPERATOR_GRAPH == btGraphOperator)//打开画面模式
        {
            if(COMFIRM_MODE_STATION == btGraphComfirm)
            {
                ui->stationConfirmRadio->setChecked(true);
            }
            else if(COMFIRM_MODE_GRAPH == btGraphComfirm)
            {
                ui->graphComfirmRadio->setChecked(true);
            }
        }
    }
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

void HRelayPage::btnGroup_clicked(int id)
{
    ui->verticalLayout_5->setStretch(0,3);
    ui->verticalLayout_5->setStretch(1,3);
    ui->verticalLayout_5->setStretch(2,2);
    ui->verticalLayout_5->setStretch(3,3);
    ui->verticalLayout_5->setStretch(4,3);
    ui->verticalLayout_5->setStretch(5,1);
    if(id == 0)
    {
        ui->groupBox_7->setVisible(false);
        ui->groupBox_9->setVisible(false);
        ui->groupBox_10->setVisible(false);
        ui->verticalLayout_5->setStretch(0,3);
        ui->verticalLayout_5->setStretch(1,3);
        ui->verticalLayout_5->setStretch(5,9);
    }
    else if(id == 1)//图片填充
    {
        ui->groupBox_9->setVisible(true);
        ui->groupBox_7->setVisible(true);
        ui->groupBox_10->setVisible(false);
        ui->verticalLayout_5->setStretch(0,3);
        ui->verticalLayout_5->setStretch(1,3);
        ui->verticalLayout_5->setStretch(2,2);
        ui->verticalLayout_5->setStretch(3,3);
        ui->verticalLayout_5->setStretch(5,4);
    }
    else if(id == 2)
    {
        ui->groupBox_7->setVisible(true);
        ui->groupBox_9->setVisible(false);
        ui->groupBox_10->setVisible(true);
        ui->verticalLayout_5->setStretch(0,3);
        ui->verticalLayout_5->setStretch(1,3);
        ui->verticalLayout_5->setStretch(2,2);
        ui->verticalLayout_5->setStretch(4,3);
        ui->verticalLayout_5->setStretch(5,4);
    }
}

void HRelayPage::onNoFillRadio_clicked()
{
    ui->groupBox_7->setVisible(false);
    ui->groupBox_9->setVisible(false);
    ui->groupBox_10->setVisible(false);
    //ui->horizontalLayout_20->addItem(ui->verticalSpacer_6);

}

void HRelayPage::onClrFillRadio_clicked()
{
    ui->groupBox_7->setVisible(true);
    ui->groupBox_9->setVisible(true);
    ui->groupBox_10->setVisible(false);
    //ui->horizontalLayout_20->removeItem(ui->verticalSpacer_6);
   // ui->horizontalLayout_19->addItem(ui->verticalSpacer_5);
}

void HRelayPage::onPicFillRadio_clicked()
{
    ui->groupBox_7->setVisible(true);
    ui->groupBox_9->setVisible(true);
    ui->groupBox_10->setVisible(true);
    //ui->horizontalLayout_20->removeItem(ui->verticalSpacer_6);
    //ui->horizontalLayout_19->removeItem(ui->verticalSpacer_5);
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
    static QString path = "/";

    QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("打开图片"),path,"Images(*.png *.jpg *.bmp)",NULL,    QFileDialog::DontUseNativeDialog);
    if(!fileName.isEmpty() || !fileName.isNull())
    {
        ui->picLineEdit->setText(fileName);
        //画面路径要保存
    }
}

//图片显示方式
/*
void HRelayPage::onAlignPicCombo_clicked(int index)
{

}
*/

void HRelayPage::relayGroup_clicked(int id)
{
    if(id == MODE_OPEN_GRAPH) //打开画面
    {
        ui->groupBox->setEnabled(false);
        ui->groupBox_2->setEnabled(true);

    }
    else if(id == MODE_OPERATOR_GRAPH) //画面操作
    {
        ui->groupBox->setEnabled(true);
        ui->groupBox_2->setEnabled(false);
    }
    else if(id == MODE_RELAY_CONTROL)
    {
        ui->listWidget->setCurrentRow(1);
        //emit ui->listWidget->currentItemChanged(ui->listWidget->item(1),ui->listWidget->item(0));
    }
}

void HRelayPage::loadAllGraph()
{
    HGraphHelper::Instance()->loadAllGraph(&m_pGraphList);
}

void HRelayPage::onOk()
{
    HText *pTextObj = (HText*)pCurObj->getIconSymbol()->getFirstTextObj();

    //边框可见
    bool bFrameSee = false;
    if(ui->seeFrameCheck->checkState() == Qt::Checked)
        bFrameSee = true;
    pTextObj->setFrameSee(bFrameSee);

    //填充方式
    pTextObj->setFillWay(btnGroup->checkedId());
    pTextObj->setFillStyle((Qt::BrushStyle)ui->fillStyleCombo->currentData().toUInt());
    pTextObj->setFillColorName(strFillColor);

    //填充字体和颜色
    pTextObj->setTextFontName(font.family());
    pTextObj->setPointSize(font.pointSize());
    pTextObj->setWeight(font.weight());
    pTextObj->setItalic(font.italic());
    pTextObj->setTextColorName(strTextColor);
    pTextObj->setTextContent(ui->textLineEdit->text());


    if(btnGroup->checkedId() == 1)//当前是颜色填充就不能用画面填充，当前是图片填充且如果两种填充都有画面填充优于颜色填充
        pTextObj->setImagePath("");
    else
        pTextObj->setImagePath(ui->picLineEdit->text());
    bool bKeep = false;
    if(ui->keepPicCheck->checkState() == Qt::Checked)
        bKeep = true;
    pTextObj->setKeepImageRatio(bKeep);
    pTextObj->setImageDirect(ui->alignPicCombo->currentData().toUInt());

    uchar btMode = relayGroup->checkedId();
    if(MODE_OPEN_GRAPH == btMode)//打开画面
    {
        pCurObj->setGraphID(ui->openPicCombo->currentData().toInt());
    }
    else if(MODE_OPERATOR_GRAPH == btMode)//打开画面模式
    {
        if(ui->stationConfirmRadio->isChecked())
        {
            pCurObj->setGraphComfirm(COMFIRM_MODE_STATION);
        }
        else if(ui->stationConfirmRadio->isChecked())
        {
            ui->graphComfirmRadio->setChecked(true);
            pCurObj->setGraphComfirm(COMFIRM_MODE_GRAPH);
        }
        else
            pCurObj->setGraphComfirm((uchar)-1);
    }
    QDialog::accept();
}

void HRelayPage::onCancel()
{
    QDialog::reject();
}
