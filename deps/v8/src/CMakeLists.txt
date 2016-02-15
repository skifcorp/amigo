

cmake_minimum_required(VERSION 2.6)

include_directories(.)


set (CMAKE_CXX_FLAGS "-std=gnu++0x -pthread -Wall -Werror -W -Wno-unused-parameter -Wno-unused-but-set-variable -Wnon-virtual-dtor -pedantic -m32 -O3 -fomit-frame-pointer -fdata-sections -ffunction-sections -fno-rtti -fno-exceptions -fvisibility=hidden -Wall -Werror -W -Wno-unused-parameter -Wno-unused-but-set-variable -Wnon-virtual-dtor -pedantic -m32 -O3 -fomit-frame-pointer -fdata-sections -ffunction-sections -DV8_TARGET_ARCH_IA32 -DV8_FAST_TLS -DENABLE_DEBUGGER_SUPPORT -DENABLE_VMSTATE_TRACKING -DENABLE_LOGGING_AND_PROFILING")

macro (check_arch)
  INCLUDE(CheckTypeSize)
  CHECK_TYPE_SIZE(void*  SIZEOF_VOID_PTR)

  IF( ${SIZEOF_VOID_PTR} MATCHES "^8$" )
    SET( OS_32_BITS 0 )
    SET( OS_64_BITS 1 )
  ELSE( ${SIZEOF_VOID_PTR} MATCHES "^8$" )
    SET( OS_32_BITS 1 )
    SET( OS_64_BITS 0 )
  ENDIF( ${SIZEOF_VOID_PTR} MATCHES "^8$" )
endmacro()

set ( V8_HEADERS "" )


set ( V8_SOURCES 
    accessors.cc
    allocation.cc
    api.cc
    assembler.cc
    ast.cc
    atomicops_internals_x86_gcc.cc
    bignum.cc
    bignum-dtoa.cc
    bootstrapper.cc
    builtins.cc
    cached-powers.cc
    checks.cc
    circular-queue.cc
    code-stubs.cc
    codegen.cc
    compilation-cache.cc
    compiler.cc
    contexts.cc
    conversions.cc
    counters.cc
    cpu-profiler.cc
    data-flow.cc
    dateparser.cc
    debug-agent.cc
    debug.cc
    deoptimizer.cc
    disassembler.cc
    diy-fp.cc
    dtoa.cc
    execution.cc
    factory.cc
    flags.cc
    frames.cc
    full-codegen.cc
    func-name-inferrer.cc
    gdb-jit.cc
    global-handles.cc
    fast-dtoa.cc
    fixed-dtoa.cc
    handles.cc
    hashmap.cc
    heap-profiler.cc
    heap.cc
    hydrogen.cc
    hydrogen-instructions.cc
    ic.cc
    inspector.cc
    interpreter-irregexp.cc
    isolate.cc
    jsregexp.cc
    lithium-allocator.cc
    lithium.cc
    liveedit.cc
    liveobjectlist.cc
    log-utils.cc
    log.cc
    mark-compact.cc
    messages.cc
    objects.cc
    objects-printer.cc
    objects-visiting.cc
    parser.cc
    preparser.cc
    preparse-data.cc
    profile-generator.cc
    property.cc
    regexp-macro-assembler-irregexp.cc
    regexp-macro-assembler.cc
    regexp-stack.cc
    rewriter.cc
    runtime.cc
    runtime-profiler.cc
    safepoint-table.cc
    scanner-base.cc
    scanner.cc
    scopeinfo.cc
    scopes.cc
    serialize.cc
    snapshot-common.cc
    spaces.cc
    string-search.cc
    string-stream.cc
    strtod.cc
    stub-cache.cc
    token.cc
    type-info.cc
    unicode.cc
    utils.cc
    v8-counters.cc
    v8.cc
    v8threads.cc
    variables.cc
    version.cc
    zone.cc
    extensions/gc-extension.cc
    extensions/externalize-string-extension.cc
)


check_arch()

IF(OS_32_BITS) 
  set(V8_SOURCES ${V8_SOURCES} 
    ia32/assembler-ia32.cc
    ia32/builtins-ia32.cc
    ia32/code-stubs-ia32.cc
    ia32/codegen-ia32.cc
    ia32/cpu-ia32.cc
    ia32/debug-ia32.cc
    ia32/deoptimizer-ia32.cc
    ia32/disasm-ia32.cc
    ia32/frames-ia32.cc
    ia32/full-codegen-ia32.cc
    ia32/ic-ia32.cc
    ia32/lithium-codegen-ia32.cc
    ia32/lithium-gap-resolver-ia32.cc
    ia32/lithium-ia32.cc
    ia32/macro-assembler-ia32.cc
    ia32/regexp-macro-assembler-ia32.cc
    ia32/stub-cache-ia32.cc  )
