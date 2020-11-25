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
    BaseItem();
    virtual ~BaseItem();
    virtual void initItem(int) = 0;

    void setUID(unsigned long);
    unsigned long getUID();
    void setType(int);
    int getType();
    int getConfID();

    bool isStack();
    bool isBind();
    bool setBind();
    
    virtual void setAttribute(ItemAttributeType key, int value);
    virtual int getAttribute(ItemAttributeType key);
    virtual std::string toString() =0;

    virtual int getCount();

    // 返回： 成功当前数量，失败 -1
    virtual int addItem(const int n) =0;
    virtual int addItem(const BaseItem*);

    // 返回： 成功当前数量，失败 -1
    virtual int delItem(const int n);

    virtual BaseItem* getBak(int _n = 1) =0;   // 默认复制数量为1
protected:
    unsigned long m_nUID;       // UID  低 32 位 是 物品配置id, 高32位是该类型物品唯一id
    int m_nFlagBit;             // 标记
    int m_nType;                // 类型  装备需要标记部位
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
    void initItem(int) override;
    int addItem(const int) override;
    std::string toString() override;
    BaseItem* getBak(int n) override;
private:
    void setAttribute(ItemAttributeType key, int value) override;
    int getAttribute(ItemAttributeType key) override;
    std::map<ItemModuleType, ItemAttribute> m_mAttributes;
};


class MoneyItem : public BaseItem {
public:
    void initItem(int) override;
    int addItem(const BaseItem*) override;
    int addItem(const int ) override;
    std::string toString() override;
    BaseItem* getBak(int n) override;
};


class ConsumItem : public BaseItem {
public:
    void initItem(int) override;
    int addItem(const BaseItem*) override;
    int addItem(const int ) override;
    std::string toString() override;
    BaseItem* getBak(int) override;
};

class ItemFactory {
public:
    BaseItem* CreateItem(int type, int n);
};



#endif
