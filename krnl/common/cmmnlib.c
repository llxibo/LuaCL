/**
    common library, prefixed to every kernel.
    2014.7.14
*/

/* Front-end gives these defines. */
#define vary_combat_length 20.0f
#define max_length 450.0f
#define initial_health_percentage 100.0f
#define death_pct 0.0f

/* Debug on CPU! */
#if !defined(__OPENCL_VERSION__)
#define _DEBUG
#pragma once
#endif /* !defined(__OPENCL_VERSION__) */

/* Codes enclosed in 'hostonly' will be discarded at OpenCL devices, and vice versa. */
#if defined(__OPENCL_VERSION__)
#define hostonly(...)
#define deviceonly(...) __VA_ARGS__
#else
#define hostonly(...) __VA_ARGS__
#define deviceonly(...)
#endif /* defined(__OPENCL_VERSION__) */

/* Diagnostic. */
#if defined(_DEBUG) && !defined(__OPENCL_VERSION__)
int printf( const char* format, ... );
void abort( void );
#define KRNL_STR2(v) #v
#define KRNL_STR(v) KRNL_STR2(v)
#define assert(expression) if(!(expression)){ \
        printf("Assertion failed: %s, function %s, file %s, line %d.\n", \
                KRNL_STR(expression),   __func__ ,__FILE__, __LINE__); \
                abort(); }else
#else
#define assert(...)
#endif /* defined(_DEBUG) && !defined(__OPENCL_VERSION__) */

/* Unified typename. */
#if defined(__OPENCL_VERSION__)
#define kbool bool
#define k8s char
#define k8u uchar
#define k16s short
#define k16u ushort
#define k32s int
#define k32u uint
#define k64s long
#define k64u ulong
#define K64S_C(num) (num##L)
#define K64U_C(num) (num##UL)
#else
#define kbool int
#define k8s signed char
#define k8u unsigned char
#define k16s short int
#define k16u unsigned short int
#define k32s long int
#define k32u unsigned long int
#define k64s long long int
#define k64u unsigned long long int
#define K64S_C(num) (num##LL)
#define K64U_C(num) (num##ULL)
#endif /* defined(__OPENCL_VERSION__) */

/* Unified compile hint. */
#if defined(__OPENCL_VERSION__)
#define kdeclspec(...) __attribute__((__VA_ARGS__))
#define hdeclspec(...)
#else
#define kdeclspec(...)
#if defined(_MSC_VER)
#define hdeclspec(...) __declspec(__VA_ARGS__)
#else
#define hdeclspec(...) __attribute__((__VA_ARGS__))
#endif /* defined(_MSC_VER) */
#endif /* defined(__OPENCL_VERSION__) */

/* get_global_id() on CPU. */
#if !defined(__OPENCL_VERSION__)
#if defined(_MSC_VER)
#define htlsvar __declspec(thread)
#else
#define htlsvar __thread
#endif /* defined(_MSC_VER) */
htlsvar int global_idx[3] = {0};
int get_global_id( int dim ) {
    return global_idx[dim];
}
void set_global_id( int dim, int idx ) {
    global_idx[dim] = idx;
}
#endif /* !defined(__OPENCL_VERSION__) */

