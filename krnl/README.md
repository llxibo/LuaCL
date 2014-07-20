### 可缩短的技能冷却时间
用DoT式冷却处理。
例如某技能A，冷却是60秒，但每次使用B技能，A技能冷却减少8秒：
```// A技能释放：
```rti->player.spell_A.expire = 60;
```eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 5 ) ), routnum_spell_A_cd, 0 );
```
```// spell_A_cd事件：
```if ( TIME_DISTANT( rti->player.spell_A.expire ) > FROM_SECONDS( 5 ) ) {
```    // 实际冷却远未结束，还需要5秒后再次检查
```    eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 5 ) ), routnum_spell_A_cd, 0 );
```} else if ( rti->player.spell_A.expire > rti->timestamp ) {
```    // 实际冷却即将结束，在结束处触发
```    eq_enqueue( rti, rti->player.spell_A.expire, routnum_spell_A_cd, 0 );
```} else if ( rti->player.spell_A.expire == rti->timestamp ) {
```    // 真正的冷却结束事件，如果有结束处触发的事件，写在这里
```} else {
```    // 被废弃的冷却结束事件，不需要任何动作。
```}
```
```// B技能释放：
```rti->player.spell_A.expire -= 8;
```if ( !UP( spell_A.expire ) )
```    eq_enqueue( rti, rti->timestamp, routnum_spell_A_cd, 0 );

### 可刷新的技能冷却时间
直接使用堆查找将老事件删除。使用旧的冷却位置信息作为提示，可以将堆查找删除复杂度降低许多。

### 即可缩短又可刷新的技能？
使用DoT式冷却处理。

