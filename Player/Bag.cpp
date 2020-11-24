#include "Bag.h"

BaseBag::BaseBag() : m_nCapacity(10), m_mItems(std::move(std::map<int, BaseItem*>()))
{

}

BaseBag::~BaseBag()
{
    
}

bool BaseBag::addItem(BaseItem* _baseitem)
{
    if (_baseitem == nullptr) {
        return false;
    }

    if (m_nCapacity >= m_mItems.size()) {
        if (_baseitem->isStack()) {
            if (m_mItems.count(_baseitem->getType())) {
                m_mItems[_baseitem->getType()]->addItem(_baseitem);
            } else {
                m_mItems.insert(std::make_pair<_baseitem->getType, >)  
            }
        }
    }
}

EquipItem