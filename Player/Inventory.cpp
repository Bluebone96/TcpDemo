#include "Inventory.h"

Inventory::Inventory(Player* _p) : m_owner(_p), m_baseBag(), m_equipBag(), m_moneyBag()
{

}

Inventory::~Inventory()
{

}

int Inventory::baseBagAdd(unsigned long _uid, int _n)
{
    
}

int Inventory::baseBagDel(unsigned long _uid, int _n)
{

}

int Inventory::moneyAdd(unsigned long _uid, int _n)
{

}

int Inventory::moneyDel(unsigned long _uid, int n) 
{

}

int Inventory::equipItem(unsigned long _uid)
{

}

int Inventory::unequipItem(unsigned long _uid)
{

}

int Inventory::tradeItem(unsigned long _uid, int _n, int _playerid)
{

}


