### �����̵ļ�����ȴʱ��
��DoTʽ��ȴ����
����ĳ����A����ȴ��60�룬��ÿ��ʹ��B���ܣ�A������ȴ����8�룺
```// A�����ͷţ�
```rti->player.spell_A.expire = 60;
```eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 5 ) ), routnum_spell_A_cd, 0 );
```
```// spell_A_cd�¼���
```if ( TIME_DISTANT( rti->player.spell_A.expire ) > FROM_SECONDS( 5 ) ) {
```    // ʵ����ȴԶδ����������Ҫ5����ٴμ��
```    eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 5 ) ), routnum_spell_A_cd, 0 );
```} else if ( rti->player.spell_A.expire > rti->timestamp ) {
```    // ʵ����ȴ�����������ڽ���������
```    eq_enqueue( rti, rti->player.spell_A.expire, routnum_spell_A_cd, 0 );
```} else if ( rti->player.spell_A.expire == rti->timestamp ) {
```    // ��������ȴ�����¼�������н������������¼���д������
```} else {
```    // ����������ȴ�����¼�������Ҫ�κζ�����
```}
```
```// B�����ͷţ�
```rti->player.spell_A.expire -= 8;
```if ( !UP( spell_A.expire ) )
```    eq_enqueue( rti, rti->timestamp, routnum_spell_A_cd, 0 );

### ��ˢ�µļ�����ȴʱ��
ֱ��ʹ�öѲ��ҽ����¼�ɾ����ʹ�þɵ���ȴλ����Ϣ��Ϊ��ʾ�����Խ��Ѳ���ɾ�����ӶȽ�����ࡣ

### ���������ֿ�ˢ�µļ��ܣ�
ʹ��DoTʽ��ȴ����

