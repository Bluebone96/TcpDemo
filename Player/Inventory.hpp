#ifndef _INVENTORY_HPP_
#define _INVENTORY_HPP_

#include <unordered_map>

#include "../Common/basetype.h"

// class Item {
//     UINT32 itemID;
//     UINT32 count;
    
//     void InitItem(UINT32 id, UINT32 n = 1) { itemID = id; count = 1; }
//     Item& operator+(UINT32 n) { count += n;  return *this; }
//     Item& operator-(UINT32 n) { count -= n;  return *this; }
// };

class Inventory {
private:
//    std::vector<Item> m_itemList;
    std::unordered_map<UINT32, UINT32> m_itemList;

public:
    Inventory() {}
    Inventory(const Inventory& _in) { m_itemList = _in.m_itemList; }

    Inventory& operator=(const Inventory& _in)
    {
        m_itemList = _in.m_itemList;
        return *this;
    }

    Inventory(Inventory&& _in) : m_itemList(std::move(_in.m_itemList)) {}

    Inventory& operator=(Inventory&& _in) 
    { 
        m_itemList = std::move(_in.m_itemList);
        return *this;
    }

    int AddItem(UINT32 _id, UINT32 _n)
    {
        auto iter = m_itemList.find(_id);
        if (iter != m_itemList.end()) {
            iter->second += _n;
        } else
        {
            m_itemList[_id] = _n;
        }
        return EOK;
    }


    int DelItem(UINT32 _id, UINT32 _n)
    {
        auto iter = m_itemList.find(_id);
        if (iter != m_itemList.end()) {
            if (iter->second < _n) {
                return ERR;
            } else if (iter->second == _n) {
                m_itemList.erase(iter);
            } else {
                iter->second -= _n;
            }
        }
        return ERR;
    }

};

#endif
