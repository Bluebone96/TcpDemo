// #include "Bag.h"

// // BaseBag::BaseBag() : m_nCapacity(10), m_mItems(std::move(std::map<int, BaseItem*>()))
// // {

// // }

// // BaseBag::~BaseBag()
// // {
    
// // }

// // bool BaseBag::addItem(BaseItem* _baseitem, int n)
// // {
// //     if (_baseitem) {
// //         if (m_nCapacity >= m_mItems.size()) {
// //             if (m_mItems.count(_baseitem->getItemID())) {
// //                 if (_baseitem->isStack()) {
// //                     m_mItems[_baseitem->getItemID()]->addItem(n);
// //                     return true;
// //                 }
// //             }
// //             m_mItems.insert(std::make_pair(_baseitem->getItemID(), _baseitem->getBak()));  // 获取备份
// //             return true;
// //         } 
// //     }
// //     return false;
// // }

// // bool BaseBag::delItem(BaseItem* _baseitem)
// // {
// //     // if ()
// //     return false;
// // }

// // bool BaseBag::delItem(unsigned long _uid, int n)
// // {
// //     auto iter = m_mItems.find(_uid);
// //     if (iter != m_mItems.end()) {
// //         int rn = m_mItems[_uid]->delItem(n);
// //         if (rn < 0) {
// //             return false;
// //         } else if (rn == 0) {
// //             delete m_mItems[_uid];  // TODO
// //             m_mItems.erase(iter);
// //         }
// //         return true;
// //     }
// //     return false;
// // }


// // // bool BaseBag::transItem(BaseBag* _dest, BaseBag* _src, unsigned long _uid, int _n)
// // // {

// // //     auto iter = _src->m_mItems.find(_uid);
// // //     if (iter != _src->m_mItems.end()) {
// // //         BaseItem* bak = iter->second->getBak(_n);           // TODO 效率太低
// // //         if (BaseBag::transItem(_dest, _src, bak)) {
// // //             return true;
// // //         }
// // //         delete bak;
// // //     }

// // //     return false;
// // // }


// // // bool BaseBag::transItem(BaseBag* _dest, BaseBag* _src, BaseItem* _item)
// // // {
// // //     if (_dest->addItem(_item)) {
// // //         _src->delItem(_item);
// // //         return true;
// // //     }
// // //     return false;
// // // }

// // // bool EquipBag::addItem(BaseItem* _item)
// // // {
// // //     EquipItem* eitem = dynamic_cast<EquipItem*>(_item);
// // //     if (eitem) {
// // //         return equip(eitem);
// // //     }
// // //     return false;
// // // }

// // int EquipBag::equip(EquipItem* _item)
// // {
// //     int part = _item->getType();
// //     for(auto& iter : m_mItems) {
// //         if (part == iter.second->getType()) {
// //             return -1;   // 同步位已经装备了
// //         }
// //     }

// //     m_mItems.insert(std::make_pair(_item->getItemID(), _item));
// //     return 0;
// // }

// // // bool EquipBag::unequip(EquipItem* _item)
// // // {

// // // }

// // BaseItem* EquipBag::unequip(unsigned long _uid)
// // {
// //     auto iter = m_mItems.find(_uid);
// //     if (iter != m_mItems.end())
// //     {
// //         return iter->second;
// //     }
// // }

// Bags::Bags()
// {

// }

// Bags::~Bags()
// {

// }

// int Bags::addItem(BaseItem* _item, int n)
// {
//       if (_item) {
//         if (m_nCapacity >= m_mItems.size()) {
//             if (m_mItems.count(_baseitem->getItemID())) {
//                 if (_baseitem->isStack()) {
//                     m_mItems[_baseitem->getItemID()]->addItem(n);
//                     return true;
//                 }
//             }
//             m_mItems.insert(std::make_pair(_baseitem->getItemID(), _baseitem->getBak()));  // 获取备份
//             return true;
//         } 
//     }
// }

// int Bags::delItem(BaseItem* _item, int n)
// {

// }

// int Bags::delItem(unsigned long _n, int n)
// {

// }

// BaseItem* Bags::getItem(unsigned long _n)
// {

// }


