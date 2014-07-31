基础设施
----

* 归一化的类型

    OpenCL C对C语言的类型系统做出了改进，但引入了一些分歧，所以框架提供了归一化的类型。

    `kbool` 布尔类型

    `k8s` 有符号char( 在C语言中对应 `signed char` 而非 `char` )

    `k8u` 无符号char

    `k16s` 有符号short

    `k16u` 无符号short

    `k32s` 有符号long（对应OpenCL C中的`int`）

    `k32u` 无符号long（对应OpenCL C中的`uint`）

    `k64s` 有符号long long（对应OpenCL C中的`long`）

    `k64u` 无符号long long（对应OpenCL C中的`ulong`）

    浮点类型使用源生。

    `K64S_C( num )` 为一个数字常量添加后缀，使其成为`k64s`类型。

    `K64U_C( num )` 为一个数字常量添加后缀，使其成为`k64u`类型。

    `K64U_MSB`为常量`0x1000000000000000`


* C标准库

    框架允许有限地使用C标准库，但相应代码不能带入OpenCL编译器中（参考`hostonly`）。可以使用的函数包括`printf`、`abort`、`malloc`和`free`。

    支持断言`assert`，在OpenCL编译时，所有`assert`语句会被忽略。


* OpenCL特性在C语言中的拟合

    在C语言中，使用`set_global_id(dim, idx)`设置当前时刻当前线程关于某一维度`dim`的索引。`get_global_id`将得到最后一次`set_global_id`所设置的值。

    

    OpenCL C中提供了丰富的类型转换函数，框架在C语言上实现了`convert_ushort_sat`、`convert_ushort_rtp`和`convert_float_rtp`。

    `convert_ushort_sat`和`convert_ushort_rtp`要求输入表达式不能含有任何边界效应，`convert_float_rtp`要求操作数可以首先安全转型至`k64u`类型。

    

    在C语言上实现了`clz`、`cos`、`cospi`、`sqrt`、`log`、`clamp`、`max`、`min`、`mix`。

    其中，`clz`首先将操作数转型为`k64u`类型；`cos`、`cospi`、`sqrt`、`log`、`clamp`首先将操作数转型为`double`类型；`max`、`min`、`mix`要求输入表达式不能含有任何边界效应。


* `hostonly`、`deviceonly`和`msvconly`

    包含在`hostonly()`括号中的代码在OpenCL编译时将被忽略；相反地，包含在`deviceonly()`括号中的代码在非OpenCL编译时将被忽略。包含在`msvconly()`括号中的代码在除MSVC以外的编译器编译时将被忽略。

    *例*

        struct {
            struct {
                k32u a;
                k32u b;
            } c;
            k32u d;
        } foo = {
            msvconly( 0 )
        };

    在MSVC中，以一对空的大括号`{}`作为结构体初值时，会得到编译错误，需要添加一个`0`。在GCC中，大括号中添加的`0`与`c`的类型不符，会得到编译警告。使用`msvconly( 0 )`将此处的零对GCC隐藏，使得代码在两种环境中均可正常工作。

* `KRNL_STR`

    包含在`KRNL_STR()`括号中的代码将被转换为字符串常量，但使用`KRNL_STR`的代码不能带入OpenCL编译器中（参考`hostonly`）。

    *例*

        k32u abc = 4262;
        printf("%s = %d", KRNL_STR(abc), abc);

    将得到输出`abc = 4262`。


* `kdeclspec`和`hdeclspec`

    `kdeclspec(...)`指定展示给OpenCL编译器的特性限定符，相当于`__attribute__((...))`；`hdeclspec(...)`指定展示给普通C语言编译器的特性限定符，在MSVC中为`__declspec(...)`，在其他编译器中为`__attribute__((...))`。


* `htlsvar`

    `htlsvar`是TLS（线程本地存储）标识，在MSVC中为`__declspec(thread)`，在其他编译器中为`__thread`。但使用`htlsvar`的代码不能带入OpenCL编译器中（参考`hostonly`）。

