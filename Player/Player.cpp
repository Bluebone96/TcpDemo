#include "Player.h"
#include "../Common/MsgTransmission.hpp"
#include "../build/PlayerInfo.pb.h"
#include <vector>
#include <cstdlib>

unsigned int BKDRHash(const std::string& _str)
{
    unsigned int seed = 1313;
    unsigned int hash = 0;
    for (auto& c : _str) {
        hash = hash * seed + c;
    }
    return (hash & 0x7FFFFFFF);
}


int Player::InitPlayer()
{

    Proto::Unity::Authentication A;
    m_msgTrans->Decode(A);
    
    unsigned int id = BKDRHash(A.name());
    unsigned int pass = BKDRHash(A.password());

    char tmp1 [30];
    snprintf(tmp1, 30, "pass_%d", id);

    char* data = nullptr;
    int n = 0;
    if (m_predis->GetByBit(tmp1, (void**)&data, &n)) {
        
    }
    if (data) {
        *((unsigned int*)data) == pass;
        
        snprintf(tmp1, 30, "usr_%d", id);

        if (m_predis->GetByBit(tmp1, (void**)&data, &n)) {

        }
        if (data) {
            m_protoInfo.ParseFromArray(data, n);
            snprintf(m_idstr, 20, "%d", id);
            return 0;
        }
    }


    // redis 没找到， 去数据库找
    std::vector<PASS> passSql;

    char tmp[40];

    snprintf(tmp, 40, "SELECT * FROM PASS where id = %d", id);

    if (m_pmysql->GetBySQL(passSql, tmp)) {
        
    }

    if (passSql[0].pass != pass) {
        return -1;
    }

    std::vector<PLAYER> playerSql;

    snprintf(tmp, 40, "SELECT * FROM PLAYER where id = %d", id);
    if (m_pmysql->GetBySQL(playerSql, tmp)) {

    }
    

    TRACER("player name: %s\n id: %d", A.name().c_str(), m_Id);

    m_Id = id;
    m_name = playerSql[0].name;
    m_protoInfo.set_id(id);
    m_protoInfo.set_name(m_name);
    m_protoInfo.set_hp(playerSql[0].hp);
    m_protoInfo.set_posx(playerSql[0].posx);
    m_protoInfo.set_posz(playerSql[0].posz);
    m_protoInfo.set_speed(playerSql[0].speed);

    m_protoInfo.set_state(0);
    return 0;
}


int Player::Update()
{
    TRACER("test Player UpDate\n");
    struct timeval  curTime;

    m_msgTrans->Decode(m_opNew);
    TRACER("new op h = %f, v = %f\n", m_opNew.h(), m_opNew.v());

    gettimeofday(&curTime, nullptr);
    
    if (curTime.tv_sec - m_offline.tv_sec > 5) {
        // TODO 离线
    }

    m_offline = curTime;


    float passtime = (curTime.tv_sec  - lastTimeUp.tv_sec) * 1000 + (curTime.tv_usec - lastTimeUp.tv_usec) / 1000;

    TRACER("passtime is %f\n", passtime);
    // PlayerStatus nextStatus = m_pStatus[(m_pos + 1) % MAXSTATUS];

    // nextStatus = m_pStatus[m_pos];

    m_protoInfo.set_posx(m_protoInfo.posx() + (passtime * m_protoInfo.speed() * m_protoInfo.op().h()) / 1000);
    m_protoInfo.set_posz(m_protoInfo.posz() + (passtime * m_protoInfo.speed() * m_protoInfo.op().v()) / 1000);
    m_protoInfo.mutable_op()->CopyFrom(m_opNew);

    TRACER("player posx = %f, posz = %f\n", m_protoInfo.posx(), m_protoInfo.posz());
    TRACER("test Player UpDate end\n");
    lastTimeUp = curTime;
    return 0;
}


// int Player::setPlayerInfo()
// {


//     m_protoInfo.set_id(currStatus.m_id);
//     m_protoInfo.set_hp(currStatus.m_hp);
//     m_protoInfo.set_speed(currStatus.m_speed);
//     m_protoInfo.set_state(currStatus.m_state);
    
//     return ERR;
// }


PROTOBUF& Player::GetPlayerInfo()
{

    TRACER("test PlayerGetPlayerInfo\n");
    Update();

//    setPlayerInfo();

    TRACER("test PlayerGetPlayerInfo end\n");
    return m_protoInfo;
}

int Player::updateInventroy()
{

/*
 * optype : value
 *  0       basebag 增加物品，add1 为数量
 *  1       basebag 减少物品，add1 为数量
 *  2       basebag 使用物品，add1 为数量
 *  3       money   增加， add1 为数量
 *  4       money   减少   add1 为数量
 *  5       equip   装备
 *  6       unequip  卸下装备
 *  7       trade   交易物品，add1 为数量， add2 为交易对象 id
 *  8       
 */
    int rn = -1;   // 结果标记
    Proto::Unity::ItemEvent itemEvent;
    m_msgTrans->Decode(itemEvent);
    switch (itemEvent.optype())
    {
        case 0:
            // TODO 装备工厂产生了一个装备后，写入数据库，然后这里去数据库找到了再加入
            // m_inventory.addItem(nullptr);
            rn = m_inventory.addItem(itemEvent.uid(), itemEvent.count());
            break;
        case 1:
            rn = m_inventory.delItem(itemEvent.uid(), itemEvent.count());
            break;
        case 2:
            rn = m_inventory.useItem(itemEvent.uid());
            break;
        case 3:
            rn = m_inventory.equipItem(itemEvent.uid());
            break;
        case 4:
            rn = m_inventory.unequipItem(itemEvent.uid());
            break;
        default:
            break;
    }

    if (rn >= 0) {
        m_msgTrans->sendmsg(itemEvent);
    }
}


int Player::savePlayer()
{
    PLAYER playersql;

    playersql.hp = m_protoInfo.hp();
    playersql.posx = m_protoInfo.posx();
    playersql.posz = m_protoInfo.posz();

    m_pmysql->SetBySQL(playersql);

    char tmp [30];

    snprintf(tmp, 30, "usr_%d", m_Id);

    int size = m_protoInfo.ByteSizeLong();
    char tmp2[size];

    m_protoInfo.SerializeToArray(tmp2, size);

    m_predis->SetByBit(tmp, tmp2, size);

    return 0;
}


int Player::saveItem(BaseItem* _item)
{
    ITEM item;

    item.itemid = _item->getUID();

    item.userid = m_Id;

    item.type = _item->getType();

    item.count = _item->getCount();

    item.hp = _item->getAttribute(ItemAttributeType::ITEM_ATTRIBUTE_HP);
    item.atk = _item->getAttribute(ItemAttributeType::ITEM_ATTRIBUTE_ATK);

    m_pmysql->SetBySQL(item);


    char key[20] = {0};
    char fiel[20] = {0};
    snprintf(key, 20, "%d", item.userid);
    snprintf(fiel, 20, "%d", item.itemid);

    Proto::Unity::Items item2;
    item2.set_m_uid(item.itemid);
    item2.set_m_type(item.type);
    item2.set_m_count(item.count);
    item2.set_m_hp(item.hp);
    item2.set_m_atk(item.atk);

    int size = item2.ByteSizeLong();
    char tmp[size];
    item2.SerializeToArray(tmp, size);

    m_predis->HSetByBit(key, fiel, tmp, size);

    return 0;
}



int Player::saveAll() {
    this->savePlayer();
    m_inventory.saveAll();
    return 0;
}