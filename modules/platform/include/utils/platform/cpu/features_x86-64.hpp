#pragma once

#include "utils/platform/cpu/cpuid.hpp"

#include <cstring>




namespace utils::platform::cpu
{
#if defined( __x86_64__ ) || defined( _M_X64 )

struct Features
{
    /// https://www.felixcloutier.com/x86/cpuid
    // Leaf 0, EBX, ECX, EDX
    char vendor[ 13 ] {};

    // Leaf 0, EAX
    UInt32 level{};


    // XCR0: OS's checks
    bool osxmm      : 1 {};
    bool osymm      : 1 {};
    bool osopmask   : 1 {};
    bool oszmm_low  : 1 {};
    bool oszmm_high : 1 {};
    bool oszmm      : 1 {};


    // Leaf 1, ECX result
    bool sse3       : 1 {}; // Streaming SIMD Extensions 3 (SSE3)
    bool pclmulqdq  : 1 {};
    bool dtes64     : 1 {}; // 64-bit DS Area
    bool monitor    : 1 {}; // MONITOR/MWAIT
    bool dscpl      : 1 {}; // CPL Qualified Debug Store
    bool vmx        : 1 {}; // Virtual Machine Extensions
    bool smx        : 1 {}; // Safer Mode Extensions
    bool eist       : 1 {}; // Enhanced Intel SpeedStep® technology
    bool tm2        : 1 {}; // Thermal Monitor 2
    bool ssse3      : 1 {}; // Supplemental Streaming SIMD Extensions 3
    bool cnxtid     : 1 {}; // L1 context ID
    bool sdbg       : 1 {}; // IA32_DEBUG_INTERFACE MSR for silicon debug
    bool fma        : 1 {}; // FMA uses YMM state
    bool cmpxchg16b : 1 {}; // compare and exchange bytes
    bool xtpruc     : 1 {}; // xTPR Update Control
    bool pdcm       : 1 {}; // Perfmon and Debug Cap (IA32_PERF_CAPABILITIES)
    bool pcid       : 1 {}; // Process-context identifiers
    bool dca        : 1 {}; // Prefetch data from a memory mapped device
    bool sse4_1     : 1 {};
    bool sse4_2     : 1 {};
    bool x2apic     : 1 {}; // x2APIC feature
    bool movbe      : 1 {}; // MOVBE instruction
    bool popcnt     : 1 {}; // POPCNT instruction
    bool tscd       : 1 {}; // TSC deadline
    bool aesni      : 1 {}; // AESNI instruction extensions
    bool xsave      : 1 {}; // XSAVE/XRSTOR instructions
    // The OS has set CR4.OSXSAVE[bit 18] to enable XSETBV/XGETBV
    // instructions to access XCR0 and to support processor extended state
    // management using XSAVE/XRSTOR.
    // Must checked before using AVX2/512 registers
    bool osxsave    : 1 {};
    bool avx        : 1 {};
    bool f16c       : 1 {}; // 16-bit floating-point conversion instructions
    bool rdrand     : 1 {};

    // Leaf 1, EDX result
    bool fpu        : 1 {};
    bool vme        : 1 {};
    bool de         : 1 {};
    bool pse        : 1 {};
    bool tsc        : 1 {};
    bool msr        : 1 {}; // Model Specific Registers RDMSR and WRMSR
    bool pae        : 1 {};
    bool mce        : 1 {}; // Machine Check Exception
    bool cx8        : 1 {}; // CMPXCHG8B Instruction (locked and atomic)
    bool apic       : 1 {};
    bool sep        : 1 {}; // SYSENTER and SYSEXIT Instructions
    bool mtrr       : 1 {}; // Memory Type Range Registers
    bool pge        : 1 {}; // Page Global Bit
    bool mca        : 1 {}; // Machine Check Arch.
    bool cmov       : 1 {}; // Conditional Move instructions
    bool pat        : 1 {}; // Page Attribute Table
    bool pse36      : 1 {}; // 36-Bit Page Size Extension
    bool psn        : 1 {}; // Processor Serial Number
    bool clflush    : 1 {}; // CLFSH instruction
    bool ds         : 1 {}; // DEBUG STORE
    bool acpi       : 1 {};
    bool mmx        : 1 {};
    bool fxsr       : 1 {}; // FXSAVE and FXRSTOR Instructions (CR4.OSFXSR)
    bool sse        : 1 {};
    bool sse2       : 1 {};
    bool ss         : 1 {}; // Self Snoop
    bool htt        : 1 {}; // 0 = single logical processor
    bool tm         : 1 {}; // Thermal Monitor
    bool pbe        : 1 {}; // Pending Break Enable