* `lprintf`

    `lprintf`会在标准输出上打印日志记录，帮助调试。在OpenCL中会被忽略。

    调试时将iterations设置为1，将标准输出导出到文件，例如`kernel.exe > log.txt`。

    *例*

        lprintf( "deal damage %d", dmg );
        lprintf( "another log" );
        /*
            05:36.420 deal damage 1635
            05:36.420 another log
        */


时间戳
----

* `time_t`

    在框架中，时间类型`time_t`被定义为`k16u`。`rti->timestamp`保存模拟的当前时间戳。

    时间分辨率被定义为10毫秒，所以`(time_t)1`等价于0:00.010，`(time_t)65535`等价于10:55.350。

    如果`time_t`用于表达超过10:55.350的时间戳，其行为未定义。如果`time_t`用于表达早于0:00.000的时间戳，其行为未定义。

    由于需要面对单位转换、溢出、舍入方向等多种问题，**直接对`time_t`类型进行运算操作是不予提倡的**，应优先使用框架提供的各种操作方法。


* `FROM_SECONDS( sec )`

    将`sec`由秒转换为`time_t`。对`sec`的类型没有要求，但当`sec`为常数或`float`类型变量时，其效率最高，因为在其内部实现中，`sec`会首先转型为`float`。

    当所给时间超过`time_t`的表示范围时，其行为未定义。

    当所给时间不能准确用`time_t`表示时，`FROM_SECONDS`会向正无穷取整而不是向最近取整。

    *例*

        FROM_SECONDS( 6 )     /* (time_t)600 */
        FROM_SECONDS( 6.01 )  /* (time_t)601 */
        FROM_SECONDS( 6.001 ) /* (time_t)601 */
        FROM_SECONDS( 6.000001 ) /* (time_t)601 */
        FROM_SECONDS( 6.0000000000001 ) /* (time_t)600 */


* `FROM_MILLISECONDS( msec )`

    与`FROM_SECONDS`相似，由毫秒转换为`time_t`。参见`FROM_SECONDS( sec )`。


* `TO_SECONDS( timestamp )`

    将`timestamp`由`time_t`转换为秒。返回类型为`float`。如果`timestamp`不是`time_t`类型，则会被强制转型为`k16u`类型，但不能保证量纲与`time_t`相当。

    如果结果不能准确使用`float`表示，`TO_SECONDS`会向正无穷取整，而不是向最近取整。但是这种情况在实践中应该不会发生。

    *例*

        TO_SECONDS( 601 ) /* 6.01f */


* `TIME_OFFSET( delta )`

    以当前时刻`rti->timestamp`为基准，向未来方向偏移长度为`delta`的时间。返回值和`delta`均为`time_t`类型。如果`delta`不是`time_t`类型，则会被强制转型为`k32s`类型，但不能保证量纲与`time_t`相当。

    如果结果超过了`time_t`的表示范围，则会得到饱和值。

    `TIME_OFFSET`只允许向未来方向偏移，任何尝试向过去方向进行偏移的行为都是未定义的。

    *例*

        rti->timestamp = FROM_SECONDS( 8 );
        TIME_OFFSET( FROM_SECONDS( 6 ) ) /* 正确，(time_t)1400 */
        TIME_OFFSET( FROM_SECONDS( -6 ) ) /* 未定义行为 */
        TIME_OFFSET( -FROM_SECONDS( 6 ) ) /* 未定义行为 */
        
        rti->timestamp = FROM_SECONDS( 655 );
        TIME_OFFSET( FROM_SECONDS( 6 ) ) /* 溢出上界，得到(time_t)65535 */


