/*
    Sample class module.
        2014.7.18, Aean
    This module simulates a robot called 'Bling', which have several classic spells.
    You can derive your own module from this sample.

    I have made a guide to build minimalist C++ simulator a long time ago, which uses
    simplified Bling for example. This time Bling will be a little bit more versatile.
*/

/**
*   Class moudle source file is cutted to several segments, each filled into different
*   location of 'frm.c', which is the simulator's back-end framework.
*   To seperate segments, use old-style C header guard words:
*       LUACL_LOAD_DECLARATIONS           -  Data structures of this module, which could be used as a member of player_t or snapshot_t.
*       LUACL_LOAD_SNAPSHOT_T_MEMBERLIST  -  The member list of struct 'snapshot_t', which is used to preserve past states for scheduled events.
*       LUACL_LOAD_PLAYER_T_MEMBERLIST    -  The member list of struct 'player_t'.
*       LUACL_LOAD_MODULE_BODY            -  Code body of this module.
*/
#ifdef LUACL_LOAD_DECLARATIONS

    typedef struct {
        time_t expire;
    } livingbomb_t;
    typedef struct {
        time_t cd;
        time_t expire;
    } bsod_t;

#endif /* LUACL_LOAD_DECLARATIONS */

#ifdef LUACL_LOAD_SNAPSHOT_T_MEMBERLIST
    float fp[1];
    k32u ip[1];
#endif /* LUACL_LOAD_SNAPSHOT_T_MEMBERLIST */


#ifdef LUACL_LOAD_PLAYER_T_MEMBERLIST
    livingbomb_t livingbomb;
    bsod_t bsod;
    time_t gcd;
#endif /* LUACL_LOAD_PLAYER_T_MEMBERLIST */

/**
*   Your module have to implement at least two functions:
*
*       void module_init ( rtinfo_t* rti )
*           Put initialization codes here.
*           When a thread is launched the simulator initializer will invoke this function.
*
*       void routine_entries ( rtinfo_t* rti, event_t e )
*           Define the behaviors for all kinds of events.
*           When an event 'e' is executed the EQ framework will invoke this function.
*
*   Any other functions is at your options.
*/
#ifdef LUACL_LOAD_MODULE_BODY
enum{
    DMGTYPE_NORMAL,
    DMGTYPE_PHISICAL,
    DMGTYPE_MAGIC,
};
void deal_damage( rtinfo_t* rti, float dmg, k8u dmgtype ) {
    switch( dmgtype ){
    case DMGTYPE_NORMAL:
        break;
    case DMGTYPE_PHISICAL:
        dmg *= ( 24835.0f / (24835.0f + 4037.5f * 90.0f - 317117.5f ) );
        break;
    case DMGTYPE_MAGIC:
        if (rti->player.bsod.expire >= rti->timestamp) dmg *= 1.3f;
        dmg *= 1.04f;
        break;
    }
    rti->damage_collected += dmg;
}

/* Event list. */
#define DECL_EVENT( name ) void event_##name ( rtinfo_t* rti, k8u snapshot )
#define HOOK_EVENT( name ) case routnum_##name: event_##name( rti, e.snapshot ); break;
#define DECL_SPELL( name ) void spell_##name ( rtinfo_t* rti )
#define SPELL( name ) spell_##name ( rti )
enum{
    routnum_gcd_expire,
    routnum_smackthat_land,
    routnum_livingbomb_execute,
    routnum_livingbomb_tick,
    routnum_livingbomb_expire,
    routnum_bsod_execute,
    routnum_bsod_cd,
    routnum_bsod_expire,
};
DECL_EVENT( gcd_expire );
DECL_EVENT( smackthat_land );
DECL_EVENT( livingbomb_execute );
DECL_EVENT( livingbomb_tick );
DECL_EVENT( livingbomb_expire );
DECL_EVENT( bsod_execute );
DECL_EVENT( bsod_expire );
DECL_EVENT( bsod_cd );

void gcd_start ( rtinfo_t* rti, time_t length ) {
    rti->player.gcd = TIME_OFFSET( length );
    eq_enqueue( rti, rti->player.gcd, routnum_gcd_expire, 0 );
}
DECL_EVENT( gcd_expire ) {
    /* Do nothing. */
}