    // leaf7:0, EBX result
    bool fsgsbase   : 1 {}; // FS/GS user mode
    bool bmi        : 1 {}; // Bit Manipulation Instructions
    bool avx2       : 1 {};
    bool bmi2       : 1 {};
    bool erms       : 1 {}; // Enhanced REP MOVSB/STOSB
    bool avx512f    : 1 {}; // AVX-512 Foundation
    bool avx512dq   : 1 {}; // AVX-512 Double/Quad
    bool avx512ifma : 1 {}; // AVX-512 Integer Fused Multiply-Add
    bool sha        : 1 {};
    bool avx512cd   : 1 {};
    bool avx512vl   : 1 {};
    bool avx512bw   : 1 {};

    // leaf7:0, ECX result
    bool prefetchwt1    : 1 {};
    bool ospke          : 1 {}; // OS Memory Protection Keys
    bool waitpkg        : 1 {}; // UMONITOR/UMWAIT/TPAUSE
    bool avx512vbmi     : 1 {};
    bool avx512vbmi2    : 1 {};
    bool avx512vnni     : 1 {};
    bool avx512vpopcntdq: 1 {};

    // leaf7:0, EDX result
    bool hybrid     : 1 {};
    bool avx512pb   : 1 {};
    bool avx512fp16 : 1 {};

    bool avx512     : 1 {};


    // extended leafs
    bool svm    : 1 {}; // AMD: Secure Virtual Machine


    UInt32 l1i_kb{}; // L1 Instruction Cache
    UInt32 l1d_kb{}; // L1 Data Cache
    UInt32 l2_kb{};  // L2 Cache
    UInt32 l3_kb{};  // L3 Cache


    char brand[ 49 ] {};



