#include "Inventory.h"
#include "../Server/Server.h"
#include <vector>

Inventory::Inventory()
{

}

Inventory::~Inventory()
{
    
}

int Inventory::InitInventory(Player* _p)
{
    m_player = _p;

    // 从redis 读取 物品
    char buf[20][1024];
    int count = 0;
    snprintf(m_bagkey, 30, "bag_%d", m_player->getId());

    if (REDIS.HMGET(m_bagkey, (char**)buf, &count) == 0) {
        for (int i = 0; i < count; ++i) {
            m_itempb.ParseFromString(buf[i]);
            BaseItem* item = ITEMFACTORY.CreateItem(m_itempb.m_type(), m_itempb.m_uid());
            pb2item(m_itempb, item);
            addItem(item);
        }
    } else {
        std::vector<ITEM> items;
        char cmd[40];
        snprintf(cmd, 40, "SELECT * FROM ITEM where userid = %d", m_player->getId());
        MYSQL.GetBySQL(items, cmd);

        for (auto& x : items) {
            BaseItem* item = ITEMFACTORY.CreateItem(x.type, x.itemid);
            sql2item(x, item);
            addItem(item);
        }
    }
}



int Inventory::addItem(BaseItem* _item)
{
    if (_item) {
        // TODO bug 背包满不能放入可叠加物品
        if (m_baseBag.capacity > m_baseBag.items.size()) {
            item2Sql(_item, m_itemsql);
            ITEM itemcp = m_itemsql;

            if (m_mItems.count(_item->getUID())) {
                if (_item->isStack()) {
                    m_mItems[_item->getUID()]->addItem(_item->getCount());
                    itemcp.count = m_mItems[_item->getUID()]->getCount();
                    MYSQL.ModBySQL(m_itemsql, itemcp);
                    item2pb(m_mItems[_item->getUID()], m_itempb);
                    
                    int size = m_itempb.ByteSizeLong();
                    char buf[200] = {0};
                    m_itempb.SerializeToArray(buf, size);
                    
                    REDIS.HSetField("bag_%d", "%d", buf, m_player->getId());
                }
            } else {
                m_mItems.insert(std::make_pair(_item->getUID(), _item));
                m_baseBag.add(_item->getUID());
            }
            return 0;
        } 
    }
    return -1;
}

int Inventory::addItem(uint _uid, int n)
{
    auto iter = m_mItems.find(_uid);
    if (iter != m_mItems.end()) {
        // TODO
        if (iter->second->isStack()) {
            m_mItems[_uid]->addItem(n);
            return 0;
        }
    }
    return -1;
}

int Inventory::delItem(uint _uid, int n)
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
            m_baseBag.del(iter->first);
            return 0;
        }
    }
    return -1;
}

int Inventory::useItem(uint _uid)
{
    auto iter = m_mItems.find(_uid);
    if (iter != m_mItems.end()) {
        ConsumItem* item = dynamic_cast<ConsumItem*>(iter->second);
        if (item) {
            // TODO player 使用
            return delItem(_uid);
        }
    }
    return -1;
}



int Inventory::equipItem(uint _uid)
{
    if (m_baseBag.isexist(_uid)) {
            for (auto& n : m_baseBag.items) {
                if (m_mItems[n]->getType() == m_mItems[_uid]->getType())    // 装备部位相同
                {
                    return -1;
                }
            }

            m_baseBag.del(_uid);
            m_equipBag.add(_uid);
            return 0;
    }
    return -1;
}

int Inventory::unequipItem(uint _uid)
{
    if (m_equipBag.items.count(_uid)) {
        if (m_baseBag.capacity < m_baseBag.items.size()) {
            m_baseBag.del(_uid);
            m_equipBag.add(_uid);
            return 0;
        }
    }
    return -1;
}




int Inventory::tradeItem(uint _uid, int _n, int _playerid)
{
    // TODO
}



int Inventory::saveAll()
{
    for (auto& iter : m_mItems) {
        saveItem(iter.second);
    }
    return 0;
}


int Inventory::item2Sql(BaseItem* _baseitem, ITEM& _itemsql)
{
    _itemsql.userid = m_player->getId();
    _itemsql.itemid = _baseitem->getUID();
    _itemsql.hp = _baseitem->getAttribute(ItemAttributeType::ITEM_ATTRIBUTE_HP);
    _itemsql.atk = _baseitem->getAttribute(ItemAttributeType::ITEM_ATTRIBUTE_ATK);
    _itemsql.count = _baseitem->getCount();
    _itemsql.name = _baseitem->toString();
    _itemsql.type = _baseitem->getType();

    return 0;
}

int Inventory::sql2item(ITEM& _itemsql, BaseItem* _baseitem)
{

    _baseitem->setUID(_itemsql.itemid);
    _baseitem->setType(_itemsql.type);
    _baseitem->setCount(_itemsql.count);
    _baseitem->setAttribute(ItemAttributeType::ITEM_ATTRIBUTE_ATK, _itemsql.atk);
    _baseitem->setAttribute(ItemAttributeType::ITEM_ATTRIBUTE_HP, _itemsql.hp);

    return 0;
}


int Inventory::item2pb(BaseItem* _baseitem, Proto::Unity::Items& _itempb)
{
    _itempb.set_m_uid(_baseitem->getUID());
    _itempb.set_m_count(_baseitem->getCount());
    _itempb.set_m_type(_baseitem->getType());
    _itempb.set_m_hp(_baseitem->getAttribute(ItemAttributeType::ITEM_ATTRIBUTE_HP));
    _itempb.set_m_atk(_baseitem->getAttribute(ItemAttributeType::ITEM_ATTRIBUTE_ATK));

    return 0;
}


int Inventory::pb2item(Proto::Unity::Items& _itempb, BaseItem* _baseitem)
{
    _baseitem->setUID(_itempb.m_uid());
    _baseitem->setType(_itempb.m_type());
    _baseitem->setCount(_itempb.m_count());
    _baseitem->setAttribute(ItemAttributeType::ITEM_ATTRIBUTE_HP, _itempb.m_hp());
    _baseitem->setAttribute(ItemAttributeType::ITEM_ATTRIBUTE_ATK, _itempb.m_atk());

    return 0;
}


int Inventory::saveItem(BaseItem* _item)
{
    item2Sql(_item, m_itemsql);


    MYSQL.SetBySQL(m_itemsql);

    item2pb(_item, m_itempb);
    
    int size = m_itempb.ByteSizeLong();
    char tmp[size];
    m_itempb.SerializeToArray(tmp, size);

    REDIS.HSetField("bag_%d", "%d", tmp, size, m_player->getId(), _item->getUID());

    return 0;
}
