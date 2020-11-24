#ifndef _ITEM_H_
#define _ITEM_H_

// 道具类型
enum class ItemType {
    Money,                      // 货币
    CONSUME,                    // 消耗品
    EQUIP                       // 装备
};

// 道具属性类型
enum class ItemAttributeType {
    ITEM_ATTRIBUTE_ATK,         // 攻击力
    ITEM_ATTRIBUTE_HP           // 加血
};


// 道具模块类型
enum class ItemModuleType {
    ITEM_MODULE_BASE,           // 基础模块
    ITEM_MODULE_POWER,          // 强化
    ITEM_MODULE_INSERT_ITEM      // 镶嵌
};


class ItemFactory {
public:
    BaseItem* CreateItem(int type)
}

class BaseItem {


}


#endif
