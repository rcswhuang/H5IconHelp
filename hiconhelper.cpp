#include "hiconhelper.h"
#include "hicontemplate.h"
#include "hbaseobj.h"
#include "hiconobj.h"
#include <QProcessEnvironment>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include "publicdata.h"
HIconHelper::HIconHelper()
{

}

HIconHelper* HIconHelper::m_pInstance = NULL;

HIconHelper* HIconHelper::Instance()
{
    if(!m_pInstance)
    {
        m_pInstance = new HIconHelper();
    }
    return m_pInstance;
}

QPixmap HIconHelper::iconPixmap(const QString& strType,const QString& uuid,const QSizeF &size,int nCurPattern)
{
    HIconTemplate* pIconTemplate = new HIconTemplate(QString());
    Q_ASSERT( pIconTemplate );

    char szIconPath[128];
    getDataFilePath(DFPATH_ICON,szIconPath);
    QString iconsPath = QString(szIconPath);
    QString path = iconsPath +"/" + strType + "/" + uuid + ".xic";
    if ( uuid.isEmpty() && strType.contains(".xic") )
    {
        path = strType;
    }
    pIconTemplate->readXml(path);

    QSizeF sizeF = size;
    if(sizeF.width() == 0 || sizeF.height() == 0)
        sizeF = pIconTemplate->getDefaultSize();
    QImage image(sizeF.width()+1.00,sizeF.height(),QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    HIconObj* pObj = new HIconObj(pIconTemplate);
    QSizeF pt = pIconTemplate->getDefaultSize();
    if(!pObj->getIconSymbol() || pObj->getIconSymbol()->getMaxPattern() <= nCurPattern)
        return QPixmap();

    pObj->getIconSymbol()->setCurrentPattern(nCurPattern);
    double w1 = sizeF.width()/(pt.width()*20);
    double h1 = sizeF.height()/(pt.height()*20);
    pObj->resize(w1,h1);
    //应该还有一个move
    painter.translate(sizeF.width()/2.0,sizeF.height()/2.0);
    pObj->paint(&painter);
    delete pObj;
    if(!pIconTemplate)
    {
        delete pIconTemplate;
        pIconTemplate = NULL;
    }

    return QPixmap().fromImage(image);
}


void HIconHelper::loadIconDoucument(QList<HIconTemplate*> *pIconTemplateList)
{
    //先找路径，在找文件夹，然后文件夹里面搜索添加完成
    if(NULL == pIconTemplateList)
        return;
    char szIconPath[128];
    getDataFilePath(DFPATH_ICON,szIconPath);
    QString iconsPath = QString(szIconPath);

    QDir dirIconsPath(iconsPath);
    if(!dirIconsPath.exists())
        return;
    QFileInfoList iconsFileInfoList = dirIconsPath.entryInfoList(QDir::Dirs  | QDir::NoDotAndDotDot);
    foreach(QFileInfo info,iconsFileInfoList)
    {
        if(info.isFile())continue;
        QString strChildFilePath = iconsPath + "/" + info.fileName();
        loadIconTemplateFile(pIconTemplateList,strChildFilePath);
    }
}

void HIconHelper::loadIconTemplateFile(QList<HIconTemplate*> *pIconTemplateList,QString strIconsPath)//加载所有的模板文件
{
    //先找路径，在找文件夹，然后文件夹里面搜索添加完成
    QDir dirIconsFilePath(strIconsPath);
    if(!dirIconsFilePath.exists())
        return;
    QStringList filters;
    filters<<"*.xic";
    dirIconsFilePath.setNameFilters(filters);
    QFileInfoList iconsFileInfoList = dirIconsFilePath.entryInfoList(QDir::Files);
    foreach(QFileInfo info,iconsFileInfoList)
    {
        if(!info.isFile())continue;
        QString strIconTemplateFile = strIconsPath + "/" + info.fileName();
        QUuid uuid = QUuid(info.fileName());
        HIconTemplate *pIconTemplate = new HIconTemplate(uuid);
        if(!pIconTemplate) continue;
        pIconTemplate->readXml(strIconTemplateFile);
        pIconTemplateList->append(pIconTemplate);
    }
}


void HIconHelper::saveIconDoucument(QList<HIconTemplate*> *pIconTemplateList)
{
    if(NULL == pIconTemplateList)
        return;
    char szIconPath[128];
    getDataFilePath(DFPATH_ICON,szIconPath);
    QString iconsPath = QString(szIconPath);
    QDir dirIconsPath(iconsPath);
    if(!dirIconsPath.exists())
        dirIconsPath.mkdir(iconsPath);
    //获取遥信，遥测，遥调等文件夹
    QFileInfoList iconsFileInfoList = dirIconsPath.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(QFileInfo info,iconsFileInfoList)
    {
        if(info.isFile())continue;
        saveIconTemplateFile(pIconTemplateList,iconsPath,info.fileName());
    }
}

void HIconHelper::saveIconTemplateFile(QList<HIconTemplate*> *pIconTemplateList,const QString& strIconsPath,const QString& strFolderName)//保存所有的模板文件
{
    QDir dirIconsFilePath(strIconsPath);
    if(!dirIconsFilePath.exists())
        return;
    QString strIconsFolderPath = strIconsPath + "/" + strFolderName;
    for(int i = 0; i < pIconTemplateList->count();i++)
    {
        HIconTemplate* iconTemplate = pIconTemplateList->at(i);
        if(!iconTemplate || !iconTemplate->getModify()) continue;
        if(IsIconInFolder(strFolderName,iconTemplate->getCatalogType()))
        {
            QString strFileName = strIconsFolderPath + "/" + iconTemplate->getUuid().toString() + ".xic";
            //如果有文件存在 就删除
            if(QFile::exists(strFileName))
            {
                QFile::remove(strFileName);
            }
            iconTemplate->writeXml(strFileName);

        }
        else
        {
           // QString strWarning = QStringLiteral("没有找到")+iconTemplate->getCatalogName()+QStringLiteral("文件夹!");
           // QMessageBox::warning(NULL,"warning",strWarning);
        }
    }
}


bool HIconHelper::IsIconInFolder(const QString& strFolderName,const int nIconType)
{
    bool bIn = false;
    if((strFolderName == "digital"||strFolderName == QStringLiteral("遥信")) && nIconType == TEMPLATE_TYPE_DIGITAL)
        bIn = true;
    else if((strFolderName == "analogue"||strFolderName == QStringLiteral("遥测")) && nIconType == TEMPLATE_TYPE_ANALOGUE)
        bIn = true;
    else if((strFolderName == "relay"||strFolderName == QStringLiteral("遥控")) && nIconType == TEMPLATE_TYPE_RELAY)
        bIn = true;
    else if((strFolderName == "pluse"||strFolderName == QStringLiteral("遥脉")) && nIconType == TEMPLATE_TYPE_PLUSE)
        bIn = true;
    else if((strFolderName == "tap"||strFolderName == QStringLiteral("档位")) && nIconType == TEMPLATE_TYPE_TAP)
        bIn = true;
    else if((strFolderName == "light"||strFolderName == QStringLiteral("光字牌")) && nIconType == TEMPLATE_TYPE_LIGHT)
        bIn = true;
    else if((strFolderName == "jdpai"||strFolderName == QStringLiteral("接地牌")) && nIconType == TEMPLATE_TYPE_JDPAI)
        bIn = true;
    else if((strFolderName == "control"||strFolderName == QStringLiteral("控制点")) && nIconType == TEMPLATE_TYPE_CONTROL)
        bIn = true;
    else if((strFolderName == "transform2"||strFolderName == QStringLiteral("两卷变")) && nIconType == TEMPLATE_TYPE_TRANSFORMER2)
        bIn = true;
    else if((strFolderName == "transform3"||strFolderName == QStringLiteral("三卷变")) && nIconType == TEMPLATE_TYPE_TRANSFORMER3)
        bIn = true;
    else if((strFolderName == "generator"||strFolderName == QStringLiteral("发电机")) && nIconType == TEMPLATE_TYPE_GENERATOR)
        bIn = true;
    else if((strFolderName == "capacitor"||strFolderName == QStringLiteral("电容器")) && nIconType == TEMPLATE_TYPE_CAPACITOR)
        bIn = true;
    else if((strFolderName == "otherdevice"||strFolderName == QStringLiteral("其他设备")) && nIconType == TEMPLATE_TYPE_OTHERDEVICE)
        bIn = true;
    else if((strFolderName == "pt"||strFolderName == QStringLiteral("电压互感器")) && nIconType == TEMPLATE_TYPE_PT)
        bIn = true;
    else if((strFolderName == "ct"||strFolderName == QStringLiteral("电流互感器")) && nIconType == TEMPLATE_TYPE_CT)
        bIn = true;
    else if((strFolderName == "arrester"||strFolderName == QStringLiteral("避雷器")) && nIconType == TEMPLATE_TYPE_ARRESTER)
        bIn = true;
    else if((strFolderName == "reactor"||strFolderName == QStringLiteral("容抗器")) && nIconType == TEMPLATE_TYPE_REACTOR)
        bIn = true;
    else if((strFolderName == "other"||strFolderName == QStringLiteral("其他")) && nIconType == TEMPLATE_TYPE_OTHER)
        bIn = true;
    return bIn;
}

QString HIconHelper::getIconFolder(int nType)
{
    QString strFolderName;
    if(nType == TEMPLATE_TYPE_DIGITAL)
        strFolderName = QStringLiteral("遥信");
    else if(nType == TEMPLATE_TYPE_ANALOGUE)
        strFolderName = QStringLiteral("遥测");
    else if(nType == TEMPLATE_TYPE_RELAY)
        strFolderName =QStringLiteral("遥控");
    else if( nType == TEMPLATE_TYPE_PLUSE)
        strFolderName = QStringLiteral("遥脉");
    else if(nType == TEMPLATE_TYPE_TAP)
        strFolderName = QStringLiteral("档位");
    else if(nType == TEMPLATE_TYPE_LIGHT)
        strFolderName = QStringLiteral("光字牌");
    else if(nType == TEMPLATE_TYPE_JDPAI)
        strFolderName = QStringLiteral("接地牌");
    else if(nType == TEMPLATE_TYPE_CONTROL)
        strFolderName = QStringLiteral("控制点");
    else if(nType == TEMPLATE_TYPE_TRANSFORMER2)
        strFolderName = QStringLiteral("两卷变");
    else if(nType == TEMPLATE_TYPE_TRANSFORMER3)
        strFolderName = QStringLiteral("三卷变");
    else if(nType == TEMPLATE_TYPE_GENERATOR)
        strFolderName = QStringLiteral("发电机");
    else if(nType == TEMPLATE_TYPE_CAPACITOR)
        strFolderName = QStringLiteral("电容器");
    else if(nType == TEMPLATE_TYPE_OTHERDEVICE)
        strFolderName = QStringLiteral("其他设备");
    else if(nType == TEMPLATE_TYPE_PT)
        strFolderName = QStringLiteral("电压互感器");
    else if(nType == TEMPLATE_TYPE_CT)
        strFolderName = QStringLiteral("电流互感器");
    else if(nType == TEMPLATE_TYPE_ARRESTER)
        strFolderName = QStringLiteral("避雷器");
    else if(nType == TEMPLATE_TYPE_REACTOR)
        strFolderName = QStringLiteral("容抗器");
    else if(nType == TEMPLATE_TYPE_OTHER)
        strFolderName = QStringLiteral("其他");
    return strFolderName;
}
