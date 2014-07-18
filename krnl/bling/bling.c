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
*       LUACL_LOAD_PLAYER_T_MEMBERLIST    -  The member list of struct 'player_t'.
*       LUACL_LOAD_DECLARATIONS           -  Declarations of this module.
*       LUACL_LOAD_MODULE_BODY            -  Code body of this module.
*/
#ifdef LUACL_LOAD_PLAYER_T_MEMBERLIST
k32u placeholder;
#endif /* LUACL_LOAD_PLAYER_T_MEMBERLIST */

#ifdef LUACL_LOAD_MODULE_BODY
void module_init( rtinfo_t* rti ){
}
void routine_entries( rtinfo_t* rti, event_t e ){
}
#endif /* LUACL_LOAD_MODULE_BODY */
