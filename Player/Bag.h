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

    static bool transItem(BaseBag*, BaseBag*, BaseItem*);
    static bool transItem(BaseBag*, BaseBag*, unsigned long, int);
protected:
    int m_nCapacity;
    std::map<int, BaseItem*> m_mItems;
};


class EquipBag : public BaseBag {
public:
    EquipBag();
    ~EquipBag();

    bool addItem(BaseItem*);
    bool delItem(BaseItem*);
    bool equip(EquipItem*);
    bool unequip(EquipItem*);
    bool unequip(unsigned long);
};

class MoneyBag : public BaseBag {
public:
    MoneyBag();
    ~MoneyBag();
};
 

#endif
