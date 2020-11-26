#include "Inventory.h"

Inventory::Inventory(Player* _p) : m_owner(_p), m_baseBag(20), m_equipBag(12), m_moneyBag(4)
{

}

Inventory::~Inventory()
{

}

int Inventory::addItem(BaseItem* _item)
{
    if (_item) {
        // TODO 背包满不能放入可叠加物品
        if (m_baseBag.capacity > m_baseBag.items.size()) {
            if (m_mItems.count(_item->getUID())) {
                if (_item->isStack()) {
                    m_mItems[_item->getUID()]->addItem(n);
                    return 0;
                }
            }
            m_mItems.insert(std::make_pair(_item->getUID(), _item));
            m_baseBag.add(_item->getUID());
            return 0;
        } 
    }
    return -1;
}

int Inventory::delItem(unsigned long _uid, int n)
{
    auto iter = m_mItems.find(_uid);
    if (iter != m_mItems.end()) {
        int left = iter->second->getCount();
        if (n < left) {
            iter->second->delItem(n);
            return 0;
        } else if (n == left) {
            delete iter->second;
            m_mItems.erase(iter);
            m_baseBag.del(iter);
            return 0;
        }
    }
    return -1;
}

int Inventory::useItem(unsigned long _uid)
{
    auto iter = m_mItems.find(_uid);
    if (iter != m_mItems.end()) {
        ConsumItem* item = dynamic_cast<ConsumItem*>(iter->second)
        if (item) {
            // TODO player 使用
            return delItem(_uid);
        }
    }
    return -1;
}



int Inventory::equipItem(unsigned long _uid)
{
    if (m_baseBag.isexist(_uid)) {
            for (unsigned long& n : m_baseBag.items) {
                if (m_mItems[n]->getType() == m_mItems[_uid]->getType())    // 装备部位相同
                {
                    return -1;
                }
            }

            m_baseBag.del(_item);
            m_equipBag.add(_item);
            return 0;
    }
    return -1;
}

int Inventory::unequipItem(unsigned long _uid)
{
    if (m_equipBag.items.count()) {
        if (m_baseBag.capacity < m_baseBag.items.size()) {
            m_baseBag.del(_item);
            m_equipBag.add(_item);
            return 0;
        }
    }
    return -1;
}




int Inventory::tradeItem(unsigned long _uid, int _n, int _playerid)
{
    // TODO
}


