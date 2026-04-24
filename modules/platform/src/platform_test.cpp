#include "utils/platform.hpp"

#include <print>
using namespace std;




int main( )
{
    using namespace utils::platform;
#if defined( __x86_64__ ) || defined( _M_X64 )
    using cpu::features;

    cpu::EAX eax{ 0x12345678 };
    cpu::EBX ebx{ 0x10101010 };
    println( "EAX = {:08x}", eax.value );
    println( "EBX = {:08x}", ebx.value );
    println( "EAL = {:08x}", eax.low.value );
    println( "EAH = {:08x}", eax.high.value );
    println ( "EAX ^ EBX = {:08x}", (eax ^ ebx).value );
    println();
    println( "CPU features:" );
    println( "CPU vendor: {}", features.vendor );
    println( "CPU brand: {}", features.brand );
    println( "CPUID level: {}", features.level );
    println( "L1i cache (instructions): {}kb", features.l1i_kb );
    println( "L1d cache (data): {}kb", features.l1d_kb );
    println( "L2 cache: {}kb", features.l2_kb );
    println( "L3 cache: {}kb", features.l3_kb );
    println( "Virtual Machine Extensions: {}", features.vmx );
    println( "Safer Mode Extensions: {}", features.smx );
    println( "VME: {}", features.vme );
    println( "XSAVE/XRSTOR support: {}", features.xsave );
    println( "OS XSAVE/XRSTOR support: {}", features.osxsave );
    println( "OS saves XMM registers: {}", features.osxmm );
    println( "OS saves YMM registers: {}", features.osymm );
    println( "OS saves low ZMM registers: {}", features.oszmm_low );
    println( "OS saves high ZMM registers: {}", features.oszmm_high );
    println( "OS saves ZMM registers: {}", features.oszmm );
    println( "CPU has XMM/SSE: {}", features.sse );
    println( "CPU has SSE 2.0: {}", features.sse2 );
    println( "CPU has SSE 3.0: {}", features.sse3 );
    println( "CPU has SSSE 3.0: {}", features.ssse3 );
    println( "CPU has SSE 4.1: {}", features.sse4_1 );
    println( "CPU has SSSE 4.2: {}", features.sse4_2 );
    println( "CPU has AVX: {}", features.avx );
    println( "CPU has AVX-2: {}", features.avx2 );
    println( "CPU has AVX-512 Foundation: {}", features.avx512f );
    println( "CPU has AVX-512 Bytes Words: {}", features.avx512bw );
    println( "CPU has AVX-512 Vector Length: {}", features.avx512vl );
    println( "AVX-512 support: {}", features.avx512 );
    println();
    println( "Size of the Features object: {}", sizeof( features ) );
#endif
}
