#ifndef _ITEM_H_
#define _ITEM_H_


#include <string>
#include <map>

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
    ITEM_MODULE_INSERT_ITEM     // 镶嵌
};


class BaseItem {
public:
    virtual void setUID(int);
    virtual int getUID();
    virtual void setType(int);
    virtual int getType();
    virtual int getConfID();
    virtual bool isStack()=0;
    virtual bool isBind()=0;
    virtual void setAttribute(ItemAttributeType key, int value) = 0;
    virtual int getAttribute(ItemAttributeType key) = 0;
    virtual void initItem() = 0;
    virtual std::string toString() =0;
    virtual void addItem(const BaseItem*)=0;
protected:
    unsigned long m_nUID;       // UID  低 32 位 是 物品配置id, 高32位是该类型物品唯一id
    int m_nFlagBit;             // 标记
    int m_nType;                // 类型
    int m_nCount;               // 数量
    bool m_bSaveNow;            // 立即存档
};

class ItemAttribute {
public:
    std::map<ItemAttributeType, int> m_mpValue;
    BaseItem* m_pFather;
private:
    void setAttribute(ItemAttributeType _key, int _value);
    int getAttribute(ItemAttributeType _key);
};


class EquipItem : public BaseItem {
public:
    void initItem();
    std::string toString();

private:
    std::map<ItemModuleType, ItemAttribute> m_mAttributes;
};


class MoneyItem : public BaseItem {
public:
    void initItem() override;
    void setAttribute(ItemAttributeType key, int value) override;
    int getAttribute(ItemAttributeType key) override;
    bool isBind() override;
    bool isStack() override;
    void addItem(const BaseItem*) override;
    std::string toString() override;
};


class ConsumItem : public BaseItem {
public:
    void initItem();
    std::string toString();
};

class ItemFactory {
public:
    BaseItem* CreateItem(int type);
};



#endif
