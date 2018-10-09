#if defined(_MSC_VER) &&(_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include "hgraphhelper.h"
#include "hicontemplate.h"
#include "hbaseobj.h"
#include "hiconobj.h"
#include "hgraph.h"
#include <QProcessEnvironment>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include "publicdata.h"

HGraphHelper::HGraphHelper()
{

}

HGraphHelper* HGraphHelper::m_pInstance = NULL;

HGraphHelper* HGraphHelper::Instance()
{
    if(!m_pInstance)
    {
        m_pInstance = new HGraphHelper();
    }
    return m_pInstance;
}

void HGraphHelper::loadIconTemplate(QList<HIconTemplate*> *pIconTemplateList)
{
    if(NULL == pIconTemplateList)
        return;
    //先找路径，在找文件夹，然后文件夹里面搜索添加完成
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

void HGraphHelper::loadIconTemplateFile(QList<HIconTemplate*> *pIconTemplateList,QString strIconsPath)//加载所有的模板文件
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

void HGraphHelper::loadAllGraph(QList<HGraph*> *pGraphList)
{
    if(NULL == pGraphList)
        return;
    //先找路径，在找文件夹，然后文件夹里面搜索添加完成
    char szIconPath[128];
    getDataFilePath(DFPATH_GRAPH,szIconPath);
    QString iconsPath = QString(szIconPath);

    QDir dirIconsPath(iconsPath);
    if(!dirIconsPath.exists())
        return;
    QFileInfoList iconsDirList = dirIconsPath.entryInfoList(QDir::Dirs  | QDir::NoDotAndDotDot);
    foreach(QFileInfo info,iconsDirList)
    {
        if(info.isFile())continue; //icons文件夹下面文件是不读取的
        QString strChildFilePath = iconsPath + "/" + info.fileName();//graph/110kV测试变主接线图 文件夹

        //在对应测点类型的文件夹里面搜索添加完成
        QDir dirIconsFilePath(strChildFilePath);
        if(!dirIconsFilePath.exists())
            continue;
        QStringList filters;
        filters<<"*.grf";
        dirIconsFilePath.setNameFilters(filters);
        QFileInfoList graphsFileInfoList = dirIconsFilePath.entryInfoList(QDir::Files);
        foreach(QFileInfo info,graphsFileInfoList)
        {
            if(!info.isFile())continue;
            QString strGraphName = strChildFilePath + "/" + info.fileName();
            HGraph* pGraph = new HGraph("");
            pGraph->readXmlFile(strGraphName);
            pGraphList->append(pGraph);
        }
    }
}

void HGraphHelper::saveAllGraph(QList<HGraph*> *pGraphList,HGraph* pCurGraph)
{
    if(NULL == pGraphList || NULL == pCurGraph)
        return;
    char szGraphPath[128];
    getDataFilePath(DFPATH_GRAPH,szGraphPath);
    QString graphsPath = QString(szGraphPath);
    QDir dirIconsPath(graphsPath);
    if(!dirIconsPath.exists())
        dirIconsPath.mkdir(graphsPath);

    //先扫描一下当前文件夹内所有的画面名称
    QStringList curExistFolderList;
    QFileInfoList iconsFileInfoList = dirIconsPath.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(QFileInfo info,iconsFileInfoList)
    {
        if(info.isFile())continue;
        curExistFolderList<<info.fileName();
    }

    for(int i = 0; i < pGraphList->count();i++)
    {
        HGraph* pGraph = (HGraph*)pGraphList->at(i);
        if(!pGraph)
            continue;
        QString strGraphName = pGraph->getGraphName();
        if((int)-1 != curExistFolderList.indexOf(strGraphName)) //当前画面找到了
        {
            //saveGraph(path,graph)
            QString strFilePath = graphsPath + "/" +strGraphName;
            saveGraph(pGraph,strFilePath,pCurGraph);
            curExistFolderList.removeOne(strGraphName);
        }
        else
        {
            //新增一个画面
            QString strFilePath = graphsPath + "/" +strGraphName;
            if(!QDir(strFilePath).exists())
            {
                if(!QDir(graphsPath).mkdir(strFilePath))
                    continue;
            }
            saveGraph(pGraph,strFilePath,pCurGraph);
        }
    }

    //最后要把curExistFolderList剩下来的所有文件夹全部删除
    for(int i = 0; i < curExistFolderList.count();i++)
    {
        QString strPath = graphsPath + "/" + curExistFolderList[i];
        QFileInfoList iconsFileInfoList = QDir(strPath).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
        foreach(QFileInfo info,iconsFileInfoList)
        {
            if(info.isFile())
                QFile::remove(strPath+"/"+info.fileName());
        }
        dirIconsPath.rmpath(strPath);
    }
}

void HGraphHelper::saveGraph(HGraph* graph)
{
    if(NULL == graph)
        return;
    char szGraphPath[128];
    getDataFilePath(DFPATH_GRAPH,szGraphPath);
    QString graphsPath = QString(szGraphPath);
    QDir dirIconsPath(graphsPath);
    if(!dirIconsPath.exists())
        dirIconsPath.mkdir(graphsPath);

    QString strGraphPath = graphsPath + "/" +graph->getGraphName();
    if(!QDir(strGraphPath).exists())
    {
       if(!QDir(graphsPath).mkdir(strGraphPath))
           return;
    }

    QString strFileName = strGraphPath + "/" +  "0.grf";
    //如果有文件存在 就删除
    if(QFile::exists(strFileName))
    {
        QFile::remove(strFileName);
    }
    graph->writeXmlFile(strFileName);
}

void HGraphHelper::saveGraph(HGraph* graph,QString& path,HGraph* pCurGraph)
{
    if(NULL == graph)
        return;

    if(pCurGraph)
    {
        if(pCurGraph->getGraphID() == graph->getGraphID())
            pCurGraph->copyTo(graph);
    }

    QDir dirIconsFilePath(path);
    if(!dirIconsFilePath.exists())
        return;
    QString strFileName = path + "/" +  "0.grf";
    //如果有文件存在 就删除
    if(QFile::exists(strFileName))
    {
        QFile::remove(strFileName);
    }
    graph->writeXmlFile(strFileName);
}


bool HGraphHelper::IsIconInFolder(const QString& strFolderName,const int nIconType)
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

QString HGraphHelper::getIconFolder(int nType)
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
