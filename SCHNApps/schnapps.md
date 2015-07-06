
SCHNApps is an extensible application based on CGoGN. It provides a multi-view interface and a dynamic plugin system.
Each plugin can provide different features such as files loading or saving, rendering or objects processing.

____________________________________________________________________________________

Interface
=========

SCHNApps window has classic configuration:
- menus on top of window (top of screen on mac)
- views at the center
- control dock on the left
- plugin dock on the right
- optional python dock console at bottom
- status bar (for messages that stay 2 seconds) at bottom of window

The docks can be docked in the main window or in a floating window (classic Qt dock).

____________________________________________________________________________________

Menu
----
The file menu has current sub-menus
- show/hide ControlDock
- show/hide PluginDock
- show/hide PythonDock
- load Python Script : load and execute a python script
- Python Recording (menu change to stop when used): all actions are captured in a script file
- Python append (menu change to stop when used): same but append at end of file
- Clean All: remove all except one view and one camera. 

Each plugin can add a menu/sub-menu.

____________________________________________________________________________________

Views
-----
Each view:
- has a name (view_0, view_1, ...)
- has a default camera
- can be split to create a new view.

With icons on each view you can choose:
- the set of maps which can be drawn with plugins (green icon)
- the set of plugins which apply on this view (purple icon)
- the camera used in the view (green icon)

There is a current selected view (green framed). The name of view appear on the status bar
when selecting and when clicking on top of view.

____________________________________________________________________________________

The control dock
----------------
The control dock contains 3 tabs:
- Camera tab : managing cameras
	+ list of cameras
	+ kind of projection
	+ draw/path (do not use !)
- MapHandlerGen tab : managing maps 
	+ list of maps
	+ which vertex attribute is used for boundinb-box computing
	+ remove map button 
	+ duplicate map button
	+ list of attributes for each orbit. Check means VBO creation (only for vertex attributes). VBOs are used by rendering plugins.
	+ list of cell selector. Store selected cells (see surface_selection_plugin)
- Plugins tab: loading/unloading plugins
	+ list of available plugins for loading
	+ list of loaded plugins
	+ up/down arrow icons to load/unload plugins

____________________________________________________________________________________

The plugin dock
---------------
The plugin doc appear on the right when one interaction-plugin is loaded.
A vertical tab is added in the doc for each interaction-plugin. The tab can
be disabled (grey shaded) if plugin is not used by current view.

____________________________________________________________________________________

The python console dock
-----------------------
The python console allow to call all bound methods of some objects:
- schnapps: the application itself
- view
- Camera
- MapHandlerGenandler
- all plugins 

See each class description for details.

____________________________________________________________________________________

Python scripting
----------------
When launching SCHNApps you get an empty application. You have to load some plugins before every thing else.
As repeating each loading at launch can be boring, almost all actions are callable from python scripting 
console or by loading a python script file (by menu or as parameter when launching SCHNApps).

It is also possible to generate a python script by capturing a sequence of action (Menu/File/Python Recording).
To stop recording go again in the menu. The Append menu append action at end of current file.
At end of recording windows size, view split positions and camera are saved (and restored when loading).
Captured script can be used directly, but can easily be customized.

