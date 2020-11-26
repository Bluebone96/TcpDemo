#include "Item.h"
#include "ItemConfig.h"


ItemAttribute& ItemAttribute::operator=(const ItemAttribute& _ia)
{
    m_mpValue = _ia.m_mpValue;
    return *this;
}


BaseItem* ItemFactory::CreateItem(int _type, int _configId)
{
    BaseItem* item = nullptr;
    switch (_type)
    {
        case 0:
            item = new MoneyItem();
        case 1:
            item = new ConsumItem();
        case 3:
            item = new EquipItem();
        default:
            return nullptr;
    }

    item->initItem(_configId);
    return item;
}

BaseItem::BaseItem(int _n) : m_nFlagBit(0), m_nType(0), m_nUID(0), m_bSaveNow(false), m_nCount((m_nCount = _n) > 0 ? _n : 1)
{
}

BaseItem::~BaseItem()
{

}

BaseItem::BaseItem(const BaseItem& _bi, int _n)
{
    m_nUID = _bi.m_nUID;
    m_nFlagBit = _bi.m_nFlagBit;
    m_nType = _bi.m_nType;
    m_bSaveNow = _bi.m_bSaveNow;
    m_nCount = _n ? _bi.m_nCount : _n;
}


unsigned long BaseItem::getUID() const
{
    return m_nUID;
}

void BaseItem::setUID(unsigned long _uid)
{
    m_nUID = _uid;
}

int BaseItem::getConfID() const
{
    return (m_nUID & 0x0000ffff);
}

int BaseItem::getCount() const
{
    return m_nCount;
}


int BaseItem::getType() const
{
    return m_nType;
}

bool BaseItem::isBind() const
{
    return (m_nFlagBit & 0x1); 
}

bool BaseItem::isStack() const
{
    return (m_nFlagBit & 0x2);
}


int BaseItem::addItem(int _n)
{
    return (m_nCount += _n);
}

int BaseItem::delItem(int _n)
{
    if (_n <= m_nCount) {
        return (m_nCount -= _n);
    }
    return -1;
}



MoneyItem::MoneyItem(int _n) : BaseItem(_n)
{
    
}

MoneyItem::~MoneyItem()
{

}

MoneyItem::MoneyItem(const MoneyItem& _m, int _n) : BaseItem(_m, _n)
{

}


std::string MoneyItem::toString() const
{
    return "money";
}

void MoneyItem::initItem(int _config)
{
    // TODO
}


BaseItem* MoneyItem::getBak(int _n) const
{
    return new MoneyItem(*this, _n);
}



ConsumItem::ConsumItem(int _n) : BaseItem(_n)
{

}

ConsumItem::~ConsumItem()
{

}

ConsumItem::ConsumItem(const ConsumItem& _ci, int _n) : BaseItem(_ci, _n)
{
    m_Attribute = _ci.m_Attribute;
}

void ConsumItem::initItem(int _confd)
{
    // TODO
}



std::string ConsumItem::toString() const
{
    return "consumitem";
}

BaseItem* ConsumItem::getBak(int _n) const
{
    return new ConsumItem(*this, _n);
}


EquipItem::EquipItem()
{

}