/* clz() "Count leading zero" on CPU */
#if !defined(__OPENCL_VERSION__)
#if defined(_MSC_VER)
#if defined(_M_IA64) || defined(_M_X64)
unsigned char _BitScanReverse64( unsigned long* _Index, unsigned __int64 _Mask ); /* MSVC Intrinsic. */
/*
    _BitScanReverse64 set IDX to the position(from LSB) of the first bit set in mask.
    What we need is counting leading zero, thus return 64 - (IDX + 1).
*/
k8u clz( k64u mask ) {
    unsigned long IDX = 0;
    _BitScanReverse64( &IDX, mask );
    return 63 - IDX;
}
#elif defined(_M_IX86)
unsigned char _BitScanReverse( unsigned long* _Index, unsigned long _Mask ); /* MSVC Intrinsic. */
/*
    On 32-bit machine, _BitScanReverse only accept 32 bit number.
    So we need do some cascade.
*/
k8u clz( k64u mask ) {
    unsigned long IDX = 0;
    if ( mask >> 32 ) {
        _BitScanReverse( &IDX, mask >> 32 );
        return 31 - IDX;
    }
    _BitScanReverse( &IDX, mask & 0xFFFFFFFFULL );
    return 63 - IDX;
}
#else
/*
    This machine is not x86/amd64/Itanium Processor Family(IPF).
    the processor don't have a 'bsr' instruction so _BitScanReverse is not available.
    Need some bit manipulation...
*/
#define NEED_CLZ_BIT_TWIDDLING
#endif /* defined(_M_IA64) || defined(_M_X64) */
#elif defined(__GNUC__)
#define clz __builtin_clzll /* GCC have already done this. */
#else
/*
    Unkown compilers. We know nothing about what intrinsics they have,
    nor what their inline assembly format is.
*/
#define NEED_CLZ_BIT_TWIDDLING
#endif /* defined(_MSC_VER) */
#endif /* !defined(__OPENCL_VERSION__) */
#if defined(NEED_CLZ_BIT_TWIDDLING)
#undef NEED_CLZ_BIT_TWIDDLING
/*
    Tons of magic numbers... For how could this works, see:
        http://supertech.csail.mit.edu/papers/debruijn.pdf
    result for zero input is NOT same as MSVC intrinsic version, but still undefined.
*/
k8u clz( k64u mask ) {
    static k8u DeBruijn[64] = {
        /*        0   1   2   3   4   5   6   7 */
        /*  0 */  0, 63, 62, 11, 61, 57, 10, 37,
        /*  8 */ 60, 26, 23, 56, 30,  9, 16, 36,
        /* 16 */  2, 59, 25, 18, 20, 22, 42, 55,
        /* 24 */ 40, 29,  5,  8, 15, 46, 35, 53,
        /* 32 */  1, 12, 58, 38, 27, 24, 31, 17,
        /* 40 */  3, 19, 21, 43, 41,  6, 47, 54,
        /* 48 */ 13, 39, 28, 32,  4, 44,  7, 48,
        /* 56 */ 14, 33, 45, 49, 34, 50, 51, 52,
    };
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    v++;
    return DeBruijn[( v * 0x022fdd63cc95386dULL ) >> 58];
}
#endif /* defined(NEED_CLZ_BIT_TWIDDLING) */

/* Const Pi. */
#define M_PI 3.14159265358979323846

/* Math utilities on CPU. */
#if !defined(__OPENCL_VERSION__)
double cos(double x);
#define cospi(x) cos(x * M_PI)
double sqrt(double x);
double log(double x);
double clamp(double val, double min, double max){
    return val < min ? min : val > max ? max : val;
}
#define max(a, b) ((a) > (b) ? (a) : (b))
#define mix(x, y, a) ((x) + (( (y) - (x) ) * (a)))
#endif /* !defined(__OPENCL_VERSION__) */

/* Seed struct which holds the current state. */
typedef struct {
    k32u mt[4]; /* State words. */
    k16u mti;   /* State counter: must be within [0,3]. */
} seed_t;

/*
    Timestamp is defined as a 16-bit unsigned int.
    the atomic time unit is 10 milliseconds, thus the max representable timestamp is 10'55"350.
*/
typedef k16u time_t;
#define FROM_SECONDS( sec ) ((time_t)((float)(sec) * 100.0f))
#define FROM_MILLISECONDS( msec ) ((time_t)((float)(msec) / 10.0f))

/* Event queue. */
#define EQ_SIZE (63)
typedef struct {
    time_t time;
    k8u routine;
    k8u snapshot; /* Preserved. */
} event_t;
typedef struct {
    k16u count;
    k16u power_suffice;
    event_t event[EQ_SIZE];
} event_queue_t;

/* Snapshot saves past states. */
#define SNAPSHOT_SIZE (64)
typedef struct {
    k32u placeholder; /* ... */
} snapshot_t;
typedef struct {
    snapshot_t* buffer; /* Should be __global__ ! */
    k64u bitmap;
} snapshot_manager_t;

/* Runtime info struct, each thread preserves its own. */
typedef struct kdeclspec( packed ) {
    seed_t seed;
    time_t timestamp;
    event_queue_t eq;
    snapshot_manager_t snapshot_manager;
    time_t expected_combat_length;

} rtinfo_t;

/* Declaration Action Priority List (APL) */
void scan_apl( rtinfo_t* rti ); /* Implement is generated by front-end. */

/*
    Event routine entries. Each class module implement its own.
    Each type of event should be assigned to a routine number.
    Given routine number 'routine_entries' select the corresponding function to call.
*/
void routine_entries( rtinfo_t* rti, event_t e );

