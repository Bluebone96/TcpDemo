#ifndef _BAG_H_
#define _BAG_H_

#include "Item.h"
// #include <memory>

class BaseBag {
public:
    // typedef std::shared_ptr<BaseItem> spItem;    // new 新对象
    BaseBag();
    ~BaseBag();

    bool addItem(BaseItem*, int n = 1);
    bool delItem(BaseItem*);
    bool delItem(unsigned long, int n = 1);

protected:
    int m_nCapacity;
    std::map<int, BaseItem*> m_mItems;
};


class EquipBag {
public:
    EquipBag();
    ~EquipBag();

    bool equip(EquipItem*);
    bool unequip(EquipItem*);
};

class MoneyBag : public BaseBag {
public:
    MoneyBag();
    ~MoneyBag();
};
 

#endif

