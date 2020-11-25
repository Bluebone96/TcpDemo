#include "Item.h"
#include "ItemConfig.h"



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
    }

    item->initItem(_configId);
    return item;
}

BaseItem::BaseItem()
{

}

BaseItem::~BaseItem()
{

}

unsigned long BaseItem::getUID()
{
    return m_nUID;
}

void BaseItem::setUID(unsigned long _uid)
{
    m_nUID = _uid;
}

int BaseItem::getConfID()
{
    return (m_nUID & 0x0000ffff);
}

int BaseItem::getCount()
{
    return m_nCount;
}

int BaseItem::getType()
{
    return m_nType;
}

bool BaseItem::isBind()
{
    return (m_nFlagBit & 0x1); 
}

bool BaseItem::isStack()
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


