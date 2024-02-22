cmake_minimum_required(VERSION 3.16)
project(nbody)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt${QT_MAJOR_VERSION} REQUIRED NO_MODULE COMPONENTS
    Core
    Quick
    Test
    Gui
    QuickControls2
    Widgets
)

find_package(KF${QT_MAJOR_VERSION} REQUIRED COMPONENTS
    Kirigami2
    CoreAddons
    I18n
)



add_subdirectory(src)