* `TIME_DISTANT( time )`

    以当前时刻`rti->timestamp`为基准，计算到达`time`的时间差。返回值和`time`均为`time_t`类型。如果`time`不是`time_t`类型，则会被强制转型为`k32s`类型，但不能保证量纲与`time_t`相当。

    如果结果超过了`time_t`的表示范围，则会得到饱和值。

    *例*

        rti->timestamp = FROM_SECONDS( 8 );
        TIME_DISTANT( FROM_SECONDS( 10 ) ) /* 正确，(time_t)200 */
        TIME_DISTANT( FROM_SECONDS( 6 ) ) /* 溢出下界，得到(time_t)0 */


* `UP( time_to_check )`

    缩写宏。从`rti->player`结构中抽取名为`time_to_check`的成员，并检查其表示的时刻是否在当前时刻之后，是则返回真，否则返回假。

    **所检查时刻与当前时刻相等时，返回假**。

    *例*

        rti->timestamp = FROM_SECONDS( 8 );
        rti->player.aura_foo.expire = FROM_SECONDS( 10 );
        UP( aura_foo.expire ) /* 返回真 */
        rti->player.spell_bar.cd = FROM_SECONDS( 8 );
        UP( spell_bar.cd ) /* 返回假 */


* `REMAIN( time_to_check )`

    缩写宏。从`rti->player`结构中抽取名为`time_to_check`的成员，并求其`TIME_DISTANT`。

    *例*

        rti->timestamp = FROM_SECONDS( 8 );
        rti->player.aura_foo.expire = FROM_SECONDS( 10 );
        REMAIN( aura_foo.expire ) /* (time_t)200 */


##### 时间和时间边界

> 处理时间时，还需要特别注意发生真-假跃变的时刻。例如冷却结束的时刻，效果结束的时刻，等等。

> 一般地，在冷却结束的瞬间，应判定为允许施放法术，即`UP(spell.cd)`为假。否则在冷却结束时刻挂钩的事件将无法执行任何动作，导致模拟不能继续推进。

> 这是宏`UP`边界处理方式的内部原因。而一些效果的结束时刻，可能正相反，需要判定为真。

> 此时可以大胆地使用hack的方式，将效果持续时间增加`(time_t)1`，使它们的`UP`条件在边界处取得真值。

> 这样处理**不会**导致效果的持续时间被延长，因为增加的`(time_t)1`是原子时间，造成的影响只有在边界处的取值由假改为真，而其他任何时刻的取值都没有发生变化。


随机数发生器
----

框架内置了MT127作为随机数发生器。MT127具有(2^127 - 1)的周期，4维均匀分布。除均匀分布以外，框架还提供了基于Box-Muller变换的标准正态分布。

* `rng_init`

    初始化随机数发生器。这个函数会在启动时被框架自动调用。


* `uni_rng`

    生成一个均匀分布在[.0f, 1.0f)之间的单精度浮点随机数。原型如下

        float uni_rng(
            rtinfo_t* rti
        );

    rti原样传递即可。


* `stdnor_rng`

    生成一个标准正态分布（mean = 0, dev = 1）的单精度浮点随机数。原型如下

        float stdnor_rng(
            rtinfo_t* rti
        );

    rti原样传递即可。

    由于寄存器字长限制，此随机数不能分布在整个实数域内，可能产生的随机数范围约为[-5.64666, 5.64666]。


事件队列（EQ）
----

* `EQ_SIZE_EXP`和`EQ_SIZE`

    在实现上，事件队列是一个固定尺寸的最小堆。`EQ_SIZE_EXP`指明了堆的层数，`EQ_SIZE`则是堆可容纳的事件数量。


* `_event_t`

    `_event_t`事件类型（**注意前缀的下划线**），包含三个成员

        time_t time; /* 事件发生的时刻 */
        k8u routine; /* 事件的处理程序编号 */
        k8u snapshot; /* 事件所需携带的快照 */

    time是事件触发的时间。EQ系统会迭代执行触发时间最早的事件，直至执行到`EVENT_END_SIMULATION`后收工。

    事件执行时，会依据routine的值决定进入哪一个事件处理函数。具体编号由模块定义，但255被定义为`EVENT_END_SIMULATION`，表示模拟结束。

    snapshot参见快照系统。


