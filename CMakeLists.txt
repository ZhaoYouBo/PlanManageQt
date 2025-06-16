cmake_minimum_required(VERSION 3.19)
project(PlanManageQt LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql Charts)

qt_standard_project_setup()

qt_add_executable(PlanManageQt
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    database.h database.cpp
    resource.rc
    addtaskdialog.h addtaskdialog.cpp addtaskdialog.ui
    datedelegate.h datedelegate.cpp
    taskstatusdelegate.h taskstatusdelegate.cpp
    utils.h utils.cpp
    addhabitdialog.h addhabitdialog.cpp addhabitdialog.ui
    habitstatusdelegate.h habitstatusdelegate.cpp
    habitfrequencydelegate.h habitfrequencydelegate.cpp
    planstatusdelegate.h planstatusdelegate.cpp
    plannamedelegate.h plannamedelegate.cpp
)

target_link_libraries(PlanManageQt
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Sql
        Qt6::Charts
)

include(GNUInstallDirs)

install(TARGETS PlanManageQt
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET PlanManageQt
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
