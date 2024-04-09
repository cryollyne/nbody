add_executable(nbody)

set_property(TARGET nbody PROPERTY CXX_STANDARD 17)

target_sources(nbody PRIVATE
    main.cpp
    canvas.cpp canvas.h simulation_object_ui_data_type.h
    backend.cpp backend.h
    dynamic_buffer.cpp dynamic_buffer.h
    simulation_data.cpp simulation_data.h
    resources.qrc
)

target_link_libraries(nbody
    Qt${QT_MAJOR_VERSION}::Quick
    Qt${QT_MAJOR_VERSION}::Qml
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::QuickControls2
    Qt${QT_MAJOR_VERSION}::Widgets
    KF${QT_MAJOR_VERSION}::I18n
)

install(TARGETS nbody ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(DIRECTORY shaders/ DESTINATION ${SHARE_INSTALL_PREFIX}/nbody)
