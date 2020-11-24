#ifndef _BAG_H_
#define _BAG_H_

#include "Item.h"

class BaseBag {
public:
    BaseBag();
    ~BaseBag();

    bool addItem(BaseItem*);
    bool delItem(BaseItem*);
protected:
    int m_nCapacity;
    std::map<int, BaseItem*> m_mItems;
};


class EquipBag  {
public:
    EquipBag();
    ~EquipBag();

    void addItem(EquipItem*);
    void delItem(EquipItem*);

private:
    int m_nCapacity;
    EquipItem[10] m_aEquip;
};

class MoneyBag : public BaseBag {
public:
    MoneyBag();
    ~MoneyBag();

    void addItem(MoneyItem*);
    void delItem(MoneyItem*);

private:
    MoneyItem[2] m_aMoney;
};
 

#endif
