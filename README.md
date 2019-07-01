# XInputDisabler
QT 5.9 app that launches a trayicon you can use to disable an Xinput device.

You simply start the app, open the settings by right clicking the TrayIcon,
choose the xinput device you wish to bind to the app, press the "Set Device" button,
close the window (optional) and press the icon to disable or enable the device.

# But why?
My 2 in 1 laptop comes with a touchscreen which I sometimes want to disable,
so only the wacom pen is active (or when using Blender I can wipe dust or dirt off).

I originally wrote a script in python but found it tedious that I couldn't do
it in 1 click, at first I wanted to write a gnome extension for it but figured
QT must have some kind of trayicon support which would be compatible with other DE's.
Which lead to this app which is a bit more than the use case.

Code is not really optimal, in hindsight the trayicon could have been a seperate
class that spawns a MainWindow with all the necesarry code inside the TrayIcon
class, deleting the MainWindow if not needed. However this would complicate on
how to attain the XInput list (could pass it in the MainWindow constructor).

# Disclaimer
Icons are from [iconarchive](http://www.iconarchive.com/)
