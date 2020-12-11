#include <vector>
#include <iostream>
#include <unistd.h>

#include "Player.h"
#include "Inventory.h"
#include "../Net/message.h"
#include "../SQL/tomysql.h"
#include "../Common/log.h"



extern msg_queue g_send_queue;
extern std::map<uint32_t, uint32_t> g_connet_server;


Inventory::Inventory()
{

}

Inventory::~Inventory()
{
    
}

int Inventory::InitInventory(Proto::Unity::PlayerBag* _bag, Player* _player)
{
    m_playerBagPb = _bag;
    m_player = _player;

    for (auto& x : m_playerBagPb->items()) {
        BaseItem* item = ITEMFACTORY.CreateItem(x.m_type(), x.m_itemid());
        if (item == nullptr) {
            continue;
        }
        addItem(item);
        TRACER_DEBUG("add item id = %d, count is %d\n", x.m_itemid(), x.m_count());
    }
    TRACER_DEBUG("inventory init end\n");

    return 0;
}



int Inventory::addItem(BaseItem* _item)
{
    if (_item) {
        // TODO bug 背包满不能放入可叠加物品
        if (m_baseBag.capacity > m_baseBag.items.size()) {
            if (m_mItems.count(_item->getUID())) {
                if (_item->isStack()) {
                    m_mItems[_item->getUID()]->addItem(_item->getCount());
                    saveItem(_item);
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
            saveItem(iter->second);
        } else if (n == left) {
            delete iter->second;
            m_mItems.erase(iter);
            m_baseBag.del(iter->first);
            
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
    m_playerBagPb->clear_items();
    for (auto& iter : m_mItems) {
        Proto::Unity::ItemInfo* pb = m_playerBagPb->add_items();
        item2pb(iter.second, *pb);
    }
    return 0;
}




int Inventory::saveItem(BaseItem* _item)
{
    item2pb(_item, m_itempb);

    TRACER("save item to dbserver\n");
    std::cout << "item id = " << m_itempb.m_itemid() << "type is " << m_itempb.m_type() << std::endl;
    message* msg = g_send_queue.enqueue();
    
    while ((msg = g_send_queue.enqueue()) == nullptr)
    {
        // 队列满了, 因为 dequeue 后 需要占有内存进行计算，有一定数据失效时间， 所以 enqueue 始终快于 dequeue
        TRACER_ERROR("sleep 50ms, g_recv_queue is full, usrid is %d\n", m_playerId);
        g_send_queue.debug_info();
        usleep(50 * 1000);
    }

    msg->m_head.m_type = SETITEM;
    msg->m_head.m_usrID = m_player->getId();
    msg->m_head.m_errID = 0;
    msg->m_to = g_connet_server[DB_SERVER];
    msg->encode_pb(m_itempb);
    msg->m_flag = msg_flags::ACTIVE;

    return 0;
}


int Inventory::update_item(Proto::Unity::ItemUpdate& pb)
{
/*
 * optype : value
 *  0       basebag 增加物品，add1 为数量
 *  1       basebag 减少物品，add1 为数量
 *  2       basebag 使用物品，add1 为数量
 *  3       money   增加， add1 为数量
 *  4       money   减少   add1 为数量
 *  5       equip   装备
 *  6       unequip  卸下装备
 *  7       trade   交易物品，add1 为数量， add2 为交易对象 id
 *  8       
 */
    int rn = -1;   // 结果标记
    Proto::Unity::ItemEvent itemEvent;
    switch (itemEvent.optype())
    {
        case 0:
            // TODO 装备工厂产生了一个装备后，写入数据库，然后这里去数据库找到了再加入
            // m_inventory.addItem(nullptr);
            rn = addItem(itemEvent.uid(), itemEvent.count());
            break;
        case 1:
            rn = delItem(itemEvent.uid(), itemEvent.count());
            break;
        case 2:
            rn = useItem(itemEvent.uid());
            break;
        case 3:
            rn = equipItem(itemEvent.uid());
            break;
        case 4:
            rn = unequipItem(itemEvent.uid());
            break;
        default:
            break;
    }
    return rn;
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
    _itempb.set_m_itemid(_baseitem->getUID());
    _itempb.set_m_count(_baseitem->getCount());
    _itempb.set_m_type(_baseitem->getType());
    _itempb.set_m_hp(_baseitem->getAttribute(ItemAttributeType::ITEM_ATTRIBUTE_HP));
    _itempb.set_m_atk(_baseitem->getAttribute(ItemAttributeType::ITEM_ATTRIBUTE_ATK));

    
    std::cout << "id is " << _itempb.m_itemid() << std::endl
              << "count is " << _itempb.m_count() << std::endl;

    return 0;
}


int Inventory::pb2item(Proto::Unity::ItemInfo& _itempb, BaseItem* _baseitem)
{
    _baseitem->setUID(_itempb.m_itemid());
    _baseitem->setType(_itempb.m_type());
    _baseitem->setCount(_itempb.m_count());
    _baseitem->setAttribute(ItemAttributeType::ITEM_ATTRIBUTE_HP, _itempb.m_hp());
    _baseitem->setAttribute(ItemAttributeType::ITEM_ATTRIBUTE_ATK, _itempb.m_atk());

    return 0;
}
