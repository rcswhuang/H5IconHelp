#if defined(_MSC_VER) &&(_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include "hstation.h"
#include "publicdata.h"

ATTRINFO DgtAttrInfo[] =
{
    {ATTR_DGT_4_STATE_VALUE,   "工程值(双遥信)"},
    {ATTR_DGT_VALUE,           "工程值"},
    {0,                          NULL}
};

ATTRINFO AnaAttrInfo[] =
{
    {ATTR_ANA_VALUE,         "工程值"},
    {ATTR_ANA_CC1,           "偏移"},
    {ATTR_ANA_CC2,           "系数"},
    {0,                      NULL}
};

HStationHelper::HStationHelper()
{

}

HStationHelper::~HStationHelper()
{
    //while(stationList.is)
}

HStationHelper* HStationHelper::m_pInstance = NULL;
HStationHelper* HStationHelper::Instance()
{
    if(!m_pInstance)
    {
        m_pInstance = new HStationHelper;
        m_pInstance->loadStation();
    }
    return m_pInstance;
}


void HStationHelper::loadStation()
{
    stationList.loadStation();
}

HStation* HStationHelper::getStation(quint16 wStationID)
{
    return stationList.getStation(wStationID);
}

//获取厂站列表
QList<HStation*> HStationHelper::getStationList()
{
    return stationList.pStationList;
}

///////////////////////////////////厂站列表/////////////////////////////////////////////////////
HStationList::HStationList()
{

}

HStationList::~HStationList()
{

}


//加载厂站信息
void HStationList::loadStation()
{
    FILEHANDLE fileHandle;
    memset(&fileHandle,0,sizeof(FILEHANDLE));
    DATAFILEHEADER dataFileHandle;
    //厂站信息
    int fd = openDB(FILE_TYPE_STATION);
    loadDataFileHeader(fd,&dataFileHandle);
    //int wStation = 0;
    for(int i = 0 ; i < dataFileHandle.wTotal; i++)
    {
        HStation* pStation = new HStation;
        if(!pStation)
            break;

        if ( false == loadDBRecord(FILE_TYPE_STATION, ++fileHandle.wStation, &pStation->station ) )
        {
            delete pStation;
            pStation=NULL;
            break;
        }
        if(false == pStation->loadData(fileHandle))
        {
            delete pStation;
            pStation = NULL;
            break;
        }
        pStationList.append(pStation);
    }
}

//厂站ID获取厂站
HStation* HStationList::getStation(quint16 wStationID)
{

    for(int i = 0; i < pStationList.count();i++)
    {
        HStation* pStation = pStationList[i];
        if(wStationID == pStation->getNo())
            return pStation;
    }
    return NULL;

}

//厂站地址获取厂站
HStation* HStationList::getRtu(quint16 wStationAddress)
{
    for(int i = 0; i < pStationList.count();i++)
    {
        HStation* pStation = pStationList[i];
        if(wStationAddress == pStation->getAddress())
            return pStation;
    }
    return NULL;
}

//索引厂站
HStation* HStationList::findStation(int nIndex)
{
    return pStationList.value(nIndex);
}

///////////////////////////////////////////////厂站信息//////////////////////////////////////
HStation::HStation()
{
    pAnalogue = NULL;
    pDigital = NULL;
    pRelay = NULL;
    pSetPoint = NULL;
    pPulse = NULL;
    pGroup = NULL;

    wTotalAnalogue = 0;
    wTotalDigital = 0;
    wTotalRelay = 0;
    wTotalSetPoint = 0;
    wTotalPulse = 0;
    wTotalGroup = 0;
}

HStation::~HStation()
{
    if(!pAnalogue)
        delete[] pAnalogue;
    if(!pDigital)
        delete[] pDigital;
    if(!pRelay)
        delete[] pRelay;
    if(!pSetPoint)
        delete[] pSetPoint;
    if(!pPulse)
        delete[] pPulse;
    if(!pGroup)
        delete[] pGroup;

    pAnalogue = NULL;
    pDigital = NULL;
    pRelay = NULL;
    pSetPoint = NULL;
    pPulse = NULL;
    pGroup = NULL;
}


quint16 HStation::getNo()
{
    return station.wStationID;
}

quint16 HStation::getAddress()
{
    return station.wStationAddress;
}

const char* HStation::getName()
{
    return station.szStationName;
}

