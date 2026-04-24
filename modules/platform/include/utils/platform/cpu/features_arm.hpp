#pragma once


#if defined( __linux__ ) || defined( __ANDROID__ )
#   include <sys/auxv.h>
#   include <asm/hwcap.h>
#   include <unistd.h> // For sysconf
#elif defined( __APPLE__ )
#   include <sys/sysctl.h>
#elif defined( _WIN32 )
#   include <Windows.h>
#endif




namespace utils::platform::cpu
{
#if defined( __aarch64__ ) || defined( _M_ARM64 )

struct Features
{
    // --- Basic Vector Extensions ---
    bool asimd   : 1 {}; // Advanced SIMD (NEON) - Equivalent to SSE/AVX

    // --- Cryptography and Hashing ---
    bool crc32   : 1 {};
    bool aes     : 1 {};
    bool sha1    : 1 {};
    bool sha2    : 1 {};
    bool pmull   : 1 {}; // Polynomial Multiply (often needed alongside AES)

    // --- Advanced Compute Features ---
    bool fp16    : 1 {}; // Half-precision floating-point
    bool atomics : 1 {}; // LSE (Large System Extensions) - Fast std::atomic
    bool dotprod : 1 {};

    // --- SVE (Scalable Vector Extension) - Equivalent to AVX-512 ---
    bool sve     : 1 {};
    bool sve2    : 1 {};

    // Cache sizes (in Kilobytes)
    uint32_t l1i_kb{};
    uint32_t l1d_kb{};
    uint32_t l2_kb{};
    uint32_t l3_kb{};



    inline static const Features& GetInstance()
    {
        static const Features features{};
        return features;
    }


private:
    explicit Features() noexcept
    {
        // ==========================================
        // 1. LINUX / ANDROID (Reading Auxiliary Vector)
        // ==========================================
#if defined( __linux__ ) || defined( __ANDROID__ )

        unsigned long hwcaps  = getauxval( AT_HWCAP );
        unsigned long hwcaps2 = getauxval( AT_HWCAP2 );

        asimd   = ( hwcaps & HWCAP_ASIMD ) != 0;
        crc32   = ( hwcaps & HWCAP_CRC32 ) != 0;
        aes     = ( hwcaps & HWCAP_AES )   != 0;
        sha1    = ( hwcaps & HWCAP_SHA1 )  != 0;
        sha2    = ( hwcaps & HWCAP_SHA2 )  != 0;
        pmull   = ( hwcaps & HWCAP_PMULL ) != 0;
        atomics = ( hwcaps & HWCAP_ATOMICS ) != 0;
        fp16    = ( hwcaps & HWCAP_FPHP )  != 0;

// SVE features (available in newer kernels)
#if defined( HWCAP_SVE )
        sve     = ( hwcaps & HWCAP_SVE ) != 0;
#endif
#if defined( HWCAP2_SVE2 )
        sve2    = ( hwcaps2 & HWCAP2_SVE2 ) != 0;
#endif

        // Reading caches via sysconf (often returns 0 on ARM Linux,
        // as the kernel hides topology, but worth a try)
        long l1d = sysconf( _SC_LEVEL1_DCACHE_SIZE );
        long l1i = sysconf( _SC_LEVEL1_ICACHE_SIZE );
        long l2  = sysconf( _SC_LEVEL2_CACHE_SIZE );
        long l3  = sysconf( _SC_LEVEL3_CACHE_SIZE );

        if ( l1d > 0 ) l1d_kb = l1d / 1024;
        if ( l1i > 0 ) l1i_kb = l1i / 1024;
        if ( l2  > 0 ) l2_kb  = l2  / 1024;
        if ( l3  > 0 ) l3_kb  = l3  / 1024;

// ==========================================
// 2. APPLE macOS / iOS (Reading via sysctl)
// ==========================================
#elif defined( __APPLE__ )

        // Lambda helper for convenient sysctl reading
        auto check_sysctl = []( const char* name ) -> bool {
            int val = 0;
            size_t size = sizeof( val );
            if ( sysctlbyname( name, &val, &size, nullptr, 0 ) == 0 ) {
                return val == 1;
            }
            return false;
        };

        // On Apple Silicon (M1/M2/M3), ASIMD (NEON) is always hw available
        asimd   = true;

        crc32   = check_sysctl( "hw.optional.armv8_crc32" );
        aes     = check_sysctl( "hw.optional.arm.FEAT_AES" );
        sha1    = check_sysctl( "hw.optional.arm.FEAT_SHA1" );
        sha2    = check_sysctl( "hw.optional.arm.FEAT_SHA2" );
        pmull   = check_sysctl( "hw.optional.arm.FEAT_PMULL" );
        atomics = check_sysctl( "hw.optional.arm.FEAT_LSE" );
        fp16    = check_sysctl( "hw.optional.arm.FEAT_FP16" );
        dotprod = check_sysctl( "hw.optional.arm.FEAT_DotProd" );

        auto get_cache = []( const char* name ) -> uint32_t {
            int64_t val = 0;
            size_t size = sizeof( val );
            if ( sysctlbyname( name, &val, &size, nullptr, 0 ) == 0 ) {
                return static_cast<uint32_t>( val / 1024 );
            }
            return 0;
        };

        l1d_kb = get_cache( "hw.l1dcachesize" );
        l1i_kb = get_cache( "hw.l1icachesize" );
        l2_kb  = get_cache( "hw.l2cachesize" );
        l3_kb  = get_cache( "hw.l3cachesize" );

// ==========================================
// 3. WINDOWS ON ARM (Reading via WinAPI)
// ==========================================
#elif defined( _WIN32 )

        asimd =
            IsProcessorFeaturePresent( PF_ARM_V8_INSTRUCTIONS_AVAILABLE );

        crc32 =
            IsProcessorFeaturePresent(
                PF_ARM_V8_CRC32_INSTRUCTIONS_AVAILABLE );
        aes   =
            IsProcessorFeaturePresent(
                PF_ARM_V8_CRYPTO_INSTRUCTIONS_AVAILABLE );

        sha1  = aes;
        sha2  = aes;

        atomics =
            IsProcessorFeaturePresent(
                PF_ARM_V81_ATOMIC_INSTRUCTIONS_AVAILABLE );

        dotprod =
            IsProcessorFeaturePresent(
                PF_ARM_V82_DP_INSTRUCTIONS_AVAILABLE );

// For Windows on ARM, reading the cache requires calling
// GetLogicalProcessorInformationEx, which requires memory allocation.
// Leaving as 0 by default for simplicity.
#endif
    }
};



inline const Features& GetFeatures( )
{
    return Features::GetInstance();
}



inline const Features& features = GetFeatures();
#endif
} // namespace utils::platform::cpu