ELSEIF(OS_64_BITS)
  set(V8_SOURCES ${V8_SOURCES} 
    x64/assembler-x64.cc
    x64/builtins-x64.cc
    x64/code-stubs-x64.cc
    x64/codegen-x64.cc
    x64/cpu-x64.cc
    x64/debug-x64.cc
    x64/deoptimizer-x64.cc
    x64/disasm-x64.cc
    x64/frames-x64.cc
    x64/full-codegen-x64.cc
    x64/ic-x64.cc
    x64/lithium-codegen-x64.cc
    x64/lithium-gap-resolver-x64.cc
    x64/lithium-x64.cc
    x64/macro-assembler-x64.cc
    x64/regexp-macro-assembler-x64.cc
    x64/stub-cache-x64.cc  )
ENDIF(OS_32_BITS)

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
  MESSAGE("Linux....")
  set(V8_SOURCES ${V8_SOURCES} platform-posix.cc platform-linux.cc)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
  MESSAGE("Windows....")
  set(V8_SOURCES ${V8_SOURCES} platform-win32.cc)
ENDIF(CMAKE_SYSTEM_NAME MATCHES "Linux")

IF("$ENV{mode}" STREQUAL "Debug" )
  set(V8_SOURCES ${V8_SOURCES} objects-debug.cc prettyprinter.cc regexp-macro-assembler-tracer.cc)
ENDIF("$ENV{mode}" STREQUAL "Debug")


set (LIBRARY_FILES ${CMAKE_CURRENT_SOURCE_DIR}/runtime.js 
                   ${CMAKE_CURRENT_SOURCE_DIR}/v8natives.js 
                   ${CMAKE_CURRENT_SOURCE_DIR}/array.js 
                   ${CMAKE_CURRENT_SOURCE_DIR}/string.js 
                   ${CMAKE_CURRENT_SOURCE_DIR}/uri.js 
                   ${CMAKE_CURRENT_SOURCE_DIR}/math.js 
                   ${CMAKE_CURRENT_SOURCE_DIR}/messages.js 
                   ${CMAKE_CURRENT_SOURCE_DIR}/apinatives.js 
                   ${CMAKE_CURRENT_SOURCE_DIR}/date.js
                   ${CMAKE_CURRENT_SOURCE_DIR}/regexp.js 
                   ${CMAKE_CURRENT_SOURCE_DIR}/json.js 
                   ${CMAKE_CURRENT_SOURCE_DIR}/liveedit-debugger.js 
                   ${CMAKE_CURRENT_SOURCE_DIR}/mirror-debugger.js 
                   ${CMAKE_CURRENT_SOURCE_DIR}/debug-debugger.js )


set (EXPERIMENTAL_LIBRARY_FILES ${CMAKE_CURRENT_SOURCE_DIR}/proxy.js)


EXECUTE_PROCESS( COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/../tools/js2c.py ${CMAKE_CURRENT_BINARY_DIR}/libries.cc CORE ${LIBRARY_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/macros.py  )
EXECUTE_PROCESS( COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/../tools/js2c.py ${CMAKE_CURRENT_BINARY_DIR}/experimental-libries.cc EXPERIMENTAL ${EXPERIMENTAL_LIBRARY_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/macros.py  )

set (V8_SOURCES ${V8_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/libries.cc ${CMAKE_CURRENT_BINARY_DIR}/experimental-libries.cc )

set( V8_SOURCES ${V8_SOURCES} snapshot-empty.cc )

add_library(v8 STATIC ${V8_HEADERS} ${V8_SOURCES}) 


set_property (TARGET v8 PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../../lib)

set(GLOB_HEADERS_SRC_DIR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(GLOB_HEADERS v8-debug.h 
                 v8-preparser.h 
                 v8-profiler.h 
                 v8-testing.h 
                 v8.h 
                 v8stdint.h)

set(GLOB_HEADERS_DST_DIR_PATH ${CMAKE_CURRENT_BINARY_DIR}/../../../include/v8)

foreach (header ${GLOB_HEADERS})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${GLOB_HEADERS_SRC_DIR_PATH}/${header} ${GLOB_HEADERS_DST_DIR_PATH}/${header} )
endforeach(header)
