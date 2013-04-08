
 Druid4Arduino Copyright (C) 2013 Pat Deegan, psychogenic.com
 http://flyingcarsandstuff.com/projects/Druid4Arduino/


The make files here have been used to build Druid4Arduino under Linux, for both Linux and Windows targets.  


*** Requirements ***
You need to have the Boost libraries (http://boost.org/), 
	wxWidgets (http://wxwidgets.org/) and
	SerialUI source (http://www.flyingcarsandstuff.com/projects/serialui/)
to build the program.


*** LINUX ***

If you are under Linux, it's simply a matter of running

$ make -f makefiles/Makefile.linux

to get a Linux program built (you'll need to have SerialUI, Boost and wxWidgets to compile it).

If you want to cross-compile, under Linux, for Windows see 
	http://flyingcarsandstuff.com/2013/03/adventures-of-a-linux-guy-building-windows-programs/
and use the appropriate makefile.


*** Windows ***

If you're building under windows using one of the Unix-like environments (cygwin, MinGW), the 
regular makefiles should work.

If you want to use MSVC or something Windows-centric... please do so, and send me the project
file(s) or whatever.  I'm clueless in this regard, and would appreciate the makefiles and such.


*** Apple/Mac ***

In theory, everything here is cross-platform.  I've no idea how to proceed under Mac either, so
any Xcode project files or instructions would be appreciated.



