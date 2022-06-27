PSCMScheduler README
---------------------
Version: 1.0

Copyright (C) 2022 Julian Luetkemeyer

Repository
-----------
<a href="https://github.com/Ltkmyr/PSCMScheduler.git" target="_blank">PSCMScheduler</a>

License
-------
See the file 'LICENSE'.
 
Description
-----------
PSCMScheduler is a tool for solving various scheduling problems in production and supply chain management.
There are some predefined scheduling problems (see the folders 'models' and 'data') which can be solved with 
the GLPK standalone solver glpsol. It is also possible to register your own solvers as binaries via the 
CONFIG file (see the file 'CONFIG'). If you do this, it is recommended to provide a solution file and a 
.chart file as output (take a closer look at the different model files in the folder 'models' to get an idea 
of how to create a .chart file in xml format). In this way, the results can be displayed and plotted in the
form of a Gantt chart.

Installation
------------
This software uses <a href="https://www.wxwidgets.org/" target="_blank">wxWidgets</a> and 
<a href="http://wxcode.sourceforge.net/components/freechart/" target="_blank">wxFreeChart</a>.
You have to build it against your default wxWidgets installation (see the file 'CMakeLists.txt').
After the binary has been build, place it in the same directory as the CONFIG file.

Changelog
---------
1.0 - First version of PSCMScheduler

Future plans
------------
- Add zoom to plots 
- Add plot window scrolling
- Add further chart types, e.g. resource utilization
 
Known issues
------------
- If there is only one category on the y-Axis, the gantt bars are not shown correctly
- Allready loaded charts and solutions are not deleted when a solver returnes an unknown exit code