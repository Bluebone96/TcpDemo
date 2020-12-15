#ifndef _SQL_BP_SWAP_
#define _SQL_BP_SWAP_
#include <memory>

#include "tomysql.h"
#include "../Proto/PlayerInfo.pb.h"


int sql2pb(const ITEM* _itemsql, Proto::Unity::ItemInfo* _itempb)
{
    _itempb->set_m_usrid(_itemsql->usrid);
    _itempb->set_m_itemid(_itemsql->itemid);
    _itempb->set_m_type(_itemsql->type);
    _itempb->set_m_count(_itemsql->count);
    _itempb->set_m_name(_itemsql->name);
    _itempb->set_m_hp(_itemsql->hp);
    _itempb->set_m_atk(_itemsql->atk);
    _itempb->set_m_def(_itemsql->def);
    _itempb->set_m_price(_itemsql->price);

    return 0;
}

int sql2pb(const std::shared_ptr<ITEM>& _itemsql, Proto::Unity::ItemInfo* _itempb)
{
    _itempb->set_m_usrid(_itemsql->usrid);
    _itempb->set_m_itemid(_itemsql->itemid);
    _itempb->set_m_type(_itemsql->type);
    _itempb->set_m_count(_itemsql->count);
    _itempb->set_m_name(_itemsql->name);
    _itempb->set_m_hp(_itemsql->hp);
    _itempb->set_m_atk(_itemsql->atk);
    _itempb->set_m_def(_itemsql->def);
    _itempb->set_m_price(_itemsql->price);

    return 0;
}

int pb2sql(const Proto::Unity::ItemInfo* _itempb, ITEM* _itemsql)
{
    _itemsql->usrid = _itempb->m_usrid();
    _itemsql->itemid = _itempb->m_itemid();
    _itemsql->count = _itempb->m_count();
    _itemsql->type = _itempb->m_type();
    _itemsql->name = _itempb->m_name();
    _itemsql->hp = _itempb->m_hp();
    _itemsql->mp = _itempb->m_mp();
    _itemsql->atk = _itempb->m_atk();
    _itemsql->def = _itempb->m_def();
    _itemsql->price = _itempb->m_price();
    
    return 0;
}

int sql2pb(const PLAYER* _playersql, Proto::Unity::PlayerInfo* _playerpb)
{
    _playerpb->set_id(_playersql->id);
    _playerpb->set_name(_playersql->name);
    
    _playerpb->set_speed(_playersql->speed);
    _playerpb->set_posx(_playersql->posx);
    _playerpb->set_posz(_playersql->posz);

    _playerpb->set_hp(_playersql->hp);
    _playerpb->set_mp(_playersql->mp);
    _playerpb->set_atk(_playersql->atk);
    _playerpb->set_def(_playersql->def);

    return 0;
}

int sql2pb(const std::shared_ptr<PLAYER>& _playersql, Proto::Unity::PlayerInfo* _playerpb)
{
    _playerpb->set_id(_playersql->id);
    _playerpb->set_name(_playersql->name);
    
    _playerpb->set_speed(_playersql->speed);
    _playerpb->set_posx(_playersql->posx);
    _playerpb->set_posz(_playersql->posz);

    _playerpb->set_hp(_playersql->hp);
    _playerpb->set_mp(_playersql->mp);
    _playerpb->set_atk(_playersql->atk);
    _playerpb->set_def(_playersql->def);

    return 0;
}

int pb2sql(const Proto::Unity::PlayerInfo* _playerpb, PLAYER* _playersql)
{
    _playersql->id = _playerpb->id();
    _playersql->name = _playerpb->name();

    _playersql->speed = _playerpb->speed();
    _playersql->posx = _playerpb->posx();
    _playersql->posz = _playerpb->posz();

    _playersql->hp = _playerpb->hp();
    _playersql->mp = _playerpb->mp();
    _playersql->atk = _playerpb->atk();
    _playersql->def = _playerpb->def();

    return 0;
}




#endif
