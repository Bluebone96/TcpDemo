#ifndef _INVENTORY_H_
#define _INVENTORY_H_

#include <map>
#include <set>

#include "../Proto/PlayerInfo.pb.h"
#include "Item.h"

#include "../SQL/toredis.h"

#define MYSQLPP_SSQLS_NO_STATICS
#include "../SQL/tomysql.h"


class Player;
struct Bag {
    uint32_t capacity;
    std::set<uint> items;
    Bag(int _n = 20) : capacity(_n) { }
    void add(int _n) { items.insert(_n); }
    void del(int _n) { items.erase(_n); }
    int isexist(int _n) { return items.count(_n); }
};

class Inventory {
public:
    Inventory();
    ~Inventory();

    Inventory(const Inventory&) = delete;
    Inventory& operator=(const Inventory&) =delete;

    int InitInventory(Proto::Unity::PlayerBag*, Player*);


    int update_item(Proto::Unity::ItemUpdate& pb);


    int addItem(BaseItem*);
    
    // 增加已有的, 如money
    int addItem(uint, int);

    int delItem(uint, int n = 1);

    int useItem(uint);

    int equipItem(uint);
    int unequipItem(uint);

    int tradeItem(uint _uid, int _n, int _playerid);

    int saveAll();

    int saveItem(BaseItem*);
    
    int item2Sql(BaseItem* _item, ITEM&);
    int item2pb(BaseItem* _item, Proto::Unity::ItemInfo&);

    int sql2item(ITEM&, BaseItem* _item);
    int pb2item(Proto::Unity::ItemInfo&, BaseItem* _item);

    Proto::Unity::PlayerBag getbagPb();


private:

    Player* m_player;
    uint32_t m_playerId;

    std::map<uint, BaseItem*> m_mItems;

    Bag m_baseBag;
    Bag m_equipBag;
    Bag m_moneyBag;



    ITEM m_itemsql;
    Proto::Unity::ItemInfo m_itempb;

    Proto::Unity::PlayerBag* m_playerBagPb;

    char m_bagkey[30];
};



#endif

