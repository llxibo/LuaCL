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

    `K64S_C( num )` Ϊһ�����ֳ�����Ӻ�׺��ʹ���Ϊ`k64s`���͡�

    `K64U_C( num )` Ϊһ�����ֳ�����Ӻ�׺��ʹ���Ϊ`k64u`���͡�

    `K64U_MSB`Ϊ����`0x1000000000000000`


* C��׼��

    ����������޵�ʹ��C��׼�⣬����Ӧ���벻�ܴ���OpenCL�������У��ο�`hostonly`��������ʹ�õĺ�������`printf`��`abort`��`malloc`��`free`��

    ֧�ֶ���`assert`����OpenCL����ʱ������`assert`���ᱻ���ԡ�


* OpenCL������C�����е����

    ��C�����У�ʹ��`set_global_id(dim, idx)`���õ�ǰʱ�̵�ǰ�̹߳���ĳһά��`dim`��������`get_global_id`���õ����һ��`set_global_id`�����õ�ֵ��

    

    OpenCL C���ṩ�˷ḻ������ת�������������C������ʵ����`convert_ushort_sat`��`convert_ushort_rtp`��`convert_float_rtp`��

    `convert_ushort_sat`��`convert_ushort_rtp`Ҫ��������ʽ���ܺ����κα߽�ЧӦ��`convert_float_rtp`Ҫ��������������Ȱ�ȫת����`k64u`���͡�

    

    ��C������ʵ����`clz`��`cos`��`cospi`��`sqrt`��`log`��`clamp`��`max`��`min`��`mix`��

    ���У�`clz`���Ƚ�������ת��Ϊ`k64u`���ͣ�`cos`��`cospi`��`sqrt`��`log`��`clamp`���Ƚ�������ת��Ϊ`double`���ͣ�`max`��`min`��`mix`Ҫ��������ʽ���ܺ����κα߽�ЧӦ��


* `hostonly`��`deviceonly`��`msvconly`

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

* `KRNL_STR`

    ������`KRNL_STR()`�����еĴ��뽫��ת��Ϊ�ַ�����������ʹ��`KRNL_STR`�Ĵ��벻�ܴ���OpenCL�������У��ο�`hostonly`����

    *��*

        k32u abc = 4262;
        printf("%s = %d", KRNL_STR(abc), abc);

    ���õ����`abc = 4262`��


* `kdeclspec`��`hdeclspec`

    `kdeclspec(...)`ָ��չʾ��OpenCL�������������޶������൱��`__attribute__((...))`��`hdeclspec(...)`ָ��չʾ����ͨC���Ա������������޶�������MSVC��Ϊ`__declspec(...)`����������������Ϊ`__attribute__((...))`��


* `htlsvar`

    `htlsvar`��TLS���̱߳��ش洢����ʶ����MSVC��Ϊ`__declspec(thread)`����������������Ϊ`__thread`����ʹ��`htlsvar`�Ĵ��벻�ܴ���OpenCL�������У��ο�`hostonly`����

* `lprintf`

    `lprintf`���ڱ�׼����ϴ�ӡ��־��¼���������ԡ���OpenCL�лᱻ���ԡ�

    ����ʱ��iterations����Ϊ1������׼����������ļ�������`kernel.exe > log.txt`��

    *��*

        lprintf( "deal damage %d", dmg );
        lprintf( "another log" );
        /*
            05:36.420 deal damage 1635
            05:36.420 another log
        */


ʱ���
----

* `time_t`

    �ڿ���У�ʱ������`time_t`������Ϊ`k16u`��`rti->timestamp`����ģ��ĵ�ǰʱ�����

    ʱ��ֱ��ʱ�����Ϊ10���룬����`(time_t)1`�ȼ���0:00.010��`(time_t)65535`�ȼ���10:55.350��

    ���`time_t`���ڱ�ﳬ��10:55.350��ʱ���������Ϊδ���塣���`time_t`���ڱ������0:00.000��ʱ���������Ϊδ���塣

    ������Ҫ��Ե�λת������������뷽��ȶ������⣬**ֱ�Ӷ�`time_t`���ͽ�����������ǲ����ᳫ��**��Ӧ����ʹ�ÿ���ṩ�ĸ��ֲ���������


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

    ��`FROM_SECONDS`���ƣ��ɺ���ת��Ϊ`time_t`���μ�`FROM_SECONDS( sec )`��


