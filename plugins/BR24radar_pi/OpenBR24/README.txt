
OpenBR24 - http://www.roboat.at/technologie/radar/

OpenBR24 is a sample implementation of the Lowrance Navico BR24 network 
protocol as described in our paper. A (complete) preprint version is available at
http://www.cci.dmu.ac.uk/administrator/components/com_jresearch/assets/publications/1326903964.pdf
The final publication is available in print (Robotic Sailing, 2011,
ISBN 978-3-642-22835-3) and online at http://www.springerlink.com/content/m7n253u08u3h5u67/ 
  
@incollection {springerlink:10.1007/978-3-642-22836-0_12,
   author = {Dabrowski, Adrian and Busch, Sebastian and Stelzer, Roland},
   title = {A Digital Interface for Imagery and Control of a Navico/Lowrance Broadband Radar},
   booktitle = {Robotic Sailing},
   editor = {Schlaefer, Alexander and Blaurock, Ole},
   publisher = {Springer Berlin Heidelberg},
   isbn = {978-3-642-22836-0},
   keyword = {Engineering},
   pages = {169-181},
   url = {http://dx.doi.org/10.1007/978-3-642-22836-0_12},
   note = {10.1007/978-3-642-22836-0_12},
   year = {2011}
}  
  
  
= LEGAL =
 
 * Copyright (C) 2011 Adrian Dabrowski, Sebastian Busch
 * 
 * OpenBR24 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * OpenBR24 is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenBR24; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 
 This software makes use of the jnetpcap-Library and IpReassemblyExample 
 both licensed under LGPL.
 
 
= USAGE =

Start the GUI class. You can choose to open a live radar ("Open Live")
device or an recording (Open File). Recordings are read from PCAP-files. 
To make your own recordings use wireshark or tcpdump.

If your open a live device, you will have to turn it on, using the "On"
button. You can then choose the range of the radar by selecting one 
from the listbox. In playback mode the range is defined by the 
recorded data.


= INSTALLATION =

OpenBR24 requires jnetpcap library which requires libpcap. If you are
using Linux you might need to make a few adjustments, in case you get
java.lang.UnsatisfiedLinkError exceptions:

1) Ubuntu (and probably other) comes with libpcap.so.1.1.1, whereas 
jnetpcap has bindings to libpcap.so.0.9.  You can fix that by making
a smbolic link as root:
	ln -s /usr/lib/libpcap.so.1.1.1 /usr/lib/libpcap.so.0.9	 

2) Native Java bindings depend on the platform you are using. Java
automatically distinguish between Win32 and Linux by choosing the 
appropriate .dll/.so file, but fails to distinguish automatically between 
32bit and 64bit systems. You might have to copy/symlink the appropriate
file in the $PROJECT/lib/jnetpcap/ directory.

3) Ubuntu (and probably other) require to add a route to the multicast
network on the network adapter connected to the radar antenna unit (e.g.
sudo route add -net 224.0.0.0/4 dev eth0).

= DEVELOPMENT =

The GUI has been build with help of the Eclipse Visual Editor (VE) 
extension. VE has been abandoned in favor of the new WindowBuilder 
by Google, which seems to work with the the old VE code.  


= CONTACT =
Adrian Dabrowski, atrox/at/innoc/dot/at
Sebastian Bush, mail/at/sebastianbusch/dot/at
Roland Stelzer, roland/at/innoc/dot/at

