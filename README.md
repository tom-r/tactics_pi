TACTICS Plugin for OpenCPN
==============================
a performance enhancement of dashboard_pi

For detailed description / manual see tactics_pi.odt and tactics_pi.pdf

In short (taken from the manual):

What tactics_pi can do :
------------------------
* Calculate true wind data : TWA, TWD, TWS from true heading (HDT), speed
  through water (STW) and app. Wind speed (AWS), with optional correction by heel-angle.
  Calculation is enabled via a preference setting and disables available true wind
  data from the bus throughout the tactics_pi plugin. 
* Calculate the “leeway”, the boat drift based on heel.
  A common formula is used for that purpose.
* Calculate the surface sea current and display it as single instruments
  (current speed/direction) as part of the “Bearing compass” or as overlay on the
  chart (semi transparent). The routines take boat heel and leeway into account. If you
  don't have a heel sensor, there is a simply workaround, see below. Current display on the
  chart can be disabled by a preference setting.
* Calculate and display the boat laylines for the current tack, and the same AWA on the other tack.
  Sea current is taken into account, if available ! Laylines may be toggled on/off. Boat forward
  direction and for the same AWA on the other tack. Adjustable length and max. width (triangle,
  with one corner at the boat). The width of the tip shows the boat's yawing (COG changes over time).
* You can load a polar file and calculate/display performance data, like
  - VMG (velocity made good up-/downwind),
  - Target-VMG,
  - Target-TWA  (the opt. TWA up-/downwind),
  - CMG AKA VMC (course made good towards a waypoint),
  - Target-CMG (opt. CMG angle and speed to approach fastest to a WP),
  - polar speed (the speed you should be able to sail at current TWA/TWS based on your polar),
  ...
* you can set a (one !) temporary tactics waypoint and display the laylines to the mark, based on a
  Target-TWA calculation, while taking your polar into account.
* it has a “dial instruments” which I called “Bearing compass”. Boat true heading (HDT)  points “up”,
  it shows the boat laylines as well, the surface current, a pointer to the waypoint (either set manually
  as the temporary Tactics waypoint or read from a NMEA RMB sentence), AWA, TWA and a marker for the Target-TWA.
  It also shows pointers (needles) for AWA and TWA.
* If you use NKE instruments, the can create specific NMEA183 performance records with the purpose to upload the data
  to the system bus and show it on the display of your instruments.
  You can now e.g. calculate the 'polar target speed' in the plugin and send it to your instrument display
  outside in the cockpit. Currently only available for NKE, but may be enhanced in the future.
* There are various settings, which I grouped in a separate tab. To access the preferences screen, right mouse click
  on the tactics_pi window, then select “Preferences ...”

Prerequisites :
---------------
* You will need to activate Open GL, if you want to use the chart based functions.
* SOG, COG from the GPS
* preferably True Heading from an electronic compass.
  --> if not available, magnetic heading will do, as long as you have magnetic variance available (either from the GPS RMC
  sentence or from the wmm_pi plugin)
* Boat speed through water from a log / “paddlewheel” sensor
* Apparent wind angle and apparent wind speed
* Heel sensor which supplies your boat heel angle to O as XDR sentence
  --> If not available, there's a workaround with manual input
* You need a polar file of your boat to use all polar based performance calculations
* Calibrate AWA, Compass HDG/HDT,  STW (Speed through water), and AWS (apparent wind speed) as good as possible.
  Especially the compass heading calibration tends to be neglected. But this is vital for proper surface current calculation.
  All I can say about calibration is :   shit in – shit out …   ;-)

Compiling
=========

* git clone git://github.com/tom-r/tactics_pi.git

Under windows, you must find the file "opencpn.lib" (Visual Studio)  which is built in the build directory after compiling opencpn. 
This file must be copied to the plugins build directory.

Windows build :
---------------
cd ..
cd build
cmake  -T v120_xp ..
cmake --build . --config release
cpack

cpack requires NSIS to make an executable.

Unix style build :
------------------
* cd ..
* cd build
* cmake ..
* make
* make install


License
=======
The plugin code is licensed under the terms of the GPL v3 or, at your will, later.

