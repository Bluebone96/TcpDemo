#ifndef _ITEM_H_
#define _ITEM_H_


#include <string>
#include <map>

#include "../Common/Singleton.h"

// 道具类型
enum class ItemType {
    Money,                      // 货币
    CONSUME,                    // 消耗品
    EQUIP                       // 装备
};

// 道具属性类型
enum class ItemAttributeType {
    ITEM_ATTRIBUTE_ATK,         // 攻击力
    ITEM_ATTRIBUTE_HP,           // 加血
    ITEM_ATTRIBUTE_SPEED        // 移速
};


// 道具模块类型
enum class ItemModuleType {
    ITEM_MODULE_BASE,           // 基础模块
    ITEM_MODULE_POWER,          // 强化
    ITEM_MODULE_INSERT_ITEM     // 镶嵌
};

class BaseItem {
public:
    explicit BaseItem(int n = 1);
    virtual ~BaseItem();
    BaseItem(const BaseItem&, int n = 1);
    virtual void initItem(int) =0;

    void setUID(uint);
    uint getUID() const;
    void setType(int);
    int getType() const;
    int getConfID() const;
    int getCount() const;
    int setCount(int n);

    bool isSaveNow() const;
    bool isStack() const;
    bool isBind() const;
    bool setBind();

    virtual void setAttribute(ItemAttributeType key, int value) { /* none */ }
    virtual int getAttribute(ItemAttributeType key) const { /* none */ return 0; }

    virtual std::string toString() const =0;

    // 返回： 成功当前数量，失败 -1
    virtual int addItem(const int n);
    virtual int addItem(const BaseItem*);

    // 返回： 成功当前数量，失败 -1
    virtual int delItem(const int n);

    virtual BaseItem* getBak(int _n = 1) const =0;   // 默认复制数量为1
    


protected:
    uint m_nUID;                // UID  低 16 位 是 物品配置id, 高 16位是该类型物品唯一id
    int m_nFlagBit;             // 标记
    int m_nType;                // 类型  装备需要标记部位
    int m_nCount;               // 数量
    bool m_bSaveNow;            // 立即存档
};

class ItemAttribute {
public:
    void setAttribute(ItemAttributeType _key, int _value);
    int getAttribute(ItemAttributeType _key);
    ItemAttribute& operator=(const ItemAttribute&);
private:
    std::map<ItemAttributeType, int> m_mpValue;
};


class EquipItem : public BaseItem {
public:
    void initItem(int) override;
    int addItem(const int) override;
    std::string toString() const override;
    BaseItem* getBak(int n) const override;
private:

    std::map<ItemModuleType, ItemAttribute> m_mAttributes;
};


class MoneyItem : public BaseItem {
public:
    explicit MoneyItem(int n = 1);
    ~MoneyItem();
    MoneyItem(const MoneyItem&, int);
    void initItem(int) override;

    std::string toString() const override;
    BaseItem* getBak(int n)  const override;
};


class ConsumItem : public BaseItem {
public:
    explicit ConsumItem(int _n = 1);
    ~ConsumItem();
    ConsumItem(const ConsumItem&, int _n = 1);

    virtual int getAttribute(ItemAttributeType key) const;
    
    void initItem(int) override;
    std::string toString() const override;
    BaseItem* getBak(int) const override;

private:
    ItemAttribute m_Attribute;
};

class ItemFactory {
public:
    BaseItem* CreateItem(int type, int n);
};

#define ITEMFACTORY Singleton<ItemFactory>::GetInstance()

#endif