//bool loadData();
bool HStation::loadData(FILEHANDLE& fileHandle)
{

    wTotalAnalogue = station.wAnalogueCounts;
    wTotalDigital = station.wDigitalCounts;
    wTotalGroup = station.wEquipmentGroupCounts;
    wTotalRelay = station.wRelayCounts;

    //间隔
    if(wTotalGroup != 0)
    {
        pGroup = new HGroup[wTotalGroup];
        if(NULL == pGroup)
        {
            wTotalGroup = 0;
            return false;
        }
        EQUIPMENTGROUP pEquip;
        HGroup* group = pGroup;
        openDB(FILE_TYPE_EQUIPMENTGROUP);
        //最好判断一下 可能station记录的和文件header里面记录的不一致
        for(int i = 0; i < station.wEquipmentGroupCounts;i++,group++)
        {
            loadDBRecord(FILE_TYPE_EQUIPMENTGROUP,++fileHandle.wEquipmentGroup,&pEquip);
            group->wNo = pEquip.wGroupID;
            qstrncpy(group->szName,pEquip.szGroupName,PTNAMELEN-1);
            group->szName[PTNAMELEN-1] = 0;
        }
    }

    //遥测
    if(wTotalAnalogue != 0)
    {
        pAnalogue = new HAnalogue[wTotalAnalogue];
        if(NULL == pAnalogue)
        {
            wTotalAnalogue = 0;
            return false;
        }
        openDB(FILE_TYPE_ANALOGUE);
        ANALOGUE pAna;
        HAnalogue* ana = pAnalogue;
        for(int i = 0; i < station.wAnalogueCounts;i++,ana++)
        {

            loadDBRecord(FILE_TYPE_ANALOGUE,++fileHandle.wAnalogue,&pAna);
            ana->wNo = pAna.wAnalogueID;
            ana->btType = pAna.btAnalogueType;
            ana->wGroupID = pAna.wGroupID;
            qstrncpy(ana->szName,pAna.szAnalogueName,PTNAMELEN-1);
            ana->szName[PTNAMELEN-1] = 0;
        }
    }

    //遥信
    if(wTotalDigital != 0)
    {
        pDigital = new HDigital[wTotalDigital];
        if(NULL == pDigital)
        {
            wTotalDigital = 0;
            return false;
        }
        openDB(FILE_TYPE_DIGITAL);
        DIGITAL* pDig = new DIGITAL;
        HDigital* digital = pDigital;
        for(int i = 0; i < station.wDigitalCounts;i++,digital++)
        {

            loadDBRecord(FILE_TYPE_DIGITAL,++fileHandle.wDigital,pDig);
            digital->wNo = pDig->wDigitalID;
            digital->wGroupID = pDig->wGroupID;
            qstrncpy(digital->szName,pDig->szDigitalName,PTNAMELEN-1);
            digital->szName[PTNAMELEN-1] = 0;
        }
        if(pDig)
        {
            delete pDig;
            pDig = NULL;
        }
    }

    //遥控
    if(wTotalRelay != 0)
    {
        pRelay = new HRelay[wTotalRelay];
        if(NULL == pRelay)
        {
            wTotalRelay = 0;
            return false;
        }
        openDB(FILE_TYPE_RELAY);
        RELAY Relay;
        HRelay* relay = pRelay;
        for(int i = 0; i < station.wRelayCounts;i++,relay++)
        {

            loadDBRecord(FILE_TYPE_RELAY,++fileHandle.wRelay,&Relay);
            relay->wNo = Relay.wNo;
            relay->btType = Relay.btType;
            qstrncpy(relay->szName,Relay.szName,PTNAMELEN-1);
            relay->szName[PTNAMELEN-1] = 0;
        }
    }

    return true;
}

quint16 HStation::getWordCount(quint8 btType)
{
    return 0;
}

quint16 HStation::getTotalAnalogue()
{
    return wTotalAnalogue;
}

quint16 HStation::getTotalDigital()
{
    return wTotalDigital;
}

quint16 HStation::getTotalSetPoint()
{
    return wTotalSetPoint;
}

quint16 HStation::getTotalRelay()
{
    return wTotalRelay;
}

quint16 HStation::getTotalPulse()
{
    return wTotalPulse;
}

quint16 HStation::getTotalGroup()
{
    return wTotalGroup;
}

HAnalogue* HStation::getAnalogue()
{
    return pAnalogue;
}

HAnalogue* HStation::getAnalogue(quint16 wNo)
{
    HAnalogue* analogue = pAnalogue;
    for(int i = 0; i < wTotalAnalogue; i++,analogue++)
        if(wNo == analogue->getNo())
            return analogue;
    return NULL;
}

