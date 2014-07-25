������ʩ
----

* ��һ��������

    OpenCL C��C���Ե�����ϵͳ�����˸Ľ�����������һЩ���磬���Կ���ṩ�˹�һ�������͡�

    `kbool` ��������

    `k8s` �з���char( ��C�����ж�Ӧ `signed char` ���� `char` )

    `k8u` �޷���char

    `k16s` �з���short

    `k16u` �޷���short

    `k32s` �з���long����ӦOpenCL C�е�`int`��

    `k32u` �޷���long����ӦOpenCL C�е�`uint`��

    `k64s` �з���long long����ӦOpenCL C�е�`long`��

    `k64u` �޷���long long����ӦOpenCL C�е�`ulong`��

    ��������ʹ��Դ����

    `K64S_C` Ϊһ�����ֳ�����Ӻ�׺��ʹ���Ϊ`k64s`���͡�

    `K64U_C` Ϊһ�����ֳ�����Ӻ�׺��ʹ���Ϊ`k64u`���͡�

    `K64U_MSB`Ϊ����`0x1000000000000000`


* C��׼��

    ����������޵�ʹ��C��׼�⣬����Ӧ���벻�ܴ���OpenCL�������У��ο�`hostonly()`��������ʹ�õĺ�������`printf`��`abort`��`malloc`��`free`��

    ֧�ֶ���`assert`����OpenCL����ʱ������`assert`���ᱻ���ԡ�


* OpenCL������C�����е����

    ��C�����У�ʹ��`set_global_id(dim, idx)`���õ�ǰʱ�̵�ǰ�̹߳���ĳһά��`dim`��������`get_global_id`���õ����һ��`set_global_id`�����õ�ֵ��

    

    OpenCL C���ṩ�˷ḻ������ת�������������C������ʵ����`convert_ushort_sat`��`convert_ushort_rtp`��`convert_float_rtp`��

    `convert_ushort_sat`��`convert_ushort_rtp`Ҫ��������ʽ���ܺ����κα߽�ЧӦ��`convert_float_rtp`Ҫ��������������Ȱ�ȫת����`k64u`���͡�

    

    ��C������ʵ����`clz`��`cos`��`cospi`��`sqrt`��`log`��`clamp`��`max`��`min`��`mix`��

    ���У�`clz`���Ƚ�������ת��Ϊ`k64u`���ͣ�`cos`��`cospi`��`sqrt`��`log`��`clamp`���Ƚ�������ת��Ϊ`double`���ͣ�`max`��`min`��`mix`Ҫ��������ʽ���ܺ����κα߽�ЧӦ��


* `hostonly()`��`deviceonly()`��`msvconly()`

    ������`hostonly()`�����еĴ�����OpenCL����ʱ�������ԣ��෴�أ�������`deviceonly()`�����еĴ����ڷ�OpenCL����ʱ�������ԡ�������`msvconly()`�����еĴ����ڳ�MSVC����ı���������ʱ�������ԡ�

    *��*

        struct {
            struct {
                k32u a;
                k32u b;
            } c;
            k32u d;
        } foo = {
            msvconly( 0 )
        };

    ��MSVC�У���һ�ԿյĴ�����`{}`��Ϊ�ṹ���ֵʱ����õ����������Ҫ���һ��`0`����GCC�У�����������ӵ�`0`��`c`�����Ͳ�������õ����뾯�档ʹ��`msvconly( 0 )`���˴������GCC���أ�ʹ�ô��������ֻ����о�������������

* `KRNL_STR()`

    ������`KRNL_STR()`�����еĴ��뽫��ת��Ϊ�ַ�����������ʹ��`KRNL_STR`�Ĵ��벻�ܴ���OpenCL�������У��ο�`hostonly()`����

    *��*

        k32u abc = 4262;
        printf("%s = %d", KRNL_STR(abc), abc);

    ���õ����`abc = 4262`��


* `kdeclspec()`��`hdeclspec()`

    `kdeclspec()`ָ��չʾ��OpenCL�������������޶������൱��`__attribute__((...))`��`hdeclspec()`ָ��չʾ����ͨC���Ա������������޶�������MSVC��Ϊ`__declspec(...)`����������������Ϊ`__attribute__((...))`��


