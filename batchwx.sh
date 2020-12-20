# install_name_tool -change old new file; 3.1.2 oder 3.1.3 oder 3.1.4 einsetzen
# old siehe otool -L file

install_name_tool -change /usr/local/opt/cairo/lib/libcairo.2.dylib @executable_path/../Frameworks/libcairo.2.dylib Frameworks/libcairo.2.dylib
install_name_tool -change /usr/local/opt/pixman/lib/libpixman-1.0.dylib @executable_path/../Frameworks/libpixman-1.0.dylib Frameworks/libcairo.2.dylib
install_name_tool -change /usr/local/opt/fontconfig/lib/libfontconfig.1.dylib @executable_path/../Frameworks/libfontconfig.1.dylib Frameworks/libcairo.2.dylib
install_name_tool -change /usr/local/opt/freetype/lib/libfreetype.6.dylib @executable_path/../Frameworks/libfreetype.6.dylib Frameworks/libcairo.2.dylib
install_name_tool -change /usr/local/opt/libpng/lib/libpng16.16.dylib @executable_path/../Frameworks/libpng16.16.dylib Frameworks/libcairo.2.dylib
echo "Fertig cairo"

install_name_tool -change /usr/local/opt/xz/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib Frameworks/liblzma.5.dylib
echo "Fertig lzma"

install_name_tool -change /usr/local/lib/libwx_baseu_net-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_baseu_net-3.1.dylib Frameworks/libwx_baseu_net-3.1.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib Frameworks/libwx_baseu_net-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.1.dylib Frameworks/libwx_baseu_net-3.1.dylib
echo "Fertig wx_baseu_net"

install_name_tool -change /usr/local/lib/libwx_baseu_xml-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_baseu_xml-3.1.dylib Frameworks/libwx_baseu_xml-3.1.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib Frameworks/libwx_baseu_xml-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.1.dylib Frameworks/libwx_baseu_xml-3.1.dylib
echo "Fertig wx_baseu_xml"

install_name_tool -change /usr/local/lib/libwx_baseu-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.1.dylib Frameworks/libwx_baseu-3.1.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib Frameworks/libwx_baseu-3.1.dylib
echo "Fertig wx_baseu"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_adv-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_adv-3.1.dylib Frameworks/libwx_osx_cocoau_adv-3.1.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib Frameworks/libwx_osx_cocoau_adv-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.1.dylib Frameworks/libwx_osx_cocoau_adv-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.1.dylib Frameworks/libwx_osx_cocoau_adv-3.1.dylib
echo "Fertig wx_osx_cocoa_adv"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_aui-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_aui-3.1.dylib Frameworks/libwx_osx_cocoau_aui-3.1.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib Frameworks/libwx_osx_cocoau_aui-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_adv-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_adv-3.1.dylib Frameworks/libwx_osx_cocoau_aui-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.1.dylib Frameworks/libwx_osx_cocoau_aui-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.1.dylib Frameworks/libwx_osx_cocoau_aui-3.1.dylib
echo "Fertig wx_osx_cocoa_aui"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.1.dylib Frameworks/libwx_osx_cocoau_core-3.1.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib Frameworks/libwx_osx_cocoau_core-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.1.dylib Frameworks/libwx_osx_cocoau_core-3.1.dylib
echo "Fertig wx_osx_cocoa_core"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_gl-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_gl-3.1.dylib Frameworks/libwx_osx_cocoau_gl-3.1.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib Frameworks/libwx_osx_cocoau_gl-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.1.dylib Frameworks/libwx_osx_cocoau_gl-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.1.dylib Frameworks/libwx_osx_cocoau_gl-3.1.dylib
echo "Fertig wx_osx_cocoa_gl"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_html-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_html-3.1.dylib Frameworks/libwx_osx_cocoau_html-3.1.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib Frameworks/libwx_osx_cocoau_html-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.1.dylib Frameworks/libwx_osx_cocoau_html-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.1.dylib Frameworks/libwx_osx_cocoau_html-3.1.dylib
echo "Fertig wx_osx_cocoa_html"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_qa-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_qa-3.1.dylib Frameworks/libwx_osx_cocoau_qa-3.1.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib Frameworks/libwx_osx_cocoau_qa-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.1.dylib Frameworks/libwx_osx_cocoau_qa-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.1.dylib Frameworks/libwx_osx_cocoau_qa-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_baseu_xml-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_baseu_xml-3.1.dylib Frameworks/libwx_osx_cocoau_qa-3.1.dylib
echo "Fertig wx_osx_cocoa_qa"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_webview-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_webview-3.1.dylib Frameworks/libwx_osx_cocoau_webview-3.1.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib Frameworks/libwx_osx_cocoau_webview-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.1.dylib Frameworks/libwx_osx_cocoau_webview-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.1.dylib Frameworks/libwx_osx_cocoau_webview-3.1.dylib
echo "Fertig wx_osx_cocoa_webview"

install_name_tool -change /usr/local/lib/libwx_osx_cocoau_xrc-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_xrc-3.1.dylib Frameworks/libwx_osx_cocoau_xrc-3.1.dylib
install_name_tool -change /usr/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib Frameworks/libwx_osx_cocoau_xrc-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_html-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_html-3.1.dylib Frameworks/libwx_osx_cocoau_xrc-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_adv-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_adv-3.1.dylib Frameworks/libwx_osx_cocoau_xrc-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_osx_cocoau_core-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_osx_cocoau_core-3.1.dylib Frameworks/libwx_osx_cocoau_xrc-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_baseu_xml-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_baseu_xml-3.1.dylib Frameworks/libwx_osx_cocoau_xrc-3.1.dylib
install_name_tool -change /usr/local/lib/libwx_baseu-3.1.4.0.0.dylib @executable_path/../Frameworks/libwx_baseu-3.1.dylib Frameworks/libwx_osx_cocoau_xrc-3.1.dylib
echo "Fertig wx_osx_cocoa_xrc"
echo "Fertig"