HDigital* HStation::getDigital()
{
    return pDigital;
}

HDigital* HStation::getDigital(quint16 wNo)
{
    HDigital* digital = pDigital;
    for(int i = 0; i < wTotalDigital;i++,digital++)
        if(wNo == digital->getNo())
            return digital;
    return NULL;
}

HRelay*  HStation::getRelay()
{
    return pRelay;
}

HRelay*  HStation::getRelay(quint16 wNo)
{
    HRelay *relay = pRelay;
    for(int i = 0;i < wTotalRelay;i++,relay++)
        if(wNo == relay->getNo())
            return relay;
    return NULL;
}

HSetPoint* HStation::getSetPoint()
{
    return pSetPoint;
}

HSetPoint* HStation::getSetPoint(quint16 wNo)
{
    HSetPoint* setPoint = pSetPoint;
    for(int i = 0; i < wTotalSetPoint;i++,setPoint++)
        if(wNo == setPoint->getNo())
            return setPoint;
    return NULL;
}

HPulse* HStation::getPulse()
{
    return pPulse;
}

HPulse* HStation::getPulse(quint16 wNo)
{
    HPulse* pulse = pPulse;
    for(int i = 0; i < wTotalPulse; i++,pulse++)
        if(wNo == pulse->getNo())
            return pulse;
    return NULL;
}

HGroup* HStation::getGroup()
{
    return pGroup;
}

HGroup* HStation::getGroup(quint16 wNo)
{
    HGroup* group = pGroup;
    for(int i = 0; i < wTotalGroup;i++,group++)
        if(wNo == group->getNo())
            return group;
    return NULL;
}

HGroup* HStation::getGroupByDigital(quint16 wDigitalID)
{
    HDigital* digital = getDigital(wDigitalID);
    if(!digital)
        return NULL;
    HGroup* group = pGroup;
    for(int i = 0; i < wTotalGroup;i++,group++)
        if( digital->getGroupID() == group->getNo())
            return group;
    return NULL;
}

HGroup* HStation::getGroupByAnalogue(quint16 wAnaID)
{
    HAnalogue* ana = getAnalogue(wAnaID);
    if(!ana)
        return NULL;
    HGroup* group = pGroup;
    for(int i = 0; i < wTotalGroup;i++,group++)
        if( ana->getGroupID() == group->getNo())
            return group;
    return NULL;
}


HAnalogue* HStation::findAnalogue(int nIndex)
{
    if(nIndex < 0 || nIndex > wTotalAnalogue)
        return NULL;
    return pAnalogue+nIndex;
}

HDigital* HStation::findDigital(int nIndex)
{
    if(nIndex < 0 || nIndex > wTotalDigital)
        return NULL;
    return pDigital+nIndex;
}

HRelay* HStation::findRelay(int nIndex)
{
    if(nIndex < 0 || nIndex > wTotalRelay)
        return NULL;
    return pRelay+nIndex;
}

HSetPoint* HStation::findSetPoint(int nIndex)
{
    if(nIndex < 0 || nIndex > wTotalSetPoint)
        return NULL;
    return pSetPoint+nIndex;
}

HPulse* HStation::findPulse(int nIndex)
{
    if(nIndex < 0 || nIndex > wTotalPulse)
        return NULL;
    return pPulse+nIndex;
}

HGroup* HStation::findGroup(int nIndex)
{
    if(nIndex < 0 || nIndex > wTotalGroup)
        return NULL;
    return pGroup+nIndex;
}

HWord* HStation::getWord(quint8 btType)
{
    switch(btType)
    {
    case TYPE_ANALOGUE: return pAnalogue;
    case TYPE_DIGITAL: return pDigital;
    case TYPE_SETPOINT: return pSetPoint;
    case TYPE_RELAY: return pRelay;
    case TYPE_PULSE : return pPulse;
    case TYPE_GROUP: return pGroup;
    }
    return NULL;
}

HWord* HStation::getWord(quint8 btType,quint8 wNo)
{
    switch(btType)
    {
    case TYPE_ANALOGUE: return getAnalogue(wNo);
    case TYPE_DIGITAL: return getDigital(wNo);
    case TYPE_SETPOINT: return getSetPoint(wNo);
    case TYPE_RELAY: return getRelay(wNo);
    case TYPE_PULSE : return getPulse(wNo);
    case TYPE_GROUP: return getGroup(wNo);
    }
    return NULL;
}
