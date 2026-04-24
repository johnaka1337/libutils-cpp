#pragma once

#if defined( __x86_64__ ) || defined( _M_X64 )
#   include "utils/platform/cpu/features_x86-64.hpp"
#elif defined( __aarch64__ ) || defined( _M_ARM64 )
#   include "utils/platform/cpu/features_arm.hpp"
#else
#   error "Unsupported architecture!"
#endif




namespace utils::platform::cpu
{
extern const Features& features;
}
