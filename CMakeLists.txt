cmake_minimum_required(VERSION 3.14)
project(DhcpSimulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

set(SOURCES
    main.cpp
    mainwindow.cpp
    protocol/dhcpmessage.cpp
    model/dhcpclient.cpp
    model/leaseinfo.cpp
    model/ipentry.cpp
    service/ippoolmanager.cpp
    service/leasemanager.cpp
    service/dhcpserver.cpp
    service/dhcprelayagent.cpp
    controller/dhcpcontroller.cpp
    controller/statemachine.cpp
    view/iptablewidget.cpp
    view/leasetablewidget.cpp
    view/flowwidget.cpp
)

set(HEADERS
    mainwindow.h
    protocol/dhcpmessage.h
    protocol/discovermessage.h
    protocol/offermessage.h
    protocol/requestmessage.h
    protocol/ackmessage.h
    protocol/renewmessage.h
    model/dhcpclient.h
    model/leaseinfo.h
    model/ipentry.h
    service/ippoolmanager.h
    service/leasemanager.h
    service/dhcpserver.h
    service/dhcprelayagent.h
    controller/dhcpcontroller.h
    controller/statemachine.h
    view/iptablewidget.h
    view/leasetablewidget.h
    view/flowwidget.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Core)
