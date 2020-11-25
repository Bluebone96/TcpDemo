#include "Bag.h"

BaseBag::BaseBag() : m_nCapacity(10), m_mItems(std::move(std::map<int, BaseItem*>()))
{

}

BaseBag::~BaseBag()
{
    
}

bool BaseBag::addItem(BaseItem* _baseitem, int n)
{
    if (_baseitem) {
        if (m_nCapacity >= m_mItems.size()) {
            if (m_mItems.count(_baseitem->getUID())) {
                if (_baseitem->isStack()) {
                    m_mItems[_baseitem->getUID()]->addItem(n);
                    return true;
                }
            }
            m_mItems.insert(std::make_pair(_baseitem->getUID(), _baseitem->getBak()));  // 获取备份
            return true;
        } 
    }
    return false;
}

bool BaseBag::delItem(BaseItem* _baseitem)
{
    // if ()
    return false;
}

bool BaseBag::delItem(unsigned long _uid, int n)
{
    auto iter = m_mItems.find(_uid);
    if (iter != m_mItems.end()) {
        int rn = m_mItems[_uid]->delItem(n);
        if (rn < 0) {
            return false;
        } else if (rn == 0) {
            delete m_mItems[_uid];  // TODO
            m_mItems.erase(iter);
        }
        return true;
    }
    return false;
}


bool BaseBag::transItem(BaseBag* _dest, BaseBag* _src, unsigned long _uid, int _n)
{

    auto iter = _src->m_mItems.find(_uid);
    if (iter != _src->m_mItems.end()) {
        BaseItem* bak = iter->second->getBak(_n);           // TODO 效率太低
        if (BaseBag::transItem(_dest, _src, bak)) {
            return true;
        }
        delete bak;
    }

    return false;
}


bool BaseBag::transItem(BaseBag* _dest, BaseBag* _src, BaseItem* _item)
{
    if (_dest->addItem(_item)) {
        _src->delItem(_item);
        return true;
    }
    return false;
}

bool EquipBag::addItem(BaseItem* _item)
{
    EquipItem* eitem = dynamic_cast<EquipItem*>(_item);
    if (_item) {
        return equip(eitem);
    }
    return false;
}

bool EquipBag::equip(EquipItem* _item)
{
    int part = _item->getType();
    for(auto& iter : m_mItems) {
        if (part == iter.second->getType()) {
            return false;   // 同步位已经装备了
        }
    }

    m_mItems.insert(std::make_pair(_item->getUID(), _item));
    return true;
}

bool EquipBag::unequip(EquipItem* _item)
{

}

bool EquipBag::unequip(unsigned long _uid)
{
    auto iter = m_mItems.find(_uid);
    if (iter != m_mItems.end())
    {
        // TODO
    }
}
