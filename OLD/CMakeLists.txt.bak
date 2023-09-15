##---------------------------------------------------------------------------
## Author:      Dave Register
## ***************************************************************************
## *   Copyright (C) 2010 by David S. Register                               *
## *   This program is free software; you can redistribute it and/or modify  *
## *   it under the terms of the GNU General Public License as published by  *
## *   the Free Software Foundation; either version 2 of the License, or     *
## *   (at your option) any later version.                                   *
## *                                                                         *
## *   This program is distributed in the hope that it will be useful,       *
## *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
## *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
## *   GNU General Public License for more details.                          *
## *                                                                         *
## *   You should have received a copy of the GNU General Public License     *
## *   along with this program; if not, write to the                         *
## *   Free Software Foundation, Inc.,                                       *
## *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
## ***************************************************************************
#  Frontend2 Author:  ongough aka. Jon Gough Copyright: wxWidgets License
#  Fronted2 Version: 1.0.226.0    17/02/2023 # DD/MM/YYYY format
# ---------------------------------------------------------------------------
## ----- When changing this file do NOT change the order in which sections occur        ----- ##
## ----- Changes should only be made between the section blocks that identify where     ----- ##
## ----- these changes should be. The whole configuration process relies on this        ----- ##
## ----- sequence to be successful                                                      ----- ##
##

##
## ----- Modify section below to include all the details for your plugin ----- ##
##


set(CMLOC "CMakeLists: ")

# define minimum cmake version
cmake_minimum_required(VERSION 3.5.1)
if(COMMAND cmake_policy)
    if(POLICY CMP0043)
        cmake_policy(SET CMP0043 NEW)
    endif(POLICY CMP0043)
    cmake_policy(SET CMP0048 NEW)
# Need cmake 3.13 to implement this, but raspbian builds are at 3.7.2 and bionic at 3.10 and xenial builds at 3.5.1
#    cmake_policy(SET CMP0076 NEW)
    if(POLICY CMP0077)
        cmake_policy(SET CMP0077 NEW)
    endif(POLICY CMP0077)
endif(COMMAND cmake_policy)

# define plugin name, owner and versions
set(VERBOSE_NAME "Tactics")
set(COMMON_NAME "Tactics")
set(TITLE_NAME "TACTICS")
set(PACKAGE_CONTACT "Tom R")
set(PACKAGE "tactics_pi")
set(SHORT_DESCRIPTION "Tactics PlugIn for OpenCPN")
set(LONG_DESCRIPTION "Tactics PlugIn for OpenCPN Provides performance data and instrument display from NMEA source and polar file.")

set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "35")
set(VERSION_TWEAK "0")
set(VERSION_DATE "27/03/2023")  # DD/MM/YYYY format
set(OCPN_MIN_VERSION "ov50")
set(OCPN_API_VERSION_MAJOR "1")
set(OCPN_API_VERSION_MINOR "16")
set(TP_COMMENT "  * Release for O5 using CI")

set(PARENT "opencpn")

# The next line allows for setting up your own local webserver with git on it for testing purposes, the default is github.com
# This has been moved to PluginConfigure. Required to build flatpak and locally.
#set(GIT_REPOSITORY_SERVER "github.com")

# Specifies cloudsmith repository sufficies
set(PROD "prod") # Standard Repos
set(BETA "beta") # Standard Repos
set(ALPHA "alpha") # Standard Repos

# set this if your cloudsmith repository name does not match your git repository name 
# set(CLOUDSMITH_BASE_REPOSITORY "tactics_pi")
set(CLOUDSMITH_BASE_REPOSITORY "tactics") #without the pi
# Set the following to use OpenCPN cloudsmith repositories instead of own repositories
set(CLOUDSMITH_USER "opencpn")
# set(CLOUDSMITH_USER "rick-gleason")


set(XML_INFO_URL "https://opencpn.org/OpenCPN/plugins/tactics.html")
set(XML_SUMMARY ${SHORT_DESCRIPTION})
set(XML_DESCRIPTION ${LONG_DESCRIPTION})

##
## ----- Modify section above to include all the details for your plugin ----- ##
## ----- Modify section below if there are special requirements for the plugin ----- ##
##

option(PLUGIN_USE_SVG "Use SVG graphics" ON)

set(CMAKE_CXX_STANDARD 11)

# Use local version of GLU library  requires libs/glu directory
#try use local glu true
#set(USE_LOCAL_GLU TRUE)
# Prefer libGL.so to libOpenGL.so, see CMP0072
set(OpenGL_GL_PREFERENCE "LEGACY")
# Don't use local version of GLU library
#try true does not help.
set(USE_LOCAL_GLU FALSE)

