Derived from Dave Register's GRADAR plugin

BR24radar_pi
=========
Navico Broadband Radar Plugin for OpenCPN
=========================================
Compiling
=========
You have to be able to compile OpenCPN itself - Get the info at http://opencpn.org/ocpn/developers_manual

* go to the plugin subdirectory in your OpenCPN source tree
* git clone git://github.com/canboat/BR24radar_pi.git

Build as normally:

* cd ..
* cd build
* cmake ..
* cmake --build .

License
=======
The plugin code is licensed under the terms of the GPL v2 or, at your will, later.

Patch???
void br24radar_pi::SetRejectionMode(int mode)
{
    settings.rejection = mode;
    if (settings.master_mode) {
        char br_rejection_cmd[] = {
            (byte)0x08,
            (byte)0xc1,     //(byte)0x0c, Always c1 for transmit?
            //0, 0, 0, 0, 0, 0, 0, 0, //Interference rejection is 1 byte according to paper? e.g. 08 c1 00 [01, 02, 03]
            (char) settings.rejection
        };
        TransmitCmd(br_rejection_cmd, sizeof(br_rejection_cmd));
        wxString msg;
        msg.Printf(wxT("Rejection mode: %o"), settings.rejection) ; //br_rejection_cmd[]); Gives a more understandable message?
        if (Log) wxLogMessage(msg);
    }

