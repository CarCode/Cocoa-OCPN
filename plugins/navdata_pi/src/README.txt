# navdata_pi

Is compatible with OpenCPN 5.0.0 (and the current master)

This new version has a completely new design.

The purpose is to show navigation information about a selected intermediate route point in the current active route. This can be useful if you want to
keep an eye on a intermediate point, not visible in the ocpn console,for example a point marking a difficult cap, or an entry in a channel with strong current.

Select-able point: Any point after the current active point.
This means that all points from the start to the current active point(included) are not select-able.

At start the plugin toolbar icon is in waiting state, the plugin is active but the point selection is deactivated.

To use the plugin, you have to click on the icon to activate the selection.
The icon shows you that the selection is activated

Verify you have activated your route.

Now, you can left click on any select-able point of this route.
As a result, you will see a green circle blinking around it and a window looking like the ocpn console will be opened.
This window  shows RNG  (distance), TTG (time to go) and ETA (estimated time of arrival) to this selected point.
As in the ocpn console, you can toggle TTG and ETA computed with SOG or VMG by left click on inside.
This window can be moved and its new position is memorized.

At any moment, you can select another select-able point, but only mean to cancel any selection and close the window is to left click on a no select-able point (for example the current active point).

If you want to lock your selection, click on the plugin icon to deactivate selection.

In any case, when this selected point become the current active point, the selection is canceled and the window is closed.


There is no setting.
The plugin uses ocpn configuration parameters. In general, these parameters apply in live, but it could be necessary to close and reopen ocpn for some changes (language …).

For those who had a look at the previous version, there is no longer neither grid window nor "Trip" data.

installation
============
 to get the binaries, go to:
 
 https://github.com/Ptizef/navdata_pi/releases
 
 then click on the more recent version

 Remark : It is recommended to have OpenCPN coming from a standard official install
            (see opencpn.org/ downloads).
            
*    Windows

        run navdata_pi-x.x-win32.exe 
        
        and select "don't create shortcut" when asked.
        
        **** warning : it was tested on W10 only but should work on any not too old Windows.

*    Linux(Ubuntu and derived)

        run navdata_pi_x.x-x_amd64.deb
        
        **** warning : It was Compiled and tested on a Linux Mint 19.2 and may not work on other version
           
 *   OSX
 
        there is no version for this OS. I have no device to test

 Once installation is done, run ocpn, go to option/plugin and activate navdata_pi        

Compiling
=========
Get the sources
* $ git clone https://github.com/Ptizef/navdata_pi.git

The plugin is compiled as a standalone

* Windows

     prerequisites (Visual studio 2017 only)

     Be sure you are able to compile OpenCPN including building package.
     Eventually read and follow instructions at:
     opencpn.org manual/Developer manual/Developer guide/Compiling on Windows
     
     compile OpenCPN
        
     ** do not forget to copy opecpn.lib from OpenCPN\build\release\opencpn.lib to  navdata_pi\build
        
     then compile navdata_pi and eventually build package

* Linux(Ubuntu and derived)

     prerequisites
    
     the same as to compile OpenCPN.
     Eventually read and follow instructions at:
     opencpn.org manual/ Developer manual/ Developer guide/ Compiling on Linux

*    $ cd sources directory
*    $ mkdir build
*    $ cd build
*    $ cmake ../
*    $ make
*    $ make install
    or
*    $ make package (you may have to install the rpm package)