option(USE_GL "Enable OpenGL support" ON)
message(STATUS "${CMLOC}USE_GL: ${USE_GL}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "" "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
message(STATUS "${CMLOC}Build type: ${CMAKE_BUILD_TYPE}")

##
## ----- The statements below are used to setup standard variables that are required by the CMAKE process - do not remove ----- ##
##

project(${PACKAGE})
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#-------------------------------------------------------
include("PluginSetup")
#-------------------------------------------------------

##
## ----- The above is used to setup standard variables that are required by the CMAKE process - do not remove ----- ##
## ----- Modify section below if there are special requirements for the plugin ----- ##
##

set(CMAKE_VERBOSE_MAKEFILE "Activate verbose mode for make files" ON)

option(Plugin_CXX11 "Use c++11" OFF)

##
## ----- Modify section above if there are special requirements for the plugin ----- ##
## ----- do not change next section - needed to configure build process ----- ##
##

#-------------------------------------------------------
include("PluginConfigure")
#-------------------------------------------------------

##
## ----- do not change section above - needed to configure build process ----- ##
# ----- Change below to match project requirements for source, headers, etc. ----- ##
#

add_definitions(-DUSE_S57)

SET(SRCS
    src/tactics_pi.h
    src/tactics_pi.cpp
	src/avg_wind.h
	src/avg_wind.cpp
    src/bearingcompass.cpp
    src/bearingcompass.h
	src/polarcompass.cpp
    src/polarcompass.h
    src/performance.cpp
    src/performance.h
    src/instrument.cpp
    src/instrument.h
    src/iirfilter.h
    src/iirfilter.cpp
    src/compass.cpp
    src/compass.h
    src/dial.cpp
    src/dial.h
    src/gps.cpp
    src/gps.h
    src/rudder_angle.cpp
    src/rudder_angle.h
    src/speedometer.cpp
    src/speedometer.h
    src/wind.cpp
    src/wind.h
    src/depth.cpp
    src/depth.h
    src/clock.cpp
    src/clock.h
    src/icons.cpp
    src/icons.h
    src/wind_history.cpp
    src/wind_history.h
    src/baro_history.cpp
    src/baro_history.h
    src/from_ownship.cpp
    src/from_ownship.h
    libs/wxJSON/src/jsonval.cpp
    libs/wxJSON/src/jsonreader.cpp
    libs/ocpn-api/ocpn_plugin.h
)


#SET(SRC_NMEA0183
#    src/nmea0183/LatLong.hpp
#    src/nmea0183/latlong.cpp
#    src/nmea0183/long.cpp
#    src/nmea0183/nmea0183.cpp
#    src/nmea0183/nmea0183.hpp
#    src/nmea0183/Response.hpp
#    src/nmea0183/response.cpp
#    src/nmea0183/RMB.hpp
#    src/nmea0183/rmb.cpp
#    src/nmea0183/Sentence.hpp
#    src/nmea0183/sentence.cpp
#    src/nmea0183/talkerid.cpp
#    src/nmea0183/RMC.HPP
#    src/nmea0183/rmc.cpp
#    src/nmea0183/hexvalue.cpp
#    src/nmea0183/lat.cpp
#    src/nmea0183/expid.cpp
#    src/nmea0183/wpl.hpp
#    src/nmea0183/wpl.cpp
#    src/nmea0183/rte.hpp
#    src/nmea0183/rte.cpp
#    src/nmea0183/hdt.hpp
#    src/nmea0183/hdt.cpp
#    src/nmea0183/hdg.hpp
#    src/nmea0183/hdg.cpp
#    src/nmea0183/hdm.hpp
#    src/nmea0183/hdm.cpp
#    src/nmea0183/gll.hpp
#    src/nmea0183/gll.cpp
#    src/nmea0183/vtg.hpp
#    src/nmea0183/vtg.cpp
#    src/nmea0183/gga.hpp
#    src/nmea0183/gga.cpp
#    src/nmea0183/gsv.hpp
#    src/nmea0183/gsv.cpp
#    src/nmea0183/dbt.cpp
#    src/nmea0183/dbt.hpp
#    src/nmea0183/dpt.cpp
#    src/nmea0183/dpt.hpp
#    src/nmea0183/mtw.cpp
#    src/nmea0183/mtw.hpp
#    src/nmea0183/mwv.cpp
#    src/nmea0183/mwv.hpp
#    src/nmea0183/vhw.hpp
#    src/nmea0183/vhw.cpp
#    src/nmea0183/vwr.cpp
#    src/nmea0183/vwr.hpp
#    src/nmea0183/zda.cpp
#    src/nmea0183/zda.hpp
#    src/nmea0183/rsa.hpp
#    src/nmea0183/rsa.cpp
#    src/nmea0183/SatInfo.h
#    src/nmea0183/mwd.cpp
#    src/nmea0183/mwd.hpp
#    src/nmea0183/vwt.cpp
#    src/nmea0183/vwt.hpp
#    src/nmea0183/mta.cpp
#    src/nmea0183/mta.hpp
#    src/nmea0183/vlw.cpp
#    src/nmea0183/vlw.hpp
#    src/nmea0183/mda.cpp
#    src/nmea0183/mda.hpp
#    src/nmea0183/xdr.cpp
#    src/nmea0183/xdr.hpp
#)


#set(OCPNSRC
#    ocpnsrc/cutil.cpp
#    ocpnsrc/TexFont.cpp
#)

#set(OCPNHDRS
#    ocpninclude/cutil.h
#    ocpninclude/ocpn_plugin.h
#    ocpninclude/pluginmanager.h
#    ocpninclude/shaders.h
#    ocpninclude/TexFont.h
#    ocpninclude/vector2D.h
#    libs/ocpn-api/ocpn_plugin.h
#)

#set(EXTSRC
#)

#set(EXTINCLUDE
#    extinclude/ODAPI.h
#    extinclude/ODJSONSchemas.h
#)

set(LIBSSRC libs/wxJSON/src/jsonreader.cpp libs/wxJSON/src/jsonval.cpp libs/wxJSON/src/jsonwriter.cpp)

set(LIBHDRS
    libs/GL/gl.h
    libs/GL/gl_private.h
    libs/GL/glext.h
    libs/GL/glu.h
    libs/wxJSON/include/json_defs.h
    libs/wxJSON/include/jsonreader.h
    libs/wxJSON/include/jsonval.h
    libs/wxJSON/include/jsonwriter.h
)
    

add_definitions(-DPLUGIN_USE_SVG)


#
# ----- If using JSON validation in plugin section below is needed ----- ##
#

# Not doing JSON_Schema_Validator

#if(APPLE)
#    option(PI_JSON_SCHEMA_VALIDATOR "Use JSON Schema validator" OFF)
#else(APPLE)
#    option(PI_JSON_SCHEMA_VALIDATOR "Use JSON Schema validator" ON)
#endif(APPLE)
#
#if(PI_JSON_SCHEMA_VALIDATOR)
#    message(STATUS "${CMLOC}Using JSON Schema validation - warning this changes the 'flavour' of the executable and it may not be compatible with OpenCPN")
#    add_definitions(-DPI_JSON_SCHEMA_VALIDATOR)
#    message(STATUS "${CMLOC}Adding JSON Schema validation")
#
#    set(EXTSRC ${EXTSRC} extsrc/json-schema-validator/json-schema-draft7.json.cpp extsrc/json-schema-validator/json-uri.cpp extsrc/json-schema-validator/json-validator.cpp)
#    set(EXTINCLUDE_DIR ${EXTINCLUDE_DIR} extinclude/nlohmann extinclude)
#    #include_directories(BEFORE ${PROJECT_SOURCE_DIR}/extinclude/json-schema-validator)
#else(PI_JSON_SCHEMA_VALIDATOR)
#    message(STATUS "${CMLOC}Not using JSON Schema validation - warning invalid JSON 'may' cause unexpected crashes")
#endif(PI_JSON_SCHEMA_VALIDATOR)

#
# ----- If using JSON validation in plugin section above is needed ----- ##
#

#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/include)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/ocpninclude)
#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/ocpnsrc/glu/include)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/${EXTINCLUDE_DIR})
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/GL)
#added this
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/glu)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/ocpn-api)
#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/wxJSON/src)																  
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/wxJSON/include)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/nmea0183/src)
#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src/nmea0183)

