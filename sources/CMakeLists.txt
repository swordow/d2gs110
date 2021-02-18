project(d2gs C)
#add_subdirectory(cJSON)
add_executable(
    d2gs
    bnethash.c
    bnethash.h
    bn_types.h
    callback.c
    callback.h
    charlist.c
    charlist.h
    config.c
    config.h
    connection.h
    d2cs_d2gs_character.h
    d2cs_d2gs_protocol.h 
    d2dbs_d2gs_protocol.h
    d2gamelist.c
    d2gamelist.h
    d2ge.c
    d2ge.h
    d2gs.h
    debug.c
    debug.h
    eventlog.c
    eventlog.h
    handle_s2s.c
    handle_s2s.h
    hexdump.c
    hexdump.h
    list.h
    main.c
    net.c
    net.h
    psapi.h
    res/d2server.aps
    res/D2Server.ico
    res/d2server.rc
    res/resource.h
    d2gelib/d2server.h
    d2gelib/colorcode.h
    telnetd.c
    telnetd.h
    timer.c
    timer.h
    utils.c
    utils.h
    vars.c
    vars.h
    versioncheck.c
    versioncheck.h
    d2server.c
)
if(WIN32)
    target_link_libraries(d2gs wsock32 ws2_32 ${CMAKE_CURRENT_SOURCE_DIR}/d2gelib/d2server.lib)
    target_compile_definitions(d2gs PUBLIC -D_CRT_SECURE_NO_WARNINGS -DD2GS -DDEBUG_ON_CONSOLE -DDEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4996 /wd4819")
endif()
set_target_properties(d2gs 
PROPERTIES 
RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../bin"
RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../bin"
RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/../bin"
RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_CURRENT_SOURCE_DIR}/../bin"
)

