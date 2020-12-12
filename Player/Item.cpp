#include "Item.h"
#include "ItemConfig.h"
#include <iostream>

// ItemAttribute& ItemAttribute::operator=(const ItemAttribute& _ia)
// {
//     m_mpValue = _ia.m_mpValue;
//     return *this;
// }


BaseItem* ItemFactory::CreateItem(int _type)
{
    std::cout << "ItemFactory createItem type is " << _type << std::endl; 
    BaseItem* item = nullptr;
    switch (_type)
    {
        case 0:
            item = new MoneyItem();
            break;
        case 1:
            item = new ConsumItem();
            break;
        case 2:
            item = new EquipItem();
            break;
        default:
            std::cout << "undefined _type\n";
            return nullptr;
    }

    std::cout << "createItem end\n";
    return item;
}

BaseItem::BaseItem(int _n) : m_nItemID(0), m_nFlagBit(0), m_nType(0), m_nCount((_n > 0) ? _n : 1), m_bSaveNow(false)
{
}

BaseItem::~BaseItem()
{

}

BaseItem::BaseItem(const BaseItem& _bi, int _n)
{
    m_nItemID = _bi.m_nItemID;
    m_nFlagBit = _bi.m_nFlagBit;
    m_nType = _bi.m_nType;
    m_bSaveNow = _bi.m_bSaveNow;
    m_nCount = _n ? _bi.m_nCount : _n;
}


uint BaseItem::getItemID() const
{
    return m_nItemID;
}

void BaseItem::setItemID(uint _uid)
{
    m_nItemID = _uid;
}

int BaseItem::getConfID() const
{
    return (m_nItemID & 0xffff);
}

int BaseItem::getCount() const
{
    return m_nCount;
}

int BaseItem::setCount(int n)
{
    return (m_nCount = n);
}


int BaseItem::getType() const
{
    return m_nType;
}

void BaseItem::setType(int _t)
{
    m_nType = _t;
}

bool BaseItem::isBind() const
{
    return (m_nFlagBit & 0x2); 
}

bool BaseItem::isStack() const
{
    return (m_nFlagBit & 0x1);
}


int BaseItem::addItem(int _n)
{
    return (m_nCount += _n);
}


int BaseItem::addItem(const BaseItem* _item)
{
    if (_item->getItemID() == m_nItemID) {
        m_nCount += _item->getCount();
    }
    return m_nCount;
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


// void MoneyItem::initItem(int _confd)
// {
//     std::cout << "money init start" << std::endl;
//     m_nType = 0;
//     m_nItemID = _confd;
//     m_nFlagBit = 0x1;   // 可叠加 不绑定
//     m_bSaveNow = true;
//     m_nCount = 1;
// }

void MoneyItem::initItem(const Proto::Unity::ItemInfo& _pb)
{
    m_nFlagBit = 0x1;   // 可叠加 不绑定
    m_bSaveNow = true;

    m_nUsrID = _pb.m_usrid();
    m_nItemID = _pb.m_itemid();
    m_nType = _pb.m_type();
    m_nCount = _pb.m_count();
    m_sName = _pb.m_name();
    
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
    m_attribute = _ci.m_attribute;
}


// void ConsumItem::initItem(int _confd)
// {
//     std::cout << "consumitem start" << std::endl;
//     m_nType = 1;
//     m_nItemID = _confd;
//     m_nFlagBit = 0x1; // 可叠加 不绑定
//     m_bSaveNow = true;
//     m_nCount = 1;
//     m_attribute.insert(std::make_pair(ItemAttributeType::ITEM_ATTRIBUTE_HP, 100));
// }

void ConsumItem::initItem(const Proto::Unity::ItemInfo& _pb)
{
    m_nFlagBit = 0x1; // 可叠加 不绑定
    m_bSaveNow = true;

    m_nUsrID = _pb.m_usrid();
    m_nItemID = _pb.m_itemid();
    m_nType = _pb.m_type();
    m_nCount = _pb.m_count();
    m_sName = _pb.m_name();

    m_attribute.insert(std::make_pair(ItemAttributeType::ITEM_ATTRIBUTE_HP, _pb.m_hp()));
    m_attribute.insert(std::make_pair(ItemAttributeType::ITEM_ATTRIBUTE_MP, _pb.m_mp()));
    m_attribute.insert(std::make_pair(ItemAttributeType::ITEM_ATTRIBUTE_ATK, _pb.m_atk()));
    m_attribute.insert(std::make_pair(ItemAttributeType::ITEM_ATTRIBUTE_DEF, _pb.m_def()));
    m_attribute.insert(std::make_pair(ItemAttributeType::ITEM_ATTRIBUTE_PRICE, _pb.m_price()));

}



BaseItem* ConsumItem::getBak(int _n) const
{
    return new ConsumItem(*this, _n);
}

void ConsumItem::setAttribute(ItemAttributeType _key, int val)
{
    m_attribute[_key] = val;
}

int ConsumItem::getAttribute(ItemAttributeType _key) const
{
    int val = 0;
    try {
        val = m_attribute.at(_key);
    } catch (...) {

    }
    return val;
}



EquipItem::EquipItem()
{

}


EquipItem::~EquipItem()
{

}

int EquipItem::uniqueID = 0;    // 唯一ID, 每初始化一次加一

void EquipItem::initItem(const Proto::Unity::ItemInfo& _pb)
{

    // std::cout << "Equip init start" << std::endl;
    // m_nItemID =  config ((++uniqueID) << 16); // 暂时都是手动添加的
    m_nFlagBit = 0x0;   // 初始为非绑定，不可叠加，未装备
    m_bSaveNow = true;  // 暂时未实现

    m_nUsrID = _pb.m_usrid();
    m_nItemID = _pb.m_itemid();
    m_nType = _pb.m_type();
    m_nCount = _pb.m_count();
    m_sName = _pb.m_name();

    m_attribute.insert(std::make_pair(ItemAttributeType::ITEM_ATTRIBUTE_HP, _pb.m_hp()));
    m_attribute.insert(std::make_pair(ItemAttributeType::ITEM_ATTRIBUTE_MP, _pb.m_mp()));
    m_attribute.insert(std::make_pair(ItemAttributeType::ITEM_ATTRIBUTE_ATK, _pb.m_atk()));
    m_attribute.insert(std::make_pair(ItemAttributeType::ITEM_ATTRIBUTE_DEF, _pb.m_def()));
    m_attribute.insert(std::make_pair(ItemAttributeType::ITEM_ATTRIBUTE_PRICE, _pb.m_price()));
}

bool EquipItem::isEquip()
{
    return m_nFlagBit & 0x4;
}

int EquipItem::equip() 
{
    if (isEquip()) {
        return -1;
    }
    m_nFlagBit |= 0x4;
    return 0;
}

int EquipItem::unequip() 
{
    if (isEquip()) {
        m_nFlagBit ^= 0x4;
        return 0;
    }
    return -1;
}

void EquipItem::setAttribute(ItemAttributeType _key, int _val)
{
    m_attribute[_key] = _val;
}

int EquipItem::getAttribute(ItemAttributeType _key) const
{
    return m_attribute.at(_key);
}

BaseItem* EquipItem::getBak(int) const
{
    return nullptr;
}