* `TO_SECONDS( timestamp )`

    ��`timestamp`��`time_t`ת��Ϊ�롣��������Ϊ`float`�����`timestamp`����`time_t`���ͣ���ᱻǿ��ת��Ϊ`k16u`���ͣ������ܱ�֤������`time_t`�൱��

    ����������׼ȷʹ��`float`��ʾ��`TO_SECONDS`����������ȡ���������������ȡ�����������������ʵ����Ӧ�ò��ᷢ����

    *��*

        TO_SECONDS( 601 ) /* 6.01f */


* `TIME_OFFSET( delta )`

    �Ե�ǰʱ��`rti->timestamp`Ϊ��׼����δ������ƫ�Ƴ���Ϊ`delta`��ʱ�䡣����ֵ��`delta`��Ϊ`time_t`���͡����`delta`����`time_t`���ͣ���ᱻǿ��ת��Ϊ`k32s`���ͣ������ܱ�֤������`time_t`�൱��

    ������������`time_t`�ı�ʾ��Χ�����õ�����ֵ��

    `TIME_OFFSET`ֻ������δ������ƫ�ƣ��κγ������ȥ�������ƫ�Ƶ���Ϊ����δ����ġ�

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

    **�����ʱ���뵱ǰʱ�����ʱ�����ؼ�**��

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

> ��������**����**����Ч���ĳ���ʱ�䱻�ӳ�����Ϊ���ӵ�`(time_t)1`��ԭ��ʱ�䣬��ɵ�Ӱ��ֻ���ڱ߽紦��ȡֵ�ɼٸ�Ϊ�棬�������κ�ʱ�̵�ȡֵ��û�з����仯��


�����������
----

���������MT127��Ϊ�������������MT127����(2^127 - 1)�����ڣ�4ά���ȷֲ��������ȷֲ����⣬��ܻ��ṩ�˻���Box-Muller�任�ı�׼��̬�ֲ���

* `rng_init`

    ��ʼ������������������������������ʱ������Զ����á�


* `uni_rng`

    ����һ�����ȷֲ���[.0f, 1.0f)֮��ĵ����ȸ����������ԭ������

        float uni_rng(
            rtinfo_t* rti
        );

    rtiԭ�����ݼ��ɡ�


* `stdnor_rng`

    ����һ����׼��̬�ֲ���mean = 0, dev = 1���ĵ����ȸ����������ԭ������

        float stdnor_rng(
            rtinfo_t* rti
        );

    rtiԭ�����ݼ��ɡ�

    ���ڼĴ����ֳ����ƣ�����������ֲܷ�������ʵ�����ڣ����ܲ������������ΧԼΪ[-5.64666, 5.64666]��


�¼����У�EQ��
----

* `EQ_SIZE_EXP`��`EQ_SIZE`

    ��ʵ���ϣ��¼�������һ���̶��ߴ����С�ѡ�`EQ_SIZE_EXP`ָ���˶ѵĲ�����`EQ_SIZE`���Ƕѿ����ɵ��¼�������


* `_event_t`

    `_event_t`�¼����ͣ�**ע��ǰ׺���»���**��������������Ա

        time_t time; /* �¼�������ʱ�� */
        k8u routine; /* �¼��Ĵ�������� */
        k8u snapshot; /* �¼�����Я���Ŀ��� */

    time���¼�������ʱ�䡣EQϵͳ�����ִ�д���ʱ��������¼���ֱ��ִ�е�`EVENT_END_SIMULATION`���չ���

    �¼�ִ��ʱ��������routine��ֵ����������һ���¼�����������������ģ�鶨�壬��255������Ϊ`EVENT_END_SIMULATION`����ʾģ�������

    snapshot�μ�����ϵͳ��


