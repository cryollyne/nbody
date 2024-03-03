add_executable(nbody)

target_sources(nbody PRIVATE
    main.cpp
    canvas.cpp canvas.h
    simulator.cpp
    resources.qrc
)

find_package(glm REQUIRED)

target_link_libraries(nbody
    glm::glm
    Qt${QT_MAJOR_VERSION}::Quick
    Qt${QT_MAJOR_VERSION}::Qml
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::QuickControls2
    Qt${QT_MAJOR_VERSION}::Widgets
    KF${QT_MAJOR_VERSION}::I18n
)

install(TARGETS nbody ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
