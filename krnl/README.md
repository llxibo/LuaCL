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

    `K64S_C` 为一个数字常量添加后缀，使其成为`k64s`类型。

    `K64U_C` 为一个数字常量添加后缀，使其成为`k64u`类型。

    `K64U_MSB`为常量`0x1000000000000000`


* C标准库

    框架允许有限地使用C标准库，但相应代码不能带入OpenCL编译器中（参考`hostonly()`）。可以使用的函数包括`printf`、`abort`、`malloc`和`free`。

    支持断言`assert`，在OpenCL编译时，所有`assert`语句会被忽略。


* OpenCL特性在C语言中的拟合

    在C语言中，使用`set_global_id(dim, idx)`设置当前时刻当前线程关于某一维度`dim`的索引。`get_global_id`将得到最后一次`set_global_id`所设置的值。

    

    OpenCL C中提供了丰富的类型转换函数，框架在C语言上实现了`convert_ushort_sat`、`convert_ushort_rtp`和`convert_float_rtp`。

    `convert_ushort_sat`和`convert_ushort_rtp`要求输入表达式不能含有任何边界效应，`convert_float_rtp`要求操作数可以首先安全转型至`k64u`类型。

    

    在C语言上实现了`clz`、`cos`、`cospi`、`sqrt`、`log`、`clamp`、`max`、`min`、`mix`。

    其中，`clz`首先将操作数转型为`k64u`类型；`cos`、`cospi`、`sqrt`、`log`、`clamp`首先将操作数转型为`double`类型；`max`、`min`、`mix`要求输入表达式不能含有任何边界效应。


* `hostonly()`、`deviceonly()`和`msvconly()`

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

* `KRNL_STR()`

    包含在`KRNL_STR()`括号中的代码将被转换为字符串常量，但使用`KRNL_STR`的代码不能带入OpenCL编译器中（参考`hostonly()`）。

    *例*

        k32u abc = 4262;
        printf("%s = %d", KRNL_STR(abc), abc);

    将得到输出`abc = 4262`。


* `kdeclspec()`和`hdeclspec()`

    `kdeclspec()`指定展示给OpenCL编译器的特性限定符，相当于`__attribute__((...))`；`hdeclspec()`指定展示给普通C语言编译器的特性限定符，在MSVC中为`__declspec(...)`，在其他编译器中为`__attribute__((...))`。


* `htlsvar`

    `htlsvar`是TLS（线程本地存储）标识，在MSVC中为`__declspec(thread)`，在其他编译器中为`__thread`。但使用`htlsvar`的代码不能带入OpenCL编译器中（参考`hostonly()`）。


时间戳
----

* `time_t`

    在框架中，时间类型`time_t`被定义为`k16u`。`rti->timestamp`保存模拟的当前时间戳。

    时间分辨率被定义为10毫秒，所以`(time_t)1`等价于0:00.010，`(time_t)65535`等价于10:55.350。

    如果`time_t`用于表达超过10:55.350的时间戳，其行为未定义。如果`time_t`用于表达早于0:00.000的时间戳，其行为未定义。

    由于需要面对单位转换、溢出、舍入方向等多种问题，直接对`time_t`类型进行运算操作是不予提倡的，应优先使用框架提供的各种操作方法。


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

    与`FROM_SECONDS`相似，由毫秒转换为`time_t`。


* `TO_SECONDS( timestamp )`

    将`timestamp`由`time_t`转换为秒。返回类型为`float`。如果`timestamp`不是`time_t`类型，则会被强制转型为`k16u`类型，但不能保证量纲与`time_t`相当。

    如果结果不能准确使用`float`表示，`TO_SECONDS`会向正无穷取整，而不是向最近取整。但是这种情况在实践中应该不会发生。

    *例*

        TO_SECONDS( 601 ) /* 6.01f */


* `TIME_OFFSET( delta )`

    以当前时刻`rti->timestamp`为基准，向未来方向偏移长度为`delta`的时间。返回值和`delta`均为`time_t`类型。如果`delta`不是`time_t`类型，则会被强制转型为`k32s`类型，但不能保证量纲与`time_t`相当。

    如果结果超过了`time_t`的表示范围，则会得到饱和值。

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

    _所检查时刻与当前时刻相等时，返回假_。

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

> 这样处理_不会_导致效果的持续时间被延长，因为增加的`(time_t)1`是原子时间，造成的影响只有在边界处的取值由假改为真，而其他任何时刻的取值都没有发生变化。


------
###### 可缩短的技能冷却时间
用DoT式冷却处理。
例如某技能A，冷却是60秒，但每次使用B技能，A技能冷却减少8秒：

    // A技能释放：
    rti->player.spell_A.cd = 60;
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
    rti->player.spell_A.cd -= 8;
    if ( !UP( spell_A.cd ) )
        eq_enqueue( rti, rti->timestamp, routnum_spell_A_cd, 0 );

###### 可刷新的技能冷却时间
直接使用堆查找将老事件删除。使用旧的冷却位置信息作为提示，可以将堆查找删除复杂度降低许多。

###### 即可缩短又可刷新的技能？
使用DoT式冷却处理。