* `eq_enqueue`

    ����һ�����¼�����EQ��ִ�С�ԭ������

        _event_t* eq_enqueue(
            rtinfo_t* rti,
            time_t trigger,
            k8u routine,
            k8u snapshot
        );

    `rti`ԭ�����ݣ�`trigger`��`routine`��`snapshot`��`_event_t`��������Ա������ɹ�������ֵ��ָ�����¼���ָ�룻����Ϊ0��

    ����ͨ�����ص�ָ����¼������޸ģ����������ڵ�ǰ�¼��Ĵ��������ʹ�á����Я���������¼��Ĵ��������ʹ�ã��ᷢ������Ԥ�ڵ���Ϊ��

    ��ʱ�����������δ��������ʱ����δ��ע���¼����ܻᵼ�´��¼��Ĵ���ʱ�����硣`eq_enqueue`������ʱ�������ȡ�򵥵��������ԣ�������0��

    ע���ڹ�ȥʱ�̵��¼��ᱻ���������������

        rti->timestamp = 65000; /* ��δ���磬ʣ��5.35�� */
        
        eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 6 ) ), rout, snap );
        /* TIME_OFFSET�Ὣ������б��ʹ����¼���ע�ᵽ(time_t)65535������(time_t)64���� */
        
        eq_enqueue( rti, rti->timestamp + FROM_SECONDS( 6 ), rout, snap );
        /* ֱ�Ӳ���ʱ��������Σ�յģ�������(time_t)64��ע���¼���eq_enqueue����Դ���Ϊ������0��ʾʧ�ܡ� */

    ��Ȼֱ�Ӳ���ʱ������ʱ������������������`eq_enqueue`�ܹ�ʶ�����һ����������Ժ��ԣ�ʹ�������ִ���ʽ��������������

    ��OpenCL�豸��ִ��ʱ������Ч�ʿ��ǣ�`eq_enqueue`��������Ƿ�����������������������¼��ᵼ�²���Ԥ�ڵ���Ϊ��ʵ��ģ��ʱ����Ҫ��CPU��Ԥ�Ȳ��Զѿռ��Ƿ���㡣


* `eq_enqueue_ps`

    ����һ�����������㡱�¼���ԭ������

        void eq_enqueue_ps(
            rtinfo_t* rti,
            time_t trigger
        );

    `rti`ԭ�����ݣ�`trigger`�Ǵ�����ʱ�̡�

    ģ�����������޷�����ĳ�������޷�ִ��ĳ����ʱ�������ݵ�ǰ�������ָ��ٶȹ��㡰�������㡱��ʱ�̣�����¼һ�������¼���ʹ�á��������㡱ʱ�̽���һ��������APLɨ�衣

    ������Ҫ��������¼�����APLɨ������������ʹ��`eq_enqueue_ps`�����������㡱�¼��������������㡱�¼���һ�����¼������Ҳ������������¼����С�

    һ������ʱ�̽���ġ��������㡱�¼��Ḳд����ʱ�̽���ġ��������㡱�¼����������͵��¼��ǲ��ȶ��ģ�Ӧ�þ����ܹ���APLɨ������еõ��ؽ������������򱻸�д����ܻᶪʧ��Ϣ��

    `eq_enqueue_ps`�ܷ��ֲ�������������Ͱ����ڹ�ȥʱ�̵��¼���

    *��*

        /* ��APL���������� */
        eq_enqueue_ps( rti, FROM_SECONDS( 50 ) ); /* �ڵ�50�봦�����ж���Ҫִ�У�����ɨ��APL�� */


* `eq_execute`

    ִ���¼�������������������¼������õ�ǰʱ�䣬���������Զ��ָ���

    ���뿪ĳ��ʱ��֮ǰ������������APLɨ�衣

    ���ʱ��仯����1�룬��������ǿ�ư���һ��APLɨ�衣

    �˺���ֻ���ɿ�ܵ��ã���ְҵģ���е��ô˺����ᵼ�±���ʧ�ܻ����޵ݹ顣


