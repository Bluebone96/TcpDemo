#ifndef _INVENTORY_HPP_
#define _INVENTORY_HPP_

#include <map>
#include <set>

//#include "Bag.h"
#include "Item.h"
#include "Player.h"


struct Bag {
    int capacity;
    std::set<uint> items;
    Bag(int _n) : capacity(_n) { }
    void add(int _n) { items.insert(_n); }
    void del(int _n) { items.erase(_n); }
    int isexist(int _n) { return items.count(_n); }
};

class Inventory {
public:
    Inventory(Player*);
    ~Inventory();
    Inventory(const Inventory&) = delete;
    Inventory& operator=(const Inventory&) =delete;


    int addItem(BaseItem*);
    
    // 增加已有的, 如money
    int addItem(uint, int);

    int delItem(uint, int n = 1);

    int useItem(uint);

    int equipItem(uint);
    int unequipItem(uint);

    int tradeItem(uint _uid, int _n, int _playerid);

    int saveAll();

private:
    Player* m_owner;

    std::map<uint, BaseItem*> m_mItems;

    Bag m_baseBag;
    Bag m_equipBag;
    Bag m_moneyBag;
};



#endif

