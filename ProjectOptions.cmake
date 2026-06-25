include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


include(CheckCXXSourceCompiles)


macro(MyMonsterWar_supports_sanitizers)
  # Emscripten doesn't support sanitizers
  if(EMSCRIPTEN)
    set(SUPPORTS_UBSAN OFF)
    set(SUPPORTS_ASAN OFF)
  elseif((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)

    message(STATUS "Sanity checking UndefinedBehaviorSanitizer, it should be supported on this platform")
    set(TEST_PROGRAM "int main() { return 0; }")

    # Check if UndefinedBehaviorSanitizer works at link time
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=undefined")
    set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=undefined")
    check_cxx_source_compiles("${TEST_PROGRAM}" HAS_UBSAN_LINK_SUPPORT)

    if(HAS_UBSAN_LINK_SUPPORT)
      message(STATUS "UndefinedBehaviorSanitizer is supported at both compile and link time.")
      set(SUPPORTS_UBSAN ON)
    else()
      message(WARNING "UndefinedBehaviorSanitizer is NOT supported at link time.")
      set(SUPPORTS_UBSAN OFF)
    endif()
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    if (NOT WIN32)
      message(STATUS "Sanity checking AddressSanitizer, it should be supported on this platform")
      set(TEST_PROGRAM "int main() { return 0; }")

      # Check if AddressSanitizer works at link time
      set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
      set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address")
      check_cxx_source_compiles("${TEST_PROGRAM}" HAS_ASAN_LINK_SUPPORT)

      if(HAS_ASAN_LINK_SUPPORT)
        message(STATUS "AddressSanitizer is supported at both compile and link time.")
        set(SUPPORTS_ASAN ON)
      else()
        message(WARNING "AddressSanitizer is NOT supported at link time.")
        set(SUPPORTS_ASAN OFF)
      endif()
    else()
      set(SUPPORTS_ASAN ON)
    endif()
  endif()
endmacro()

macro(MyMonsterWar_setup_options)
  option(MyMonsterWar_ENABLE_HARDENING "Enable hardening" ON)
  option(MyMonsterWar_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    MyMonsterWar_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    MyMonsterWar_ENABLE_HARDENING
    OFF)

  MyMonsterWar_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR MyMonsterWar_PACKAGING_MAINTAINER_MODE)
    option(MyMonsterWar_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(MyMonsterWar_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(MyMonsterWar_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(MyMonsterWar_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(MyMonsterWar_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(MyMonsterWar_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(MyMonsterWar_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(MyMonsterWar_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(MyMonsterWar_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(MyMonsterWar_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(MyMonsterWar_ENABLE_PCH "Enable precompiled headers" OFF)
    option(MyMonsterWar_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(MyMonsterWar_ENABLE_IPO "Enable IPO/LTO" ON)
    option(MyMonsterWar_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(MyMonsterWar_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(MyMonsterWar_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(MyMonsterWar_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(MyMonsterWar_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(MyMonsterWar_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(MyMonsterWar_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(MyMonsterWar_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(MyMonsterWar_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(MyMonsterWar_ENABLE_PCH "Enable precompiled headers" OFF)
    option(MyMonsterWar_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      MyMonsterWar_ENABLE_IPO
      MyMonsterWar_WARNINGS_AS_ERRORS
      MyMonsterWar_ENABLE_SANITIZER_ADDRESS
      MyMonsterWar_ENABLE_SANITIZER_LEAK
      MyMonsterWar_ENABLE_SANITIZER_UNDEFINED
      MyMonsterWar_ENABLE_SANITIZER_THREAD
      MyMonsterWar_ENABLE_SANITIZER_MEMORY
      MyMonsterWar_ENABLE_UNITY_BUILD
      MyMonsterWar_ENABLE_CLANG_TIDY
      MyMonsterWar_ENABLE_CPPCHECK
      MyMonsterWar_ENABLE_COVERAGE
      MyMonsterWar_ENABLE_PCH
      MyMonsterWar_ENABLE_CACHE)
  endif()

  MyMonsterWar_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (MyMonsterWar_ENABLE_SANITIZER_ADDRESS OR MyMonsterWar_ENABLE_SANITIZER_THREAD OR MyMonsterWar_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(MyMonsterWar_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(MyMonsterWar_global_options)
  if(MyMonsterWar_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    MyMonsterWar_enable_ipo()
  endif()

  MyMonsterWar_supports_sanitizers()

  if(MyMonsterWar_ENABLE_HARDENING AND MyMonsterWar_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR MyMonsterWar_ENABLE_SANITIZER_UNDEFINED
       OR MyMonsterWar_ENABLE_SANITIZER_ADDRESS
       OR MyMonsterWar_ENABLE_SANITIZER_THREAD
       OR MyMonsterWar_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${MyMonsterWar_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${MyMonsterWar_ENABLE_SANITIZER_UNDEFINED}")
    MyMonsterWar_enable_hardening(MyMonsterWar_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(MyMonsterWar_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(MyMonsterWar_warnings INTERFACE)
  add_library(MyMonsterWar_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  MyMonsterWar_set_project_warnings(
    MyMonsterWar_warnings
    ${MyMonsterWar_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  include(cmake/Linker.cmake)
  # Must configure each target with linker options, we're avoiding setting it globally for now

  if(NOT EMSCRIPTEN)
    include(cmake/Sanitizers.cmake)
    MyMonsterWar_enable_sanitizers(
      MyMonsterWar_options
      ${MyMonsterWar_ENABLE_SANITIZER_ADDRESS}
      ${MyMonsterWar_ENABLE_SANITIZER_LEAK}
      ${MyMonsterWar_ENABLE_SANITIZER_UNDEFINED}
      ${MyMonsterWar_ENABLE_SANITIZER_THREAD}
      ${MyMonsterWar_ENABLE_SANITIZER_MEMORY})
  endif()

  set_target_properties(MyMonsterWar_options PROPERTIES UNITY_BUILD ${MyMonsterWar_ENABLE_UNITY_BUILD})

  if(MyMonsterWar_ENABLE_PCH)
    target_precompile_headers(
      MyMonsterWar_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(MyMonsterWar_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    MyMonsterWar_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(MyMonsterWar_ENABLE_CLANG_TIDY)
    MyMonsterWar_enable_clang_tidy(MyMonsterWar_options ${MyMonsterWar_WARNINGS_AS_ERRORS})
  endif()

  if(MyMonsterWar_ENABLE_CPPCHECK)
    MyMonsterWar_enable_cppcheck(${MyMonsterWar_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(MyMonsterWar_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    MyMonsterWar_enable_coverage(MyMonsterWar_options)
  endif()

  if(MyMonsterWar_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(MyMonsterWar_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(MyMonsterWar_ENABLE_HARDENING AND NOT MyMonsterWar_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR MyMonsterWar_ENABLE_SANITIZER_UNDEFINED
       OR MyMonsterWar_ENABLE_SANITIZER_ADDRESS
       OR MyMonsterWar_ENABLE_SANITIZER_THREAD
       OR MyMonsterWar_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    MyMonsterWar_enable_hardening(MyMonsterWar_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
