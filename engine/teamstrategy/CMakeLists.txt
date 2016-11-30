add_library(TeamStrategy STATIC
    order/orderfactory.cpp

    dumbteamstrategy.cpp
    dumbteamstrategy.h
    managedteamstrategy.cpp
    managedteamstrategy.h
    teamstrategy.cpp
    teamstrategy.h
    teamstrategybuilder.cpp
    teamstrategybuilder.h
    teamstrategyrobocup.cpp
    teamstrategyrobocup.h
    teamstrategyhelper.cpp
    teamstrategyhelper.h
    teamstrategyloader.cpp
    teamstrategyloader.h
    strikerBehaviour.cpp
    strikerBehaviour.h
)

target_link_libraries(TeamStrategy
    nullmq
    naomessages)

target_include_directories(TeamStrategy
  PUBLIC
    .
    order
  PRIVATE
    ../../includes/firmware/worldmodel
    ../../includes/firmware/vision_primitives
    ../../includes/3rdparty/spl
    ../../includes/firmware/gamecontroller
    ../../includes/firmware/include
    ../../includes/include)
    
# All those PRIVATE include directories actually shouldn't be here.
# They should come from dependent packages.
