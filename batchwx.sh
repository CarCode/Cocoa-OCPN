# install_name_tool -change old new file; 3.1.2 oder 3.1.3 oder 3.1.4 oder 3.1.5 oder 3.1.6 oder 3.1.7 oder 3.2.2 einsetzen (KEIN ...adv...)
# old siehe otool -L file

install_name_tool -change /usr/local/opt/cairo/lib/libcairo.2.dylib @executable_path/../Frameworks/libcairo.2.dylib ../Frameworks/libcairo.2.dylib
install_name_tool -change /usr/local/opt/pixman/lib/libpixman-1.0.dylib @executable_path/../Frameworks/libpixman-1.0.dylib ../Frameworks/libcairo.2.dylib
install_name_tool -change /usr/local/opt/fontconfig/lib/libfontconfig.1.dylib @executable_path/../Frameworks/libfontconfig.1.dylib ../Frameworks/libcairo.2.dylib
install_name_tool -change /usr/local/opt/freetype/lib/libfreetype.6.dylib @executable_path/../Frameworks/libfreetype.6.dylib ../Frameworks/libcairo.2.dylib
install_name_tool -change /usr/local/opt/libpng/lib/libpng16.16.dylib @executable_path/../Frameworks/libpng16.16.dylib ../Frameworks/libcairo.2.dylib
install_name_tool -change /usr/local/opt/libxcb/lib/libxcb-shm.0.dylib @executable_path/../Frameworks/libxcb-shm.0.dylib ../Frameworks/libcairo.2.dylib
install_name_tool -change /usr/local/opt/libxcb/lib/libxcb.1.dylib @executable_path/../Frameworks/libxcb.1.dylib ../Frameworks/libcairo.2.dylib
install_name_tool -change /usr/local/opt/libxcb/lib/libxcb-render.0.dylib @executable_path/../Frameworks/libxcb-render.0.dylib ../Frameworks/libcairo.2.dylib
install_name_tool -change /usr/local/opt/libxrender/lib/libXrender.1.dylib @executable_path/../Frameworks/libXrender.1.dylib ../Frameworks/libcairo.2.dylib
install_name_tool -change /usr/local/opt/libx11/lib/libX11.6.dylib @executable_path/../Frameworks/libXrender.1.dylib ../Frameworks/libcairo.2.dylib
install_name_tool -change /usr/local/opt/libxext/lib/libXext.6.dylib @executable_path/../Frameworks/libXext.6.dylib ../Frameworks/libcairo.2.dylib
echo "Fertig cairo"

install_name_tool -change /usr/local/opt/fontconfig/lib/libfontconfig.1.dylib @executable_path/../Frameworks/libfontconfig.1.dylib ../Frameworks/libfontconfig.1.dylib
install_name_tool -change /usr/local/opt/freetype/lib/libfreetype.6.dylib @executable_path/../Frameworks/libfreetype.6.dylib ../Frameworks/libfontconfig.1.dylib
echo "Fertig fontconfig"

install_name_tool -change /usr/local/opt/freetype/lib/libfreetype.6.dylib @executable_path/../Frameworks/libfreetype.6.dylib ../Frameworks/libfreetype.6.dylib
install_name_tool -change /usr/local/opt/libpng/lib/libpng16.16.dylib @executable_path/../Frameworks/libpng16.16.dylib ../Frameworks/libfreetype.6.dylib
echo "Fertig freetype"

install_name_tool -change /usr/local/opt/xz/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib ../Frameworks/liblzma.5.dylib
echo "Fertig lzma"

install_name_tool -change /usr/local/opt/pixman/lib/libpixman-1.0.dylib @executable_path/../Frameworks/libpixman-1.0.dylib ../Frameworks/libpixman-1.0.dylib
echo "Fertig pixman"

install_name_tool -change /usr/local/opt/libpng/lib/libpng16.16.dylib @executable_path/../Frameworks/libpng16.16.dylib ../Frameworks/libpng16.16.dylib
echo "Fertig libpng"

install_name_tool -change /usr/local/opt/libx11/lib/libX11.6.dylib @executable_path/../Frameworks/libX11.6.dylib ../Frameworks/libX11.6.dylib
install_name_tool -change /usr/local/opt/libxcb/lib/libxcb.1.dylib @executable_path/../Frameworks/libxcb.1.dylib ../Frameworks/libX11.6.dylib
echo "Fertig libX11"

install_name_tool -change /usr/local/opt/libxau/lib/libXau.6.dylib @executable_path/../Frameworks/libXau.6.dylib ../Frameworks/libXau.6.dylib
echo "Fertig libXau"

install_name_tool -change /usr/local/opt/libxcb/lib/libxcb-render.0.dylib @executable_path/../Frameworks/libxcb-render.0.dylib ../Frameworks/libxcb-render.0.dylib
install_name_tool -change /usr/local/Cellar/libxcb/1.14_2/lib/libxcb.1.dylib @executable_path/../Frameworks/libxcb.1.dylib ../Frameworks/libxcb-render.0.dylib
install_name_tool -change /usr/local/opt/libxau/lib/libXau.6.dylib @executable_path/../Frameworks/libXau.6.dylib ../Frameworks/libxcb-render.0.dylib
install_name_tool -change /usr/local/opt/libxdmcp/lib/libXdmcp.6.dylib @executable_path/../Frameworks/libXdmcp.6.dylib ../Frameworks/libxcb-render.0.dylib
echo "Fertig libxcb-render"

install_name_tool -change /usr/local/opt/libxcb/lib/libxcb-shm.0.dylib @executable_path/../Frameworks/libxcb-shm.0.dylib ../Frameworks/libxcb-shm.0.dylib
install_name_tool -change /usr/local/Cellar/libxcb/1.14_2/lib/libxcb.1.dylib @executable_path/../Frameworks/libxcb.1.dylib ../Frameworks/libxcb-shm.0.dylib
install_name_tool -change /usr/local/opt/libxau/lib/libXau.6.dylib @executable_path/../Frameworks/libXau.6.dylib ../Frameworks/libxcb-shm.0.dylib
install_name_tool -change /usr/local/opt/libxdmcp/lib/libXdmcp.6.dylib @executable_path/../Frameworks/libXdmcp.6.dylib ../Frameworks/libxcb-shm.0.dylib
echo "Fertig libxcb-shm"

install_name_tool -change /usr/local/opt/libxcb/lib/libxcb.1.dylib @executable_path/../Frameworks/libxcb.1.dylib ../Frameworks/libxcb.1.dylib
install_name_tool -change /usr/local/opt/libxau/lib/libXau.6.dylib @executable_path/../Frameworks/libXau.6.dylib ../Frameworks/libxcb.1.dylib
install_name_tool -change /usr/local/opt/libxdmcp/lib/libXdmcp.6.dylib @executable_path/../Frameworks/libXdmcp.6.dylib ../Frameworks/libxcb.1.dylib
echo "Fertig libxcb"

install_name_tool -change /usr/local/opt/libxext/lib/libXext.6.dylib @executable_path/../Frameworks/libXext.6.dylib ../Frameworks/libXext.6.dylib
install_name_tool -change /usr/local/opt/libx11/lib/libX11.6.dylib @executable_path/../Frameworks/libX11.6.dylib ../Frameworks/libXext.6.dylib
echo "Fertig libXext"

install_name_tool -change /usr/local/opt/libxrender/lib/libXrender.1.dylib @executable_path/../Frameworks/libXrender.1.dylib ../Frameworks/libXrender.1.dylib
install_name_tool -change /usr/local/opt/libx11/lib/libX11.6.dylib @executable_path/../Frameworks/libX11.6.dylib ../Frameworks/libXrender.1.dylib
echo "Fertig libXrender"

install_name_tool -change /usr/local/opt/libxdmcp/lib/libXdmcp.6.dylib @executable_path/../Frameworks/libXdmcp.6.dylib ../Frameworks/libXdmcp.6.dylib
echo "Fertig libXdmcp"

install_name_tool -change /usr/local/lib/libwx_baseu_net-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu_net-3.2.dylib ../Frameworks/libwx_baseu_net-3.2.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib ../Frameworks/libwx_baseu_net-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.2.dylib ../Frameworks/libwx_baseu_net-3.2.dylib
echo "Fertig wx_baseu_net"