* `htlsvar`

    `htlsvar`��TLS���̱߳��ش洢����ʶ����MSVC��Ϊ`__declspec(thread)`����������������Ϊ`__thread`����ʹ��`htlsvar`�Ĵ��벻�ܴ���OpenCL�������У��ο�`hostonly()`����


ʱ���
----

* `time_t`

    �ڿ���У�ʱ������`time_t`������Ϊ`k16u`��`rti->timestamp`����ģ��ĵ�ǰʱ�����

    ʱ��ֱ��ʱ�����Ϊ10���룬����`(time_t)1`�ȼ���0:00.010��`(time_t)65535`�ȼ���10:55.350��

    ���`time_t`���ڱ�ﳬ��10:55.350��ʱ���������Ϊδ���塣���`time_t`���ڱ������0:00.000��ʱ���������Ϊδ���塣

    ������Ҫ��Ե�λת������������뷽��ȶ������⣬ֱ�Ӷ�`time_t`���ͽ�����������ǲ����ᳫ�ģ�Ӧ����ʹ�ÿ���ṩ�ĸ��ֲ���������


* `FROM_SECONDS( sec )`

    ��`sec`����ת��Ϊ`time_t`����`sec`������û��Ҫ�󣬵���`sec`Ϊ������`float`���ͱ���ʱ����Ч����ߣ���Ϊ�����ڲ�ʵ���У�`sec`������ת��Ϊ`float`��

    ������ʱ�䳬��`time_t`�ı�ʾ��Χʱ������Ϊδ���塣

    ������ʱ�䲻��׼ȷ��`time_t`��ʾʱ��`FROM_SECONDS`����������ȡ�������������ȡ����

    *��*

        FROM_SECONDS( 6 )     /* (time_t)600 */
        FROM_SECONDS( 6.01 )  /* (time_t)601 */
        FROM_SECONDS( 6.001 ) /* (time_t)601 */
        FROM_SECONDS( 6.000001 ) /* (time_t)601 */
        FROM_SECONDS( 6.0000000000001 ) /* (time_t)600 */


* `FROM_MILLISECONDS( msec )`

    ��`FROM_SECONDS`���ƣ��ɺ���ת��Ϊ`time_t`��


* `TO_SECONDS( timestamp )`

    ��`timestamp`��`time_t`ת��Ϊ�롣��������Ϊ`float`�����`timestamp`����`time_t`���ͣ���ᱻǿ��ת��Ϊ`k16u`���ͣ������ܱ�֤������`time_t`�൱��

    ����������׼ȷʹ��`float`��ʾ��`TO_SECONDS`����������ȡ���������������ȡ�����������������ʵ����Ӧ�ò��ᷢ����

    *��*

        TO_SECONDS( 601 ) /* 6.01f */


* `TIME_OFFSET( delta )`

    �Ե�ǰʱ��`rti->timestamp`Ϊ��׼����δ������ƫ�Ƴ���Ϊ`delta`��ʱ�䡣����ֵ��`delta`��Ϊ`time_t`���͡����`delta`����`time_t`���ͣ���ᱻǿ��ת��Ϊ`k32s`���ͣ������ܱ�֤������`time_t`�൱��

    ������������`time_t`�ı�ʾ��Χ�����õ�����ֵ��

    *��*

        rti->timestamp = FROM_SECONDS( 8 );
        TIME_OFFSET( FROM_SECONDS( 6 ) ) /* ��ȷ��(time_t)1400 */
        TIME_OFFSET( FROM_SECONDS( -6 ) ) /* δ������Ϊ */
        TIME_OFFSET( -FROM_SECONDS( 6 ) ) /* δ������Ϊ */
        
        rti->timestamp = FROM_SECONDS( 655 );
        TIME_OFFSET( FROM_SECONDS( 6 ) ) /* ����Ͻ磬�õ�(time_t)65535 */


* `TIME_DISTANT( time )`

    �Ե�ǰʱ��`rti->timestamp`Ϊ��׼�����㵽��`time`��ʱ������ֵ��`time`��Ϊ`time_t`���͡����`time`����`time_t`���ͣ���ᱻǿ��ת��Ϊ`k32s`���ͣ������ܱ�֤������`time_t`�൱��

    ������������`time_t`�ı�ʾ��Χ�����õ�����ֵ��

    *��*

        rti->timestamp = FROM_SECONDS( 8 );
        TIME_DISTANT( FROM_SECONDS( 10 ) ) /* ��ȷ��(time_t)200 */
        TIME_DISTANT( FROM_SECONDS( 6 ) ) /* ����½磬�õ�(time_t)0 */


