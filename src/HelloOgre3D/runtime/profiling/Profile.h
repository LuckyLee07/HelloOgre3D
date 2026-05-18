#ifndef __RUNTIME_PROFILING_PROFILE_H__
#define __RUNTIME_PROFILING_PROFILE_H__

#if defined(HELLO_ENABLE_TRACY)
#include "tracy/Tracy.hpp"

#define H3D_PROFILE_CONCAT_IMPL(a, b) a##b
#define H3D_PROFILE_CONCAT(a, b) H3D_PROFILE_CONCAT_IMPL(a, b)
#define H3D_PROFILE_FRAME() FrameMark
#define H3D_PROFILE_SCOPE(name) ZoneNamedN(H3D_PROFILE_CONCAT(__h3d_tracy_zone, __LINE__), name, true)
#define H3D_PROFILE_SCOPE_NAMED(varname, name) ZoneNamedN(varname, name, true)
#define H3D_PROFILE_TEXT(varname, text, size) ZoneTextV(varname, text, size)
#define H3D_PROFILE_PLOT(name, value) TracyPlot(name, value)
#define H3D_PROFILE_THREAD(name) tracy::SetThreadName(name)
#else
#define H3D_PROFILE_FRAME()
#define H3D_PROFILE_SCOPE(name)
#define H3D_PROFILE_SCOPE_NAMED(varname, name)
#define H3D_PROFILE_TEXT(varname, text, size)
#define H3D_PROFILE_PLOT(name, value)
#define H3D_PROFILE_THREAD(name)
#endif

#endif  // __RUNTIME_PROFILING_PROFILE_H__