    inline static const Features& GetInstance( )
    {
        static const Features features{ };
        return features;
    }



private:
    explicit Features() noexcept
    {
        // 1. Vendor String (Leaf 0x00)
        const auto l0 = cpuid( 0x00 );
        const auto& eax0 = l0.eax;
        const auto& ebx0 = l0.ebx;
        const auto& ecx0 = l0.ecx;
        const auto& edx0 = l0.edx;
        std::memcpy( vendor + 0, &ebx0.value, 4 );
        std::memcpy( vendor + 4, &edx0.value, 4 );
        std::memcpy( vendor + 8, &ecx0.value, 4 );
        level = eax0.value;

        // 2. Base features (Leaf 0x01)
        const auto l1 = cpuid( 0x01 );
        const auto& eax1 = l1.eax;
        const auto& ebx1 = l1.ebx;
        const auto& ecx1 = l1.ecx;
        const auto& edx1 = l1.edx;
        sse3        = ecx1[ 0 ];
        pclmulqdq   = ecx1[ 1 ];
        dtes64      = ecx1[ 2 ];
        monitor     = ecx1[ 3 ];
        dscpl       = ecx1[ 4 ];
        vmx         = ecx1[ 5 ];
        smx         = ecx1[ 6 ];
        eist        = ecx1[ 7 ];
        tm2         = ecx1[ 8 ];
        ssse3       = ecx1[ 9 ];
        cnxtid      = ecx1[ 10 ];
        sdbg        = ecx1[ 11 ];
        //fma         = ecx1[ 12 ]; should checked after XSAVE/STOR checks
        cmpxchg16b  = ecx1[ 13 ];
        xtpruc      = ecx1[ 14 ];
        pdcm        = ecx1[ 15 ];
        // ecx1[ 16 ] reserved
        pcid        = ecx1[ 17 ];
        dca         = ecx1[ 18 ];
        sse4_1      = ecx1[ 19 ];
        sse4_2      = ecx1[ 20 ];
        x2apic      = ecx1[ 21 ];
        movbe       = ecx1[ 22 ];
        popcnt      = ecx1[ 23 ];
        tscd        = ecx1[ 24 ];
        aesni       = ecx1[ 25 ];
        xsave       = ecx1[ 26 ];
        osxsave     = ecx1[ 27 ];
        //avx         = ecx1[ 28 ]; should checked after XSAVE/STOR checks
        //f16c        = ecx1[ 29 ]; should checked after XSAVE/STOR checks
        rdrand      = ecx1[ 30 ];
        // ecx1[ 31 ] not used (always 0)

        fpu     = edx1[ 0 ];
        vme     = edx1[ 1 ];
        de      = edx1[ 2 ];
        pse     = edx1[ 3 ];
        tsc     = edx1[ 4 ];
        msr     = edx1[ 5 ];
        pae     = edx1[ 6 ];
        mce     = edx1[ 7 ];
        cx8     = edx1[ 8 ];
        apic    = edx1[ 9 ];
        // edx1[ 10 ] reserved
        sep     = edx1[ 11 ];
        mtrr    = edx1[ 12 ];
        pge     = edx1[ 13 ];
        mca     = edx1[ 14 ];
        cmov    = edx1[ 15 ];
        pat     = edx1[ 16 ];
        pse36   = edx1[ 17 ];
        psn     = edx1[ 18 ];
        clflush = edx1[ 19 ];
        // edx1[ 20 ] reserved
        ds      = edx1[ 21 ];
        acpi    = edx1[ 22 ];
        mmx     = edx1[ 23 ];
        fxsr    = edx1[ 24 ];
        sse     = edx1[ 25 ];
        sse2    = edx1[ 26 ];
        ss      = edx1[ 27 ];
        htt     = edx1[ 28 ];
        tm      = edx1[ 29 ];
        // edx1[ 30 ] reserved
        pbe     = edx1[ 31 ];

        if( xsave && osxsave )
        {
            XCR0 xcr0 = xgetbv();
            osxmm      = xcr0[ 1 ];
            osymm      = xcr0[ 2 ];
            osopmask   = xcr0[ 5 ];
            oszmm_low  = xcr0[ 6 ];
            oszmm_high = xcr0[ 7 ];
            oszmm      = osopmask && oszmm_low && oszmm_high;

            if( osxmm && osymm )
            {
                fma = ecx1[ 12 ];
                avx = ecx1[ 28 ];
                f16c = ecx1[ 29 ];
            }

            if( level >= 0x07 )
            {
                // Extended feateures (leaf 0x07)
                const auto l7 = cpuid( 0x07 );
                const auto& eax7 = l7.eax;
                const auto& ebx7 = l7.ebx;
                const auto& ecx7 = l7.ecx;
                const auto& edx7 = l7.edx;
                if( osxmm && osymm )
                {
                    avx2 = ebx7[ 5 ];

                    if( oszmm )
                    {
                        avx512f  = ebx7[ 16 ];
                        if( avx512f )
                        {
                            avx512dq   = ebx7[ 17 ];
                            avx512ifma = ebx7[ 21 ];
                            avx512cd   = ebx7[ 28 ];
                            avx512bw   = ebx7[ 30 ];
                            avx512vl   = ebx7[ 31 ];

                            avx512vbmi      = ecx7[ 1 ];
                            avx512vbmi2     = ecx7[ 6 ];
                            avx512vnni      = ecx7[ 11 ];
                            avx512vpopcntdq = ecx7[ 14 ];

                            avx512pb   = edx7[ 16 ];
                            avx512fp16 = edx7[ 23 ];

                            avx512 = (avx512f && avx512vl) &&
                                     (avx512bw && avx512dq);
                        }
                    }
                }

                fsgsbase = ebx7[ 0 ];
                bmi      = ebx7[ 3 ];
                bmi2     = ebx7[ 8 ];
                erms     = ebx7[ 9 ];
                sha      = ebx7[ 29 ];

                prefetchwt1 = ecx7[ 0 ];
                ospke       = ecx7[ 4 ];
                waitpkg     = ecx7[ 5 ];
            }
        }

        // 5. Brand String
        const auto leaf_ext = cpuid( 0x80000000 );
        if( leaf_ext.eax.value >= 0x80000004 )
        {
            const auto b1 = cpuid( 0x80000002 );
            const auto b2 = cpuid( 0x80000003 );
            const auto b3 = cpuid( 0x80000004 );

            std::memcpy( brand + 0,  &b1.eax.value, 4 );
            std::memcpy( brand + 4,  &b1.ebx.value, 4 );
            std::memcpy( brand + 8,  &b1.ecx.value, 4 );
            std::memcpy( brand + 12, &b1.edx.value, 4 );
            std::memcpy( brand + 16, &b2.eax.value, 4 );
            std::memcpy( brand + 20, &b2.ebx.value, 4 );
            std::memcpy( brand + 24, &b2.ecx.value, 4 );
            std::memcpy( brand + 28, &b2.edx.value, 4 );
            std::memcpy( brand + 32, &b3.eax.value, 4 );
            std::memcpy( brand + 36, &b3.ebx.value, 4 );
            std::memcpy( brand + 40, &b3.ecx.value, 4 );
            std::memcpy( brand + 44, &b3.edx.value, 4 );
        }

        const bool is_intel = vendor[ 0 ] == 'G';
        const bool is_amd   = vendor[ 0 ] == 'A';

        if( is_intel && level >= 4 )
        {
            for ( uint32_t i = 0; ; ++i )
            {
                const auto l4 = cpuid( 0x04, i );
                uint32_t cache_type = l4.eax.value & 0x1F;

                if ( cache_type == 0 ) break;

                uint32_t cache_level = (l4.eax.value >> 5) & 0x7;
                uint32_t ways        = ( (l4.ebx.value >> 22) & 0x3FF ) + 1;
                uint32_t partitions  = ( (l4.ebx.value >> 12) & 0x3FF ) + 1;
                uint32_t line_size   = ( l4.ebx.value & 0xFFF ) + 1;
                uint32_t sets        = l4.ecx.value + 1;

                // Intel: (Ways * Partitions * LineSize * Sets)
                uint32_t size_kb =
                    ( ways * partitions * line_size * sets ) / 1024;

                if( cache_level == 1 && cache_type == 1 )
                    l1d_kb = size_kb; // Data
                else if( cache_level == 1 && cache_type == 2 )
                    l1i_kb = size_kb; // Instruction
                else if( cache_level == 2 )
                    l2_kb = size_kb;
                else if( cache_level == 3 )
                    l3_kb = size_kb;
            }
        }
        else if( is_amd )
        {
            if( leaf_ext.eax.value >= 0x80000001 )
            {
                const auto amd_ext_leaf = cpuid( 0x80000001 );
                svm = amd_ext_leaf.ecx[ 2 ];
            }

            if( leaf_ext.eax.value >= 0x80000006 )
            {
                // Leaf 0x80000005: L1 Cache
                auto l5 = cpuid( 0x80000005 );
                l1d_kb = ( l5.ecx.value >> 24 ) & 0xFF; // 31:24 bits
                l1i_kb = ( l5.edx.value >> 24 ) & 0xFF; // 31:24 bits

                // Leaf 0x80000006: L2 and L3 Cache
                auto l6 = cpuid( 0x80000006 );
                l2_kb = ( l6.ecx.value >> 16 ) & 0xFFFF; // 31:16 bits

                // L3 = chunks = 512kb (31:18 bits)
                uint32_t l3_chunks = ( l6.edx.value >> 18 ) & 0x3FFF;
                l3_kb = l3_chunks * 512;
            }
        }
    }
};



inline const Features& GetFeatures( )
{
    return Features::GetInstance();
}



inline const Features& features = GetFeatures();
#endif
} // namespace utils::platform::cpu