* `UP( time_to_check )`

    ��д�ꡣ��`rti->player`�ṹ�г�ȡ��Ϊ`time_to_check`�ĳ�Ա����������ʾ��ʱ���Ƿ��ڵ�ǰʱ��֮�����򷵻��棬���򷵻ؼ١�

    _�����ʱ���뵱ǰʱ�����ʱ�����ؼ�_��

    *��*

        rti->timestamp = FROM_SECONDS( 8 );
        rti->player.aura_foo.expire = FROM_SECONDS( 10 );
        UP( aura_foo.expire ) /* ������ */
        rti->player.spell_bar.cd = FROM_SECONDS( 8 );
        UP( spell_bar.cd ) /* ���ؼ� */


* `REMAIN( time_to_check )`

    ��д�ꡣ��`rti->player`�ṹ�г�ȡ��Ϊ`time_to_check`�ĳ�Ա��������`TIME_DISTANT`��

    *��*

        rti->timestamp = FROM_SECONDS( 8 );
        rti->player.aura_foo.expire = FROM_SECONDS( 10 );
        REMAIN( aura_foo.expire ) /* (time_t)200 */


##### ʱ���ʱ��߽�

> ����ʱ��ʱ������Ҫ�ر�ע�ⷢ����-��Ծ���ʱ�̡�������ȴ������ʱ�̣�Ч��������ʱ�̣��ȵȡ�

> һ��أ�����ȴ������˲�䣬Ӧ�ж�Ϊ����ʩ�ŷ�������`UP(spell.cd)`Ϊ�١���������ȴ����ʱ�̹ҹ����¼����޷�ִ���κζ���������ģ�ⲻ�ܼ����ƽ���

> ���Ǻ�`UP`�߽紦��ʽ���ڲ�ԭ�򡣶�һЩЧ���Ľ���ʱ�̣��������෴����Ҫ�ж�Ϊ�档

> ��ʱ���Դ󵨵�ʹ��hack�ķ�ʽ����Ч������ʱ������`(time_t)1`��ʹ���ǵ�`UP`�����ڱ߽紦ȡ����ֵ��

> ��������_����_����Ч���ĳ���ʱ�䱻�ӳ�����Ϊ���ӵ�`(time_t)1`��ԭ��ʱ�䣬��ɵ�Ӱ��ֻ���ڱ߽紦��ȡֵ�ɼٸ�Ϊ�棬�������κ�ʱ�̵�ȡֵ��û�з����仯��


------
###### �����̵ļ�����ȴʱ��
��DoTʽ��ȴ����
����ĳ����A����ȴ��60�룬��ÿ��ʹ��B���ܣ�A������ȴ����8�룺

    // A�����ͷţ�
    rti->player.spell_A.cd = 60;
    eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 5 ) ), routnum_spell_A_cd, 0 );
        
    // spell_A_cd�¼���
    if ( TIME_DISTANT( rti->player.spell_A.cd ) > FROM_SECONDS( 5 ) ) {
        // ʵ����ȴԶδ����������Ҫ5����ٴμ��
        eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 5 ) ), routnum_spell_A_cd, 0 );
    } else if ( rti->player.spell_A.cd > rti->timestamp ) {
        // ʵ����ȴ�����������ڽ���������
        eq_enqueue( rti, rti->player.spell_A.cd, routnum_spell_A_cd, 0 );
    } else if ( rti->player.spell_A.cd == rti->timestamp ) {
        // ��������ȴ�����¼�������н������������¼���д������
    } else {
        // ����������ȴ�����¼�������Ҫ�κζ�����
    }
        
    // B�����ͷţ�
    rti->player.spell_A.cd -= 8;
    if ( !UP( spell_A.cd ) )
        eq_enqueue( rti, rti->timestamp, routnum_spell_A_cd, 0 );

###### ��ˢ�µļ�����ȴʱ��
ֱ��ʹ�öѲ��ҽ����¼�ɾ����ʹ�þɵ���ȴλ����Ϣ��Ϊ��ʾ�����Խ��Ѳ���ɾ�����ӶȽ�����ࡣ

###### ���������ֿ�ˢ�µļ��ܣ�
ʹ��DoTʽ��ȴ����

