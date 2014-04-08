#!/bin/bash
# verzeichnis cd otool
EXECFILES="libcalculator_pi.dylib libcelestial_navigation_pi.dylib libdashboard_pi.dylib libfindit_pi.dylib libgradar_pi.dylib libgrib_pi.dylib libiacfleet_pi.dylib liblogbookkonni_pi.dylib libvdr_pi.dylib libwatchman_pi.dylib libweather_routing_pi.dylib libweatherfax_pi.dylib libwmm_pi.dylib libs63_pi.dylib libtrimplot_pi.dylib OpenCPN"
LIBPATH="../Frameworks"
NEWLIBPATH="@executable_path/../Frameworks"

TARGETS="libwx_baseu_net-3.1.dylib libwx_baseu_xml-3.1.dylib libwx_baseu-3.1.dylib libwx_osx_cocoau_adv-3.1.dylib libwx_osx_cocoau_aui-3.1.dylib libwx_osx_cocoau_core-3.1.dylib libwx_osx_cocoau_gl-3.1.dylib libwx_osx_cocoau_html-3.1.dylib libwx_osx_cocoau_xrc-3.1.dylib
libwx_osx_cocoau_qa-3.1.dylib libwx_osx_cocoau_webview-3.1.dylib"
for EXECFILE in ${EXECFILES} ; do
for TARGET in ${TARGETS} ; do
#TARGETID=`otool -DX ${LIBPATH}/$TARGET`
NEWTARGETID=${NEWLIBPATH}/${TARGET}

install_name_tool -change /usr/local/lib/${TARGET} ${NEWTARGETID} ${EXECFILE}
# klartext: ... /usr/local/lib/libwx_baseu_net-3.1.dylib @executable_path/../Frameworks/libwx_baseu_net-3.1.dylib libcalculator_pi.dylib

done
done

echo "Erledigt"
