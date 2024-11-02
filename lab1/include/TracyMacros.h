#pragma once

#ifdef TRACY_ENABLE
#include <Tracy/tracy/Tracy.hpp>
#define TRACE_SCOPE ZoneScoped
#define TRACE_SCOPE_NAMED(name) ZoneScopedN(name)
#define TRACE_PLOT(name, value) TracyPlot(name, value)
#define TRACE_FRAME FrameMark
#define TRACE_FRAME_NAMED(name) FrameMarkNamed(name)
#else
#define TRACE_SCOPE
#define TRACE_SCOPE_NAMED(name)
#define TRACE_PLOT(name, value)
#define TRACE_FRAME
#define TRACE_FRAME_NAMED(name)
#endif
