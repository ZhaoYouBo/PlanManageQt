cmake_minimum_required(VERSION 3.19)
project(PlanManageQt LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql Charts)

qt_standard_project_setup()

set(CMAKE_AUTORCC ON)

qt_add_executable(PlanManageQt
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    database.h database.cpp
    addtaskdialog.h addtaskdialog.cpp addtaskdialog.ui


    utils.h utils.cpp
    addhabitdialog.h addhabitdialog.cpp addhabitdialog.ui




    resources.qrc
    photosurface.rc
    models/taskmodel.h models/taskmodel.cpp
    models/habitmodel.h models/habitmodel.cpp
    models/planmodel.h models/planmodel.cpp
    delegates/habitfrequencydelegate.h delegates/habitfrequencydelegate.cpp
    delegates/datedelegate.h delegates/datedelegate.cpp
    delegates/habitstatusdelegate.h delegates/habitstatusdelegate.cpp
    delegates/plannamedelegate.h delegates/plannamedelegate.cpp
    delegates/planstatusdelegate.h delegates/planstatusdelegate.cpp
    delegates/taskstatusdelegate.h delegates/taskstatusdelegate.cpp

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
