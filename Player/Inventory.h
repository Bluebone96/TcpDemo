#ifndef _INVENTORY_HPP_
#define _INVENTORY_HPP_

#include <map>
#include <set>

//#include "Bag.h"
#include "Item.h"
#include "Player.h"


struct Bag {
    int capacity;
    std::set<unsigned long> items;
    Bag(int _n) : capacity(_n) { }
    void add(int _n) { items.insert(_n); }
    void del(int _n) { items.remove(_n); }
    int isexist(int _n) { return items.count(_n); }
};

class Inventory {
public:
    Inventory(Player*);
    ~Inventory();
    Inventory(const Inventory&) = delete;
    Inventory& operator=(const Inventory&) =delete;


    int addItem(BaseItem*);
    int delItem(unsigned long, int n = 1);

    int useItem(unsigned long);

    int equipItem(unsigned long);
    int unequipItem(unsigned long);

    int tradeItem(unsigned long _uid, int _n, int _playerid);

private:
    Player* m_owner;

    std::map<unsigned long, BaseItem*> m_mItems;

    Bag m_baseBag;
    Bag m_equipBag;
    Bag m_moneyBag;
};



#endif