DECL_EVENT( smackthat_land ) {
    deal_damage( rti, 500.0f, DMGTYPE_PHISICAL );
}

DECL_EVENT( livingbomb_execute ) {
    snapshot_t* ss;
    snapshot = snapshot_save( rti, &ss );
    rti->player.livingbomb.expire = TIME_OFFSET( FROM_SECONDS( 15 ) );
    ss->ip[0] = (k32u)rti->player.livingbomb.expire;
    eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 3 ) ), routnum_livingbomb_tick, snapshot );
}

DECL_EVENT( livingbomb_tick ) {
    time_t next_tick;
    snapshot_t* ss = snapshot_read( rti, snapshot );
    /* Livingbomb may be refreshed. */
    if ( ss->ip[0] != (k32u)rti->player.livingbomb.expire ){
        snapshot_kill( rti, snapshot );
        return;
    }
    deal_damage( rti, 100.0f, DMGTYPE_MAGIC );
    next_tick = TIME_OFFSET( FROM_SECONDS( 3 ) );
    if ( next_tick <= rti->player.livingbomb.expire ) {
        eq_enqueue( rti, next_tick, routnum_livingbomb_tick, snapshot );
    }else{
        eq_enqueue( rti, rti->player.livingbomb.expire, routnum_livingbomb_expire, snapshot );
    }
}

DECL_EVENT( livingbomb_expire ) {
    /* Livingbomb may be refreshed. Double check the expire time. */
    snapshot_t* ss = snapshot_read( rti, snapshot );
    if ( ss->ip[0] == (k32u)rti->player.livingbomb.expire )
        deal_damage( rti, 600.0f, DMGTYPE_MAGIC );
    snapshot_kill( rti, snapshot );
}

DECL_EVENT( bsod_execute ) {
    power_gain( rti, 30.0f );
    rti->player.bsod.cd = TIME_OFFSET( FROM_SECONDS( 8 ) );
    eq_enqueue( rti, rti->player.bsod.cd, routnum_bsod_cd, 0 );
    rti->player.bsod.expire = TIME_OFFSET( FROM_SECONDS( 5 ) );
    eq_enqueue( rti, rti->player.bsod.expire, routnum_bsod_execute, 0 );
}

DECL_EVENT( bsod_cd ) {
    /* Do nothing. */
}

DECL_EVENT( bsod_expire ) {
    /* Do nothing. */
}

DECL_SPELL( smackthat ) {
    if ( rti->player.gcd > rti->timestamp ) return;
    if ( !power_check( rti, 5.0f ) ) return;
    power_consume( rti, 5.0f );
    gcd_start( rti, FROM_SECONDS( 1.5 ) );
    eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 1 ) ), routnum_smackthat_land, 0 );
}
DECL_SPELL( livingbomb ) {
    if ( rti->player.gcd > rti->timestamp ) return;
    if ( !power_check( rti, 20.0f ) ) return;
    power_consume( rti, 20.0f );
    gcd_start( rti, FROM_SECONDS( 1.5 ) );
    eq_enqueue( rti, rti->timestamp, routnum_livingbomb_execute, 0 );
}
DECL_SPELL( bsod ) {
    if ( rti->player.gcd > rti->timestamp ) return;
    if ( rti->player.bsod.cd > rti->timestamp ) return;
    gcd_start( rti, FROM_SECONDS( 1.5 ) );
    eq_enqueue( rti, rti->timestamp, routnum_bsod_execute, 0 );
}

void routine_entries( rtinfo_t* rti, event_t e ){
    switch(e.routine){
        HOOK_EVENT( gcd_expire );
        HOOK_EVENT( smackthat_land );
        HOOK_EVENT( livingbomb_execute );
        HOOK_EVENT( livingbomb_tick );
        HOOK_EVENT( livingbomb_expire );
        HOOK_EVENT( bsod_execute );
        HOOK_EVENT( bsod_cd );
        HOOK_EVENT( bsod_expire );
    default:
        assert( 0 );
    }
}

void module_init( rtinfo_t* rti ){
    rti->player.power_regen = 3.0f;
    rti->player.power = 100.0f;
}
#endif /* LUACL_LOAD_MODULE_BODY */