* `eq_delete`

    ���¼�������ɾ��һ���¼���ԭ������

        void eq_delete(
            rtinfo_t* rti,
            time_t time,
            k8u routnum
        );

    ��EQ�в��Ҵ�����ʱ��`time`���¼����������Ϊ`routnum`���¼�������ҵ�����ɾ������������ԡ�

    ��EQ���ж���¼������������ʱ��һ��`eq_delete`ֻ��ɾ������һ����������ȷ��ɾ��������һ����

    `eq_delete`��ʱ�临�Ӷ�ΪO(n)�����Գ������ܿ��ǣ�Ӧ�þ����ܱ���ʹ������


����ϵͳ
----

* `snapshot_t`

    ������Ҫ�洢����Ϣ��ְҵģ��ȷ����`snapshot_t`�ṹ��ĳ�Ա�б���ְҵģ��Դ�ļ��ڣ���`LUACL_LOAD_SNAPSHOT_T_MEMBERLIST`��Ϊ��־����������


* `SNAPSHOT_SIZE`

    ָ���˿���ϵͳ���������ɵĿ���������


* `snapshot_init`

    ��ʼ������ϵͳ��������ʱ�ɿ���Զ����á�


* `snapshot_alloc`

    �����ϵͳ����һ�����ա�ԭ������

        k8u snapshot_alloc(
            rtinfo_t* rti,
            snapshot_t** snapshot
        );

    rtiԭ�����ݡ�������snapshot��ָ��`snapshot_t*`����Ϊָ����ÿ��տռ��ָ�룬Ȼ�󷵻ش˿��յı�š�

    ���ձ�ű����ڶ�ȡ�����ٿ��գ�Ҳ��`_event_t`�ĳ�Ա֮һ��

    *��*

        k8u no;
        snapshot_t* pss;
        
        no = snapshot_alloc( rti, &pss ); /* ������� */
        pss->fieldA = foo;  /* ��д���� */
        pss->fieldB = bar;
        eq_enqueue( rti, TIME_OFFSET( 300 ), rout, no ); /* �����մ��ݸ��¼�rout */

    ��OpenCL����ʱ���������ϵͳ�Ƿ��������������ϵͳ����������ʹ��`snapshot_alloc`�ᵼ�²���Ԥ�ڵ���Ϊ��ģ��ʵ��ʱӦ��CPU�ϲ��Կ���ϵͳ�ռ��Ƿ���㡣


* `snapshot_read`

    ��ȡһ�����ա�ԭ������

        snapshot_t* snapshot_read(
            rtinfo_t* rti,
            k8u no
        );

    rtiԭ�����ݡ�noָ����һ�����ձ�ţ���������ָ��ÿ��յ�ָ�롣


* `snapshot_kill`

    ��ȡһ�����գ���������ռ�еĿռ��ͷš�ԭ������

        snapshot_t* snapshot_kill(
            rtinfo_t* rti,
            k8u no
        );

    rtiԭ�����ݡ�noָ����һ�����ձ�ţ���������ָ��ÿ��յ�ָ�롣
    

------
###### �����̵ļ�����ȴʱ��
��DoTʽ��ȴ����
����ĳ����A����ȴ��60�룬��ÿ��ʹ��B���ܣ�A������ȴ����8�룺

    // A�����ͷţ�
    rti->player.spell_A.cd = TIME_OFFSET( FROM_SECONDS( 60 );
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
    if ( UP( spell_A.cd ) ){ /* ���spell_A����CD�У������8���ܻᵼ�����硣Ӧ���⡣ */
        rti->player.spell_A.cd -= FROM_SECONDS( 8 );
        if ( !UP( spell_A.cd ) )
            eq_enqueue( rti, rti->timestamp, routnum_spell_A_cd, 0 );
    }

###### ��ˢ�µļ�����ȴʱ��
ֱ��ʹ�öѲ��ҽ����¼�ɾ����

###### ���������ֿ�ˢ�µļ��ܣ�
ʹ��DoTʽ��ȴ����

