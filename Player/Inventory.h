#ifndef _INVENTORY_HPP_
#define _INVENTORY_HPP_

#include "Bag.h"
#include "Item.h"
#include "Player.h"


class Inventory {
public:
    Inventory(Player*);
    ~Inventory();
    Inventory(const Inventory&) = delete;
    Inventory& operator=(const Inventory&) =delete;


    int baseBagAdd(unsigned long, int n =1);
    int baseBagDel(unsigned long, int n =1);
    int baseBagUse(unsigned long, int n =1);


    int equipItem(unsigned long);
    int unequipItem(unsigned long);

    int moneyAdd(unsigned long, int);
    int moneyDel(unsigned long, int);


    int tradeItem(unsigned long _uid, int _n, int _playerid);

    // static bool transItem(BaseBag*, BaseBag*, unsigned long, int);
    // static bool transItem(BaseBag*, BaseBag*, BaseItem*);

private:
    Player* m_owner;

    BaseBag m_baseBag;
    EquipBag m_equipBag;
    MoneyBag m_moneyBag;
};



#endif

