#include "Inventory.h"
#include "../Server/Server.h"
#include "Player.h"
#include <vector>
#include <iostream>

Inventory::Inventory()
{

}

Inventory::~Inventory()
{
    
}

int Inventory::InitInventory(uint32_t _id, Player* _player)
{
    m_playerId = _id;
    m_player = _player;
    
    TRACER("inventory init start\n");
    // 从redis 读取 物品
    // std::vector<std::string> bufs;
    // int count = 0;
    // snprintf(m_bagkey, 30, "bag_%d", m_playerId);

    // if (REDIS.HMGET(m_bagkey, bufs, &count) > 0) {
    //     TRACER("get %d items\n", count);
    //     for (int i = 0; i < count; ++i) {
    //         // m_itempb.ParseFromString(buf[i]);
    //         Proto::Unity::ItemInfo *pitem  =  m_bagPb.add_items();

    //         pitem->ParseFromString(bufs[i]);

    //         BaseItem* item = ITEMFACTORY.CreateItem(m_itempb.m_type(), m_itempb.m_uid());
    //         pb2item(*pitem, item);

    //         addItem(item);
    //     }
    // } else {
        std::vector<ITEM> items;
        char cmd[60];
        snprintf(cmd, 60, "SELECT * FROM ITEM where userid = %d", m_playerId);
        MYSQL.GetBySQL(items, cmd);
        TRACER("get %ld items\n", items.size());
        for (auto& x : items) {
            BaseItem* item = ITEMFACTORY.CreateItem(x.type, x.itemid);
            if (item == nullptr) {
                continue;
            }
            Proto::Unity::ItemInfo *pitem  =  m_bagPb.add_items();
            sql2item(x, item);
            item2pb(item, *pitem);
            addItem(item);

            std::string str = pitem->SerializeAsString();

            REDIS.HSetField("bag_%d", "%d", str.c_str(), m_playerId, item->getUID());

            TRACER("add item id = %d, count is %d\n", x.itemid, x.count);
        }
    //}
    TRACER("inventory init end\n");

    return 0;
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
                    
                    REDIS.HSetField("bag_%d", "%d", buf, m_playerId);
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
        ITEM itemsql;
        item2Sql(iter->second, itemsql);
        ITEM bak = itemsql;
        int left = iter->second->getCount();
        if (n < left) {
            iter->second->delItem(n);
            itemsql.count -= n;
            MYSQL.ModBySQL(bak, itemsql);
            saveItem(iter->second);

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
    return 0;
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
    _itemsql.userid = m_playerId;
    _itemsql.itemid = _baseitem->getUID();
    _itemsql.hp = _baseitem->getAttribute(ItemAttributeType::ITEM_ATTRIBUTE_HP);
    _itemsql.atk = _baseitem->getAttribute(ItemAttributeType::ITEM_ATTRIBUTE_ATK);
    _itemsql.count = _baseitem->getCount();
    _itemsql.name = _baseitem->toString();
    _itemsql.type = _baseitem->getType();
    std::cout << "userid is " << _itemsql.userid << std::endl
              << "itemid is " << _itemsql.itemid << std::endl
              << "count is " << _itemsql.count << std::endl;
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


int Inventory::item2pb(BaseItem* _baseitem, Proto::Unity::ItemInfo& _itempb)
{
    _itempb.set_m_uid(_baseitem->getUID());
    _itempb.set_m_count(_baseitem->getCount());
    _itempb.set_m_type(_baseitem->getType());
    _itempb.set_m_hp(_baseitem->getAttribute(ItemAttributeType::ITEM_ATTRIBUTE_HP));
    _itempb.set_m_atk(_baseitem->getAttribute(ItemAttributeType::ITEM_ATTRIBUTE_ATK));

    
    std::cout << "id is " << _itempb.m_uid() << std::endl
              << "count is " << _itempb.m_count() << std::endl;

    return 0;
}


int Inventory::pb2item(Proto::Unity::ItemInfo& _itempb, BaseItem* _baseitem)
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


    TRACER("save item to sql\n");
    MYSQL.SetBySQL(m_itemsql);

    item2pb(_item, m_itempb);

    std::cout << "item id = " << m_itempb.m_uid() << "type is " << m_itempb.m_type() << std::endl;

    std::string str = m_itempb.SerializeAsString();


    REDIS.HSetField("bag_%d", "%d", str.c_str(), m_playerId, _item->getUID());

    TRACER("save item to redis\n");

    return 0;
}


Proto::Unity::PlayerBag& Inventory::getbagPb()
{
    return m_bagPb;
}
