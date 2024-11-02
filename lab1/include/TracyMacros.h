#pragma once
/*
    .h Macro para utilizar Tracy profiler https://github.com/wolfpld/tracy
    - para evitar conflictos, se hacen macros simbolicos para las funciones de
   Tracy
    - si no esta definido, los macros no hacen nada y tampoco geneneran errores
    - se puede definir TRACY_ENABLE en el archivo de proyecto o un archivo para
   habilitar (aunque el make ya lo hace con make tracy)
*/
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
