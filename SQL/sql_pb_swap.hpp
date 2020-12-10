#ifndef _SQL_BP_SWAP_
#define _SQL_BP_SWAP_

#include "tomysql.h"
#include "../Proto/PlayerInfo.pb.h"



int sql2pb(ITEM& _itemsql, Proto::Unity::ItemInfo* _itempb)
{
    _itempb->set_m_uid(_itemsql.itemid);
    _itempb->set_m_count(_itemsql.count);
    _itempb->set_m_type(_itemsql.type);
    _itempb->set_m_hp(_itemsql.hp);
    _itempb->set_m_atk(_itemsql.atk);

    return 0;
}

int pb2sql(Proto::Unity::ItemInfo* _itempb, ITEM& _itemsql)
{
    // todo item 应增加 usrid
    _itemsql.itemid = _itempb->m_uid();
    _itemsql.count = _itempb->m_count();
    _itemsql.type = _itempb->m_type();
    _itemsql.hp = _itempb->m_hp();
    _itemsql.atk = _itempb->m_atk();

    return 0;
}

int sql2pb(PLAYER& _playersql, Proto::Unity::PlayerInfo& _playerpb)
{
    _playerpb.set_id(_playersql.id);
    _playerpb.set_name(_playersql.name);
    
    _playerpb.set_speed(_playersql.speed);
    _playerpb.set_posx(_playersql.posx);
    _playerpb.set_posz(_playersql.posz);

    _playerpb.set_hp(_playersql.hp);
    _playerpb.set_mp(_playersql.mp);
    _playerpb.set_atk(_playersql.atk);
    _playerpb.set_def(_playersql.def);

    return 0;
}

int pb2sql(Proto::Unity::PlayerInfo& _playerpb, PLAYER& _playersql)
{
    _playersql.id = _playerpb.id();
    _playersql.name = _playerpb.name();

    _playersql.speed = _playerpb.speed();
    _playersql.posx = _playerpb.posx();
    _playersql.posz = _playerpb.posz();

    _playersql.hp = _playerpb.hp();
    _playersql.mp = _playerpb.mp();
    _playersql.atk = _playerpb.atk();
    _playersql.def = _playerpb.def();

    return 0;
}




#endif
