cmake_minimum_required(VERSION 3.5)

if (TARGET ocpn::nmea0183dtactics)
    return ()
endif ()

if (NOT CMAKE_MODULE_PATH)
  set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)
endif ()

SET(SRC
    src/LatLong.hpp
    src/latlong.cpp
    src/long.cpp
    src/nmea0183.cpp
    src/nmea0183.hpp
    src/Response.hpp
    src/response.cpp
    src/RMB.hpp
    src/rmb.cpp
    src/Sentence.hpp
    src/sentence.cpp
    src/talkerid.cpp
    src/RMC.HPP
    src/rmc.cpp
    src/hexvalue.cpp
    src/lat.cpp
    src/expid.cpp
    src/wpl.hpp
    src/wpl.cpp
    src/rte.hpp
    src/rte.cpp
    src/hdt.hpp
    src/hdt.cpp
    src/hdg.hpp
    src/hdg.cpp
    src/hdm.hpp
    src/hdm.cpp
    src/gll.hpp
    src/gll.cpp
    src/vtg.hpp
    src/vtg.cpp
    src/gga.hpp
    src/gga.cpp
    src/gsv.hpp
    src/gsv.cpp
#    src/GPwpl.cpp  # Sentences that opencpn-lib has
#    src/GPwpl.hpp
#    src/apb.cpp
#    src/apb.hpp

    src/xte.cpp
    src/xte.hpp
    src/mwd.cpp
    src/mwd.hpp
    src/mwv.cpp
    src/mwv.hpp
    src/dbt.cpp
    src/dbt.hpp
    src/dpt.cpp
    src/dpt.hpp
    src/mta.hpp
    src/mda.hpp
    src/mtw.hpp
    src/rsa.hpp
    src/vhw.hpp
    src/vlw.hpp
    src/vwr.hpp
    src/vwt.hpp
    src/xdr.hpp
    src/zda.hpp
    src/mta.cpp
    src/mda.cpp
    src/mtw.cpp
    src/rsa.cpp
    src/vhw.cpp
    src/vlw.cpp
    src/vwr.cpp
    src/vwt.cpp
    src/xdr.cpp		   
    src/zda.cpp
# Not in opencpn-lib
    src/SatInfo.h
	)

if (NOT QT_ANDROID)
if (NOT wxWidgets_INCLUDE_DIRS)
    include(SimpleWxConfig)
    SimpleWxConfig()
endif ()
endif ()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(NMEA0183DTACTICS STATIC ${SRC})
add_library(ocpn::nmea0183dtactics ALIAS NMEA0183DTACTICS)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  set_property(TARGET NMEA0183DTACTICS PROPERTY COMPILE_FLAGS "-fvisibility=hidden")
endif ()
target_include_directories(NMEA0183DTACTICS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(NMEA0183DTACTICS PRIVATE ${wxWidgets_INCLUDE_DIRS})
