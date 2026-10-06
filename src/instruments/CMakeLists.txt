include_directories(${CMAKE_SOURCE_DIR}/libs/asio/include/)

add_executable(bridgecommand-id
    main.cpp
    ../IniFile.cpp
    ../InstrumentData.cpp
    ../InstrumentNetworkCodec.cpp
    ../Utilities.cpp
    EventReceiver.cpp
    InstrumentGUI.cpp
    Network.cpp
)

set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)

if (WIN32)
target_link_libraries(bridgecommand-id
    bc-asio
    enet64
    irrlicht
    Threads::Threads
    winmm
    ws2_32
)
else ()
target_link_libraries(bridgecommand-id
    bc-asio
    enet
    bc-irrlicht
    Threads::Threads
)
endif ()
