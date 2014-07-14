/**
    common library, prefixed to every kernel.
    2014.7.14
*/

#define _DEBUG

/* Diagnostic. */
#if defined(_DEBUG) && !defined(__OPENCL_VERSION__)
int printf(const char * format, ...);
void abort(void);
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
#define k8s char
#define k8u uchar
#define k16s short
#define k16u ushort
#define k32s int
#define k32u uint
#define k64s long
#define k64u ulong
#else
#define k8s signed char
#define k8u unsigned char
#define k16s short int
#define k16u unsigned short int
#define k32s long int
#define k32u unsigned long int
#define k64s long long int
#define k64u unsigned long long int
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

/* Event queue. */
typedef struct{
    time_t time;
    k8u routine;
    k8u snapshot;
} event_t;
typedef struct{
    k32u count;
    event_t event[63];
    event_t power_suffice;
} event_queue_t;

/* Runtime info struct, each thread preserves its own. */
typedef struct kdeclspec( packed ) {
    seed_t seed;
    time_t timestamp;
    event_queue_t eq;

} rtinfo_t;

/* Initialize RNG */
/*** Behaviors of uninitialized RNG is undefined!!! ***/
void init_rng( seed_t* state, k32u seed ) {
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

/* Generate one IEEE-754 single precision float point within [.0f, 1.0f). */
float rng( seed_t* seed ) {
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

/* Enroll an event into EQ. */
event_t* eq_enroll(event_queue_t* eq, time_t trigger, k8u routine, k8u snapshot){
    k32u i = ++eq->count;
    event_t* p = &eq->event[-1];

    assert( eq->count < 64 ); /* No full check on device. */

    for( ; i > 1 && p[i >> 1].time > trigger; i >>= 1 )
        p[i] = p[i >> 1];
    p[i] = (event_t){ .time = trigger, .routine = routine, .snapshot = snapshot };
    return &p[i];
}

/* Execute the top priority. */
void eq_execute( rtinfo_t* rti ){
    k16u i, child;
    event_t min, last;
    event_t* p = &rti->eq.event[-1];

    assert( rti->eq.count ); /* No empty check on device. */
    assert( rti->eq.count < 64 ); /* Not zero but negative? */

    min = p[1];
    if (!rti->eq.power_suffice.time || rti->eq.power_suffice.time >= min.time){
        /* Delete from heap. */
        last = p[rti->eq.count--];
        for( i = 1; i << 1 <= rti->eq.count; i = child ){
            child = i << 1;
            if ( child != rti->eq.count && rti->eq.event[child].time < p[child].time )
                child++;
            if ( last.time > p[child].time )
                p[i] = p[child];
            else
                break;
        }
        p[i] = last;
    }else{
        /* Invoke power suffice routine. */
        min = rti->eq.power_suffice;
        rti->eq.power_suffice.time = 0;
    }

    /* Now 'min' contains the top priority. Execute it. */
    rti->timestamp = min.time;
    /* TODO: execute a event. */
    printf("Execute %d\n", min.routine);

    /* TODO: scan APL. */
}

/* Delete this. */
int main(){
    rtinfo_t rti = {};
    eq_enroll(&rti.eq, 1, 2, 0);
    eq_enroll(&rti.eq, 3, 4, 0);
    eq_enroll(&rti.eq, 12, 13, 0);
    eq_enroll(&rti.eq, 7, 8, 0);
    eq_enroll(&rti.eq, 5, 6, 0);
    eq_execute(&rti); printf("\tTime = %d\n", rti.timestamp);
    eq_enroll(&rti.eq, 2, 3, 0);
    eq_enroll(&rti.eq, 2, 3, 0);
    eq_execute(&rti); printf("\tTime = %d\n", rti.timestamp);
    eq_execute(&rti); printf("\tTime = %d\n", rti.timestamp);
    eq_execute(&rti); printf("\tTime = %d\n", rti.timestamp);
    eq_execute(&rti); printf("\tTime = %d\n", rti.timestamp);
    eq_execute(&rti); printf("\tTime = %d\n", rti.timestamp);
    eq_execute(&rti); printf("\tTime = %d\n", rti.timestamp);
}