install_name_tool -change /usr/local/lib/libwx_baseu_xml-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu_xml-3.2.dylib ../Frameworks/libwx_baseu_xml-3.2.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib ../Frameworks/libwx_baseu_xml-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.2.dylib ../Frameworks/libwx_baseu_xml-3.2.dylib
echo "Fertig wx_baseu_xml"

install_name_tool -change /usr/local/lib/libwx_baseu-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.2.dylib ../Frameworks/libwx_baseu-3.2.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib ../Frameworks/libwx_baseu-3.2.dylib
echo "Fertig wx_baseu"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_aui-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_aui-3.2.dylib ../Frameworks/libwx_osx_cocoau_aui-3.2.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib ../Frameworks/libwx_osx_cocoau_aui-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.2.dylib ../Frameworks/libwx_osx_cocoau_aui-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.2.dylib ../Frameworks/libwx_osx_cocoau_aui-3.2.dylib
echo "Fertig wx_osx_cocoau_aui"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.2.dylib ../Frameworks/libwx_osx_cocoau_core-3.2.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib ../Frameworks/libwx_osx_cocoau_core-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.2.dylib ../Frameworks/libwx_osx_cocoau_core-3.2.dylib
echo "Fertig wx_osx_cocoau_core"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_gl-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_gl-3.2.dylib ../Frameworks/libwx_osx_cocoau_gl-3.2.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib ../Frameworks/libwx_osx_cocoau_gl-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.2.dylib ../Frameworks/libwx_osx_cocoau_gl-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.2.dylib ../Frameworks/libwx_osx_cocoau_gl-3.2.dylib
echo "Fertig wx_osx_cocoau_gl"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_html-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_html-3.2.dylib ../Frameworks/libwx_osx_cocoau_html-3.2.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib ../Frameworks/libwx_osx_cocoau_html-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.2.dylib ../Frameworks/libwx_osx_cocoau_html-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.2.dylib ../Frameworks/libwx_osx_cocoau_html-3.2.dylib
echo "Fertig wx_osx_cocoau_html"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_qa-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_qa-3.2.dylib ../Frameworks/libwx_osx_cocoau_qa-3.2.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib ../Frameworks/libwx_osx_cocoau_qa-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.2.dylib ../Frameworks/libwx_osx_cocoau_qa-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.2.dylib ../Frameworks/libwx_osx_cocoau_qa-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_baseu_xml-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu_xml-3.2.dylib ../Frameworks/libwx_osx_cocoau_qa-3.2.dylib
echo "Fertig wx_osx_cocoau_qa"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_stc-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_stc-3.2.dylib ../Frameworks/libwx_osx_cocoau_stc-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.2.dylib ../Frameworks/libwx_osx_cocoau_stc-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.2.dylib ../Frameworks/libwx_osx_cocoau_stc-3.2.dylib
echo "Fertig wx_osx_cocoau_stc"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_adv-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_adv-3.2.dylib ../Frameworks/libwx_osx_cocoau_adv-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.2.dylib ../Frameworks/libwx_osx_cocoau_adv-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.2.dylib ../Frameworks/libwx_osx_cocoau_adv-3.2.dylib
echo "Fertig wx_osx_cocoau_adv"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_webview-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_webview-3.2.dylib ../Frameworks/libwx_osx_cocoau_webview-3.2.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib ../Frameworks/libwx_osx_cocoau_webview-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.2.dylib ../Frameworks/libwx_osx_cocoau_webview-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.2.dylib ../Frameworks/libwx_osx_cocoau_webview-3.2.dylib
echo "Fertig wx_osx_cocoau_webview"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_xrc-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_xrc-3.2.dylib ../Frameworks/libwx_osx_cocoau_xrc-3.2.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib ../Frameworks/libwx_osx_cocoau_xrc-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_html-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_html-3.2.dylib ../Frameworks/libwx_osx_cocoau_xrc-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.2.dylib ../Frameworks/libwx_osx_cocoau_xrc-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_baseu_xml-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu_xml-3.2.dylib ../Frameworks/libwx_osx_cocoau_xrc-3.2.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.2.2.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.2.dylib ../Frameworks/libwx_osx_cocoau_xrc-3.2.dylib
echo "Fertig wx_osx_cocoau_xrc"
echo "Fertig"