#
# ----- Change below to match project requirements for android build ----- ##
#

if(QT_ANDROID)
    include_directories( ${CMAKE_SOURCE_DIR}/extinclude/GLES2)
endif(QT_ANDROID)

##
## ----- Change above to match project requirements for android build ----- ##
##

#target_sources(${PACKAGE_NAME} PUBLIC ${SRCS} ${HDRS} ${OCPNSRC} ${EXTSRC} ${LIBSSRC})
#target_sources(${PACKAGE_NAME} PUBLIC ${SRCS} ${SRC_NMEA0183} ${HDRS} ${OCPNSRC} ${EXTSRC}  ${LIBHDRS}  ${LIBSSRC})
target_sources(${PACKAGE_NAME} PUBLIC ${SRCS} ${OCPNSRC} ${EXTSRC} ${LIBSSRC})


if(NOT OCPN_FLATPAK_CONFIG)
    # Build environment not available when flatpak is being configured so following statements will not work
    message(STATUS "${CMLOC}Adding target link libraries to ${PACKAGE_NAME}")

    add_subdirectory(libs/nmea0183)
    add_subdirectory(libs/tinyxml)

    target_link_libraries(${PACKAGE_NAME} ocpn::nmea0183)
    target_link_libraries(${PACKAGE_NAME} ocpn::tinyxml)
	
endif(NOT OCPN_FLATPAK_CONFIG)

add_definitions(-DTIXML_USE_STL)

#
# ----- If using JSON validation in plugin section below is needed ----- ##
#

# Not doing JSON_SCHEMA_VALIDATOR

#
# ----- If using JSON validation in plugin section above is needed ----- ##
#

## ----- Change above to match project requirements ----- ##
## ----- do not change next section - needed to configure build process ----- ##

#-------------------------------------------------------
# Needed for android builds
if(QT_ANDROID)
    include_directories(BEFORE ${qt_android_include})
endif(QT_ANDROID)

# Needed for all builds
include("PluginInstall")
include("PluginLocalization")
include("PluginPackage")
#-------------------------------------------------------

## ----- do not change section above - needed to configure build process ----- ##
##
message(STATUS "${CMLOC}include directories: ")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "${CMLOC}dir='${dir}'")
endforeach()
