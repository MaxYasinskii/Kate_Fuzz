# Hasn't been ported to Windows yet.
if(WIN32)
  return()
endif()

kate_add_plugin(kategdbplugin)
target_compile_definitions(kategdbplugin PRIVATE TRANSLATION_DOMAIN="kategdbplugin")
target_link_libraries(kategdbplugin PRIVATE kateprivate KF6::I18n KF6::TextEditor)

include(ECMQtDeclareLoggingCategory)
ecm_qt_declare_logging_category(
    DEBUG_SOURCES
    HEADER dapclient_debug.h
    IDENTIFIER DAPCLIENT
    CATEGORY_NAME "kategdbplugin"
)

ki18n_wrap_ui(kategdbplugin advanced_settings.ui)
ki18n_wrap_ui(kategdbplugin debugconfig.ui)

set(DAP_SOURCES
    dap/bus.cpp
    dap/socketbus.cpp
    dap/processbus.cpp
    dap/client.cpp
    dap/entities.cpp
    dap/messages.h
    dap/settings.cpp
    dap/socketprocessbus.cpp
    dap/bus_selector.cpp
)

set(GDBMI_SOURCES
    gdbmi/parser.cpp
    gdbmi/records.h
    gdbmi/tokens.cpp
)

target_sources(
  kategdbplugin
  PRIVATE
    plugin_kategdb.cpp
    backendinterface.cpp
    gdbbackend.cpp
    configview.cpp
    ioview.cpp
    localsview.cpp
    advanced_settings.cpp
    debugconfigpage.cpp

    ${DAP_SOURCES}
    ${GDBMI_SOURCES}
    ${DEBUG_SOURCES}
    json_placeholders.cpp
    dapbackend.cpp
    gdbvariableparser.cpp
    backend.cpp

    plugin.qrc 
)

if (BUILD_PCH)
    target_precompile_headers(kategdbplugin REUSE_FROM katepch)
endif()

if(BUILD_TESTING)
  add_subdirectory(autotests)
endif()