* `eq_enqueue`

    创建一个新事件插入EQ待执行。原型如下

        _event_t* eq_enqueue(
            rtinfo_t* rti,
            time_t trigger,
            k8u routine,
            k8u snapshot
        );

    `rti`原样传递，`trigger`、`routine`、`snapshot`即`_event_t`的三个成员。如果成功，返回值是指向新事件的指针；否则为0。

    允许通过返回的指针对事件进行修改，但仅限于在当前事件的处理过程内使用。如果携带入其他事件的处理过程内使用，会发生不可预期的行为。

    当时间戳即将但仍未发生上溢时，向未来注册事件可能会导致此事件的触发时间上溢。`eq_enqueue`对所有时间上溢采取简单的抛弃策略，并返回0。

    注册在过去时刻的事件会被当作上溢而抛弃。

        rti->timestamp = 65000; /* 还未上溢，剩余5.35秒 */
        
        eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 6 ) ), rout, snap );
        /* TIME_OFFSET会将上溢进行饱和处理，事件被注册到(time_t)65535处而非(time_t)64处。 */
        
        eq_enqueue( rti, rti->timestamp + FROM_SECONDS( 6 ), rout, snap );
        /* 直接操作时间类型是危险的，尝试在(time_t)64处注册事件，eq_enqueue会忽略此行为，返回0表示失败。 */

    虽然直接操作时间类型时的疏忽导致了溢出，但`eq_enqueue`能够识别出后一种情况并予以忽略，使上述两种处理方式均能正常工作。

    在OpenCL设备上执行时，出于效率考虑，`eq_enqueue`不会检查堆是否已满。如果堆已满，插入事件会导致不可预期的行为，实现模块时，需要在CPU上预先测试堆空间是否充足。


* `eq_enqueue_ps`

    插入一个“能量充足”事件。原型如下

        void eq_enqueue_ps(
            rtinfo_t* rti,
            time_t trigger
        );

    `rti`原样传递，`trigger`是触发的时刻。

    模拟遇到能量无法满足某条件而无法执行某动作时，会依据当前的能量恢复速度估算“能量充足”的时刻，并记录一个傀儡事件，使得“能量充足”时刻进行一次完整的APL扫描。

    所有需要插入傀儡事件触发APL扫描的情况都可以使用`eq_enqueue_ps`按“能量充足”事件处理。“能量充足”事件是一个空事件，而且不会真正插入事件堆中。

    一个触发时刻较早的“能量充足”事件会覆写触发时刻较晚的“能量充足”事件。这种类型的事件是不稳定的，应该具有能够在APL扫描过程中得到重建的能力，否则被覆写后可能会丢失信息。

    `eq_enqueue_ps`能发现并抛弃上溢情况和安排在过去时刻的事件。

    *例*

        /* 在APL处理例程中 */
        eq_enqueue_ps( rti, FROM_SECONDS( 50 ) ); /* 在第50秒处可能有动作要执行，安排扫描APL。 */


* `eq_execute`

    执行事件队列中排列在最早的事件，设置当前时间，处理能量自动恢复。

    在离开某个时刻之前，触发完整的APL扫描。

    如果时间变化超过1秒，则插入检查点强制安排一次APL扫描。

    此函数只能由框架调用，在职业模块中调用此函数会导致编译失败或无限递归。


* `eq_delete`

    从事件队列中删除一个事件。原型如下

        void eq_delete(
            rtinfo_t* rti,
            time_t time,
            k8u routnum
        );

    在EQ中查找触发在时刻`time`、事件处理程序编号为`routnum`的事件。如果找到，则删除它；否则忽略。

    当EQ中有多个事件均满足此条件时，一次`eq_delete`只会删除其中一个，但不能确保删除的是哪一个。

    `eq_delete`的时间复杂度为O(n)，所以出于性能考虑，应该尽可能避免使用它。


