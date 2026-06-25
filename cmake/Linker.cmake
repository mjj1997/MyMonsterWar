macro(MyMonsterWar_configure_linker project_name)
  set(MyMonsterWar_USER_LINKER_OPTION
    "DEFAULT"
      CACHE STRING "Linker to be used")
    set(MyMonsterWar_USER_LINKER_OPTION_VALUES "DEFAULT" "SYSTEM" "LLD" "GOLD" "BFD" "MOLD" "SOLD" "APPLE_CLASSIC" "MSVC")
  set_property(CACHE MyMonsterWar_USER_LINKER_OPTION PROPERTY STRINGS ${MyMonsterWar_USER_LINKER_OPTION_VALUES})
  list(
    FIND
    MyMonsterWar_USER_LINKER_OPTION_VALUES
    ${MyMonsterWar_USER_LINKER_OPTION}
    MyMonsterWar_USER_LINKER_OPTION_INDEX)

  if(${MyMonsterWar_USER_LINKER_OPTION_INDEX} EQUAL -1)
    message(
      STATUS
        "Using custom linker: '${MyMonsterWar_USER_LINKER_OPTION}', explicitly supported entries are ${MyMonsterWar_USER_LINKER_OPTION_VALUES}")
  endif()

  set_target_properties(${project_name} PROPERTIES LINKER_TYPE "${MyMonsterWar_USER_LINKER_OPTION}")
endmacro()