/* Initialize RNG */
/*** Behaviors of uninitialized RNG is undefined!!! ***/
void rng_init( seed_t* state, k32u seed ) {
    state->mti = 0; /* Reset counter */
    /* Use a LCG to fill state matrix. See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    state->mt[0] = seed & 0xffffffffUL;
    seed = ( 1812433253UL * ( seed ^ ( seed >> 30 ) ) ) + 1;
    state->mt[1] = seed & 0xffffffffUL;
    seed = ( 1812433253UL * ( seed ^ ( seed >> 30 ) ) ) + 2;
    state->mt[2] = seed & 0xffffffffUL;
    seed = ( 1812433253UL * ( seed ^ ( seed >> 30 ) ) ) + 3;
    state->mt[3] = seed & 0xffffffffUL;
}

/* Generate one IEEE-754 single precision float point uniformally distributed in the interval [.0f, 1.0f). */
float uni_rng( seed_t* seed ) {
    k32u y; /* Should be a register. */
    assert( seed->mti < 4 ); /* If not, RNG is uninitialized. */

    /* Concat lower-right and upper-left state bits. */
    y = ( seed->mt[seed->mti] & 0xfffffffeU ) | ( seed->mt[( seed->mti + 1 ) & 3] & 0x00000001U );
    /* Compute next state with the recurring equation. */
    y = seed->mt[seed->mti] = seed->mt[( seed->mti + 2 ) & 3] ^ ( y >> 1 ) ^ ( 0xfa375374U & -( y & 0x1U ) );
    /* Increase the counter */
    seed->mti = ( seed->mti + 1 ) & 3;
    /* Tempering */
    y ^= ( y >> 12 );
    y ^= ( y << 7 ) & 0x33555f00U;
    y ^= ( y << 15 ) & 0xf5f78000U;
    y ^= ( y >> 18 );
    /* Mask to IEEE-754 format [1.0f, 2.0f). */
    y = ( ( y & 0x3fffffffU ) | 0x3f800000U );
    return ( *( float* )&y ) - 1.0f; /* Decrease to [.0f, 1.0f). */
}

/* Generate one IEEE-754 single precision float point with standard normal distribution. */
float stdnor_rng( seed_t* seed ) {
    /*
        The max number generated by uni_rng() should equal to:
            as_float( 0x3fffffff ) - 1.0f => as_float( 0x3f7ffffe )
        Thus, we want transform the interval to (.0f, 1.0f], we should add:
            1.0f - as_float( 0x3f7ffffe ) => as_float( 0x34000000 )
        Which is representable by decimal 1.1920929E-7.
        With a minimal value 1.1920929E-7, the max vaule stdnor_rng could give is approximately 5.64666.
    */
    return sqrt( -2.0f * log( uni_rng( seed ) + 1.1920929E-7 ) ) * cospi( 2.0f * uni_rng( seed ) );
    /*
        To get another individual normally distributed number in pair, replace 'cospi' to 'sinpi'.
        It's simply thrown away here, because of diverage penalty.
        With only one thread in a warp need to recalculate, the whole warp must diverage.
    */
}

/* Enqueue an event into EQ. */
event_t* eq_enqueue( rtinfo_t* rti, time_t trigger, k8u routine, k8u snapshot ) {
    k32u i = ++( rti->eq.count );
    event_t* p = &( rti->eq.event[-1] );

    assert( rti->eq.count <= EQ_SIZE ); /* Full check. */

    /*
        There are two circumstances which could cause the assert below fail:
        1. Devs got something wrong in the class module, enqueued an event happens before 'now'.
        2. Time register is about to overflow, the triggering delay + current timestamp have exceeded the max representable time.
        Since the later circumstance is not a fault, we would just throw the event away and continue quietly.
        When you are exceeding the max time limits, all new events will be thrown, and finally you will get an empty EQ,
        then the empty checks on EQ will fail.
    */
    if ( rti->timestamp <= trigger ){
        for( ; i > 1 && p[i >> 1].time > trigger; i >>= 1 )
            p[i] = p[i >> 1];
        p[i] = ( event_t ) {
            .time = trigger, .routine = routine, .snapshot = snapshot
                                    };
        return &p[i];
    }
    return 0;
}

/* Enqueue a power suffice event into EQ. */
void eq_enqueue_ps( rtinfo_t* rti, time_t trigger ) {
    if ( !rti->eq.power_suffice || rti->eq.power_suffice > trigger )
        rti->eq.power_suffice = trigger;
}

/* Execute the top priority. */
void eq_execute( rtinfo_t* rti ) {
    k16u i, child;
    event_t min, last;
    event_t* p = &rti->eq.event[-1];

    assert( rti->eq.count ); /* Empty check. */
    assert( rti->eq.count <= EQ_SIZE ); /* Not zero but negative? */
    assert( rti->timestamp <= p[1].time ); /* Time won't go back. */

    /* When time elapse, trigger a full scanning at APL. */
    if ( rti->timestamp < p[1].time && ( !rti->eq.power_suffice || rti->timestamp < rti->eq.power_suffice ) )
        scan_apl( rti ); /* This may change p[1]. */

    min = p[1];

    if ( !rti->eq.power_suffice || rti->eq.power_suffice >= min.time ) {

        /* Delete from heap. */
        last = p[rti->eq.count--];
        for( i = 1; i << 1 <= rti->eq.count; i = child ) {
            child = i << 1;
            if ( child != rti->eq.count && rti->eq.event[child].time < p[child].time )
                child++;
            if ( last.time > p[child].time )
                p[i] = p[child];
            else
                break;
        }
        p[i] = last;

        /* Now 'min' contains the top priority. Execute it. */
        rti->timestamp = min.time;
        /* TODO: Some preparations? */
        routine_entries( rti, min );
        /* TODO: Some finishing works? */

    } else {
        /* Invoke power suffice routine. */
        rti->timestamp = rti->eq.power_suffice;
        rti->eq.power_suffice = 0;
        /* Power suffices would not make any impact, just a reserved APL scanning. */
    }

}

/*
    Assumed char is exactly 8 bit.
    Most std headers will pollute the namespace, thus we do not use CHAR_BIT,
    which is defined in <limits.h>.
*/
#define K64U_MSB ( K64U_C( 1 ) << (sizeof(k64u) * 8 - 1) )

k8u snapshot_save( rtinfo_t* rti, snapshot_t snapshot ) {
    k8u no;
    assert( rti->snapshot_manager.bitmap ); /* Full check. */
    no = clz( rti->snapshot_manager.bitmap ); /* Get first available place. */
    rti->snapshot_manager.bitmap &= ~( K64U_MSB >> no ); /* Mark as occupied. */
    rti->snapshot_manager.buffer[ no ] = snapshot;
    return no;
}

snapshot_t* snapshot_kill( rtinfo_t* rti, k8u no ) {
    assert( no < SNAPSHOT_SIZE ); /* Subscript check. */
    assert( ~rti->snapshot_manager.bitmap & ( K64U_MSB >> no ) ); /* Existance check. */
    rti->snapshot_manager.bitmap |= K64U_MSB >> no; /* Mark as available. */
    return &( rti->snapshot_manager.buffer[ no ] );
}

snapshot_t* snapshot_read( rtinfo_t* rti, k8u no ) {
    assert( no < SNAPSHOT_SIZE ); /* Subscript check. */
    assert( ~rti->snapshot_manager.bitmap & ( K64U_MSB >> no ) ); /* Existance check. */
    return &( rti->snapshot_manager.buffer[ no ] );
}

void snapshot_init( rtinfo_t* rti, snapshot_t* buffer ) {
    rti->snapshot_manager.bitmap = ~ K64U_C( 0 ); /* every bit is set to available. */
    rti->snapshot_manager.buffer = &buffer[ get_global_id( 0 ) * SNAPSHOT_SIZE ];
}

float enemy_health_percent( rtinfo_t* rti ){
    /*
        What differs from SimulationCraft, OpenCL iterations are totally parallelized.
        It's impossible to determine mob initial health by the results from previous iterations.
        The best solution here is to use a linear mix of time to approximate the health precent,
        which is used in SimC for the very first iteration.
    */
    time_t remainder = max( FROM_SECONDS( 0 ), rti->expected_combat_length - rti->timestamp );
    return mix( death_pct, initial_health_percentage, (float)remainder / (float)rti->expected_combat_length );
}

void sim_init(rtinfo_t* rti, k32u seed, snapshot_t* ssbuf){
    /* Simulate get_global_id for CPU. */
    hostonly(
        static int gid = 0;
        set_global_id(0, gid++);
    )
    /* Write zero to RTI. */
    *rti = (rtinfo_t){};
    /* RNG. */
    rng_init( &rti->seed, seed );
    /* Snapshot manager. */
    snapshot_init( rti, ssbuf );

    /* Combat length. */
    assert( vary_combat_length < max_length ); /* Vary can't be greater than max. */
    assert( vary_combat_length + max_length < 655.35f ); /* It's suggested  */
    rti->expected_combat_length = FROM_SECONDS( max_length + vary_combat_length * clamp( stdnor_rng( &rti->seed ) * ( 1.0f / 3.0f ), -1.0f, 1.0f ) );

}

/* Delete this. */
void scan_apl( rtinfo_t* rti ){
}
void routine_entries( rtinfo_t* rti, event_t e ){
    printf("Time = %d, Event %d, snapshot: %d\n", rti->timestamp, e.routine, snapshot_kill(rti, e.snapshot)->placeholder);
}

int main(){
    rtinfo_t rti;
    deviceonly(__global) snapshot_t buffer[SNAPSHOT_SIZE];
    int i, j;
    for (j = 0; j < 20; j++){
        for (i = 0; i < 5; i++){
            sim_init(&rti, j*5+i, buffer);
            printf("%d\t", rti.expected_combat_length);
        }
        printf("\n");
    }
}
