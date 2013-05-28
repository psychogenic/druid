druid
=====

Druid4Arduino: a universal GUI for arduino-based devices


 Druid4Arduino Copyright (C) 2013 Pat Deegan, psychogenic.com
 http://flyingcarsandstuff.com/projects/Druid4Arduino/

 Released under the GNU GPL v3--see LICENSE.txt for details.

Overview
========

This program takes advantage of the SerialUI program mode to provide a dynamically reconfigured graphical user interface for Arduino projects.

Druid4Arduino determines the available commands, menus and sub-menus, and creates a system of menus and buttons that reflect those on the device.

This is a preliminary release, more details available soon.


Requirements
============
To compile this program, you'll need to have a number of dependencies installed.  You'll need:

	* The boost libraries (http://www.boost.org)
	* WxWidgets (http://www.wxwidgets.org/)
	* The SerialUI source code (http://flyingcarsandstuff.com/projects/serialui/)

You'll probably be able to get the first two as pre-compiled packages, for your Linux distro, though I had some trouble getting things working with CygWin under Windows (got it working under linux though, which is how I compiled the Win versions--see http://flyingcarsandstuff.com/2013/03/adventures-of-a-linux-guy-building-windows-programs/)

The makefile is configured to search for the SerialUI includes in the same parent directory as the source you're building here (in ../SerialUI/includes) -- you can dump the source there, or change the makefile.

Enjoy, get in touch if you have any issues (http://flyingcarsandstuff.com/contact/)

Regards,
Pat Deegan