快照系统
----

* `snapshot_t`

    快照需要存储的信息由职业模块确定。`snapshot_t`结构体的成员列表在职业模块源文件内，由`LUACL_LOAD_SNAPSHOT_T_MEMBERLIST`作为标志包含进来。


* `SNAPSHOT_SIZE`

    指明了快照系统最大可以容纳的快照数量。


* `snapshot_init`

    初始化快照系统，在启动时由框架自动调用。


* `snapshot_alloc`

    向快照系统申请一个快照。原型如下

        k8u snapshot_alloc(
            rtinfo_t* rti,
            snapshot_t** snapshot
        );

    rti原样传递。函数将snapshot所指的`snapshot_t*`设置为指向可用快照空间的指针，然后返回此快照的编号。

    快照编号被用于读取和销毁快照，也是`_event_t`的成员之一。

    *例*

        k8u no;
        snapshot_t* pss;
        
        no = snapshot_alloc( rti, &pss ); /* 申请快照 */
        pss->fieldA = foo;  /* 填写快照 */
        pss->fieldB = bar;
        eq_enqueue( rti, TIME_OFFSET( 300 ), rout, no ); /* 将快照传递给事件rout */

    在OpenCL运行时不会检查快照系统是否已满。如果快照系统已满，尝试使用`snapshot_alloc`会导致不可预期的行为。模块实现时应在CPU上测试快照系统空间是否充足。


* `snapshot_read`

    读取一个快照。原型如下

        snapshot_t* snapshot_read(
            rtinfo_t* rti,
            k8u no
        );

    rti原样传递。no指定了一个快照编号，函数返回指向该快照的指针。


* `snapshot_kill`

    读取一个快照，并将其所占有的空间释放。原型如下

        snapshot_t* snapshot_kill(
            rtinfo_t* rti,
            k8u no
        );

    rti原样传递。no指定了一个快照编号，函数返回指向该快照的指针。
    

------
###### 可缩短的技能冷却时间
用DoT式冷却处理。
例如某技能A，冷却是60秒，但每次使用B技能，A技能冷却减少8秒：

    // A技能释放：
    rti->player.spell_A.cd = TIME_OFFSET( FROM_SECONDS( 60 );
    eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 5 ) ), routnum_spell_A_cd, 0 );
        
    // spell_A_cd事件：
    if ( TIME_DISTANT( rti->player.spell_A.cd ) > FROM_SECONDS( 5 ) ) {
        // 实际冷却远未结束，还需要5秒后再次检查
        eq_enqueue( rti, TIME_OFFSET( FROM_SECONDS( 5 ) ), routnum_spell_A_cd, 0 );
    } else if ( rti->player.spell_A.cd > rti->timestamp ) {
        // 实际冷却即将结束，在结束处触发
        eq_enqueue( rti, rti->player.spell_A.cd, routnum_spell_A_cd, 0 );
    } else if ( rti->player.spell_A.cd == rti->timestamp ) {
        // 真正的冷却结束事件，如果有结束处触发的事件，写在这里
    } else {
        // 被废弃的冷却结束事件，不需要任何动作。
    }
        
    // B技能释放：
    if ( UP( spell_A.cd ) ){ /* 如果spell_A不在CD中，对其减8可能会导致下溢。应避免。 */
        rti->player.spell_A.cd -= FROM_SECONDS( 8 );
        if ( !UP( spell_A.cd ) )
            eq_enqueue( rti, rti->timestamp, routnum_spell_A_cd, 0 );
    }

###### 可刷新的技能冷却时间
直接使用堆查找将老事件删除。

###### 即可缩短又可刷新的技能？
使用DoT式冷却处理。

