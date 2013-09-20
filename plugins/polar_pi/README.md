polar_pi
========

Polar-Diagram-Plugin for OpenCPN


Building from source
--------------------
Please use Git for download and don't hit the downloads-link above to create a .zip or tar.zip !!

Clone the git repository to <OPENCPN_SOURCE_TLD>/plugins/polar_pi (the _pi at the end matters!) as follows:

cd OPENCPN_SOURCE_TLD/plugins

OPENCPN_SOURCE_TLD = where you stored the opencpn-source files e.g. ~/home/username/path_to_opencpn-source

open a terminal here and copy/paste this command:
git clone git://github.com/konnibe/polar_pi.git polar_pi

Run this commands:

cd ../build

cmake ..

cmake --build . --target polar_pi

sudo make install

that's it.
