��    �      �  �   �      �     �  '   �     �     �     �     �     �          	       #        B     O     ]     t     �     �     �  '   �  �   �  '   I     q     y     �     �     �     �     �     �     �     �  	   �     �                    !  	   '     1     B     I     R     W  	   ]     g     n  !   }     �     �  ;   �     �          $     C     Y     u     �  N   �     �     �                     %  	   ,     6     B     O     b     o     x     �     �     �     �     �     �  
   �     �     �               #     '     /     I     Q  :   U     �     �     �     �  &  �     �     �       	     	        !  	   0     :     ?     O     T     p     |     �     �     �     �  	   �  	   �  	   �     �     �  2   �  +        8     A     J     N  �  f     E     N     U     ]     b  '   g  /   �  *   �     �     �     �  �      /   �  �     
   �!     �!     �!     �!     �!     �!  
   �!     �!     "     "     "  1   "  l   L"     �"     �"     �"     �"  �   	#     �#     $  #   $  
   ?$     J$     g$     ~$  0   �$     �$  &   �$     �$     %  5   %     U%  
   h%     s%     {%     �%     �%     �%  	   �%     �%     �%  	   �%     �%  	   �%     �%     �%  
   &     &     #&  c  (&     �'  '   �'     �'     �'     �'     �'     �'     �'     �'     �'  '   (     *(     7(     E(     ](     l(     z(     ~(  -   �(  �   �(  2   H)  	   {)     �)     �)     �)     �)  
   �)     �)     �)     �)     �)  	   �)      �)     *     !*     )*     2*  	   8*     B*     V*     ]*  	   c*     m*  
   s*     ~*     �*  '   �*     �*     �*  7   �*  #   +     /+     D+     c+      |+     �+     �+  B   �+     ,      ,     7,     =,     A,     G,     N,     W,     e,     t,     �,  
   �,     �,     �,     �,     �,     �,     �,     -     -     -     &-     5-     J-     V-     ]-     e-     �-     �-  G   �-     �-     �-     �-     .  �  .     �3     �3     �3  
   �3     �3     4      4     ,4     14     E4     M4     l4     x4     �4     �4     �4     �4     �4  
   �4  
   �4  	   �4     �4  6   �4  /   5     B5     H5     Q5     U5    o5  
   �7     �7     �7     �7     �7  -   �7  5   �7  /   8     A8  	   I8     S8  �   X8  8   G9  [  �9  
   �:  
   �:     �:     ;     %;     @;     D;     R;     Y;     `;     g;  6   k;  h   �;  	   <     <     $<     7<  �   U<     B=     [=     s=  	   �=     �=     �=     �=  0   �=     >  4   #>     X>  "   ^>  ;   �>     �>  
   �>  	   �>     �>     �>     �>     �>  	   ?     ?     ?  
   ?  '   %?  
   M?  	   X?     b?  	   u?     ?     �?     &   |       �      ;   X                          ?   q   �       r   �           �           �   N   �      �   �   �   E       �   +       �   �          e   �   �   i       "      �      [      �   )           �   �   `                   �      �   B   4   \   �          h   I   �   y             2   �   L       �          ~   R   �       J   
   C   3   k   #                     �   }   �   �   5           0               1       c      G         �   =   b   P   �      d   �   8           f   �   �       n   w   �   O   .   Y           �           u       �          �   a   �      s                  >   A   S      �       %   �   �   ,   7   {   _   K   @   �   �       <   ^      z       9   H   �      ]   M   �   D   W   �   F      �   �          �   �   �       Q   t   �   $       T       U   �   	      v   !       V   o           �           '   Z   *   �   �   j   /   (   �   6   l       m       �   x              g       p   �      -       :         %d  Conflicts with already scheduled fax:   disable this fax?  %.2f 1 1 minute alarm 180 About About Weatherfax All Already capturing, cannot capture:  Alsa Capture Audio Capture Audio Decoding Options Bits per Pixel Boat Position CCW CW Calculating Equator not yet supported.
 Calculating Mapping Failed.
Perhaps you have chosen the wrong mapping type for this image, or the coordinates specified are not correct
 Calculating Mapping not yet supported.
 Capture Capture Options Capture Status Capturing fax:  Carrier Close Colors Constraints Contains Contents Coord Y/X Coordinates Mapping Correction Current fax:  Decoder Delete Depth Deviation Display Selected Donate Duration Edit Email Equator Y Export External Alarm External Capture Execution failed External Command Failed Failed to apply mapping
Check Mapping Correction Parameters Failed to find Duration for:  Failed to launch:  Failed to load coordinate sets Failed to load file:  Failed to load input file:  Failed to match Area:  Failed to save xml file:  Failed to set stuff up with dsp, this only works on linux if you have /dev/dsp Failure Failure Decoding Fax:  Fathoms Fax File Filter Frequency Get Equator Get Mapping  HF Radio Schedules HF Schedules Has Area Has Valid Time Help Image Coordinates Image Correction Image Width Include header data in image Information Input Type Internet Internet Retrieval Invalid xml file Invert Lat Lat/Lon Load Schedules at Startup Loading Lon Manual Capture (with external program) automatic Open File Map Area Mapping Mapping Failed Mapping Weather Fax Image Mapping allows for scaling, and conversion between coordinate systems
Latitudes are +N -S, Longitudes +E -W

For polar mapping mode:
Get Mapping parameters calculates the mapping from the coordinates given, but the first coordinate's X value (vertical red line) must align with the pole in polar mode on the vertical meridian
Once the coordinates are entered, the mapping should make the the blue latitudes align.  If they are far off, check the input coordinates.
Once they are close:
First adjust pole X to center the latitudes. Next adjust the Pole Y value until the blue latitude curve closest to the pole is correct. Now, if the second blue latitude is too wide, then the true width ratio needs to be increased, otherwise it should be decreased. Repeat adjusting the Pole Y and true width ratio until both align. It is easiest to calibrate correctly if you use the farthest spaced latitudes available.
Once the blue latitudes align perfectly, recompute the equator to ensure the resulting mapping is correct. At this point, the coordinates can be changed to better locations if needed. If true width is very close to 1.0 it probably should be exactly 1.0.
It is also possible to go to the next step, adjust the coordinates, then come back to this step and get the mapping parameters again to improve the estimate. Mercator Message Box Meters New Coord No Action No Capture Set No Filter None Not Implemented Open Open Weather Fax Input File Output Type Phasing Pole X Pole Y Populating List Preferences Reduce to Removal 1 Removal 2 Remove Reset Resulting image has negative dimensions, aborting
 Resulting image larger than %dMB, aborting
 Retrieve Rotation Sat Save Weather Fax To KAP Select fax images by clicking in the capture (first) column

Schedules can be sorted by clicking the header column

The schedules need testing as I can only receive from a few of the possible stations.  Corrections can be made by modifying the WeatherFaxSchedules.xml file. Patches can be submitted via github.

Automatic control of a ssb radio is desireable as well, however the author only has a tecsun pl-600.  If a suitable radio can be donated, support will be implemented. Selected Server Servers Size Skew Skip start, stop and 
phasing detection Sorting this many schedules might take too long Sorting this many urls might take too long Station Stations Stop Supported Files|*.BMP;*.bmp;*.XBM;*.xbm;*.XPM;*.xpm;*.TIF;*.tif;*.TIFF;*.tiff;*.GIF;*.gif;*.JPEG;*.jpeg;*.JPG;*.jpg;*.PNM;*.pnm;*.PNG;*.png;*.PCX;*.pcx;*.PICT;*.pict;*.TGA;*.tga;*.WAV;*.wav|All files (*.*)|*.* Supported Files|*.KAP;*.kap|All files (*.*)|*.* The weatherfax plugin for opencpn is intended to reduce the amount of user interaction to receive weather fax images and to overlay them directly onto charts.

To get started, open either an image or recorded wav audio file of a ssb radio fax transmission.

License: GPLv3+

Source Code:
https://github.com/seandepagnier/weatherfax_pi

Author:
Sean D'Epagnier

Many thanks to all of the translators and testers. Time (UTC) Transparency True Width Ratio Tune ssb radio to Unrecognized xml node Url Valid Time Value 1 Value 2 Value 3 W/H WAV files (*.wav)|*.WAV;*.wav|All files (*.*)|*.* Warning, latitudes on different sides of equator not recommended because of ambiguity of north or south pole Weather Fax Weather Fax Image Weather Fax Mapper Weather Fax PlugIn for OpenCPN Weather Fax PlugIn for OpenCPN
Read weather fax encoded data as audio or image. 
Overlay this on top of charts. 
Enable OpenGL (in Display options) for best results. 

The Weather Fax plugin was written by Sean D'Epagnier
 Weather Fax Plugin Version Weather Fax Preferences Weather Fax Schedule Beginning Soon WeatherFax WeatherFax InternetRetrieval WeatherFax Preferences WeatherFax Schedules aplay /usr/local/share/opencpn/sounds/2bells.wav arecord -f S16_LE cannot deal with incomplete input file conic could not open input file:  fax_decode_image_line requires specific buffer length fix sounding datum fixed-flat hour(s) khz max khz min mercator middle minute(s) narrow polar removal 3 sample size not 8 or 16 bit:  second(s) starting in to receive fax for weatherfax weatherfax schedules wide Project-Id-Version: weatherfax_pi 3.2.0
Report-Msgid-Bugs-To: 
POT-Creation-Date: 2013-10-25 15:37+0200
PO-Revision-Date: 2013-10-25 17:17+0100
Last-Translator: CarCode <carcode@me.com>
Language-Team: CarCode <carcode@me.com>
Language: de
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
X-Generator: Poedit 1.5.7
  %d  Konflikte mit schon eingeplantem Fax:   verwerfe dies Fax?  %.2f 1 1 Minute Alarm 180 Über Über WetterFax Alle Schon empfangen, kann nicht empfangen:  Alsa Empfang Audio Empfang Audio Dekodier-Optionen Bits pro Pixel Boot Position CCW CW Berechnung Äquator noch nicht unterstützt.
 Berechnung der Zeichnung fehlgeschlagen.
Vielleicht wurde falscher Zeichnungstyp für dies Bild gewählt oder angegebene Koordinaten sind nicht korrekt
 Berechnung der Zeichnung noch nicht unterstützt.
 Empfangen Empfang Optionen Empfang Status Empfange Fax:  Carrier Schließen Farben Grenzen Enthält Inhalt Koord Y/X Korrektur Zeichnungs-Koordinaten Aktuelles Fax:  Dekoder Löschen Tiefe Deviation Gewähltes anzeigen Spende Dauer Editieren Email Äquator Y Export Externer Alarm Externer Empfangs-Befehl fehlgeschlagen Externer Befehl Fehler Fehler beim Kartieren
Prüfe Karten-Korrektur Parameter Fehler beim Finder der Dauer für:  Fehler bei Starten:  Fehler bei Laden Koordinaten:  Fehler bei Laden Datei:  Fehler bei Laden Eingangsdatei:  Fehler bei Gebietsdaten:  Fehler bei Sichern xml-Datei:  Fehler bei Verwendung von dsp, das geht nur mit Linux und /dev/dsp Fehler Failure Decoding Fax:  Faden Fax Datei Filter Frequenz Hole Äquator Hole Zeichnung HF Radio Zeitpläne HF Zeitpläne Hat Gebiet Hat gültige Zeit Hilfe Bild Koordinaten Bild Korrektur Bild Breite Kopfdaten in Bild einschließen Information Eingangstyp Internet Internet Suche Ungültige XML Datei Invertieren Breite Lat/Lon Lade Zeitpläne beim Start Lade Länge Manuelles Empfangen (mit externem Programm) automatisches Datei-Öffnen Karten-Gebiet Zeichnen Zeichnen fehlgeschlagen Zeichne WetterFax Bild Zeichnung erlaubt für Skalierung und Umwandlung zwischen Koordinaten Systemen
Breiten sind +N -S, Längen +E -W

Für Polar Zeichnungs Modus:
Zeichnungs-Parameter berechnen die Zeichnung mit den angegebenen Koordinaten, aber der X-Wert (vertikale rote Linie) der ersten Koordinate muß innerhalb der Pole im Polar Modus im vertikalen Meridian liegen
Sind die Koordinaten eingegeben, soll die Zeichnung mit den blauen Breiten übereinstimmen. Sind sie weit entfernt, überprüfen Sie die Eingangs-Koordinaten.
Sind sie nahe:
Justieren Sie erst Pol X zur Zentrierung der Breiten. Dann justieren Sie den Pol Y-Wert, bis die blaue Breitenkurve am nächsten zum Pol korrekt ist. Ist die zweite blaue Breite zu weit, dann muß das echte Breitenverhältnis erhöht werden, sonst muß es erniedrigt werden. Wiederholen Sie die Justierung des Pols Y und des echten Breitenverhältnisses, bis beide übereinstimmen. Am Einfachsten ist eine korrekte Kalibrierung, wenn die verfügbare weitest entfernte Breite benutzt wird.
Stimmen die blauen Breiten perfekt überein, berechnen Sie den Äquator erneut, um eine korrekte Zeichnung zu erhalten. An diesem Punkt können die Koordinaten erforderlichenfalls auf einen besseren Ort gesetzt werden. Ist die echte Breite nahe bei 1.0, sollte es vermutlich exakt 1.0 sein.
Man kann auch zum nächsten Schritt gehen, die Koordinaten justieren und dann zu diesem Schritt zurück gehen, um die Zeichnungs-Parameter erneut zu optimieren. Mercator Nachrichten-Box Meter Neue Koord Keine Aktion Kein Empfang gesetzt Kein Filter Kein Nicht implementiert Öffnen Öffne WetterFax Eingangsdatei Ausgangstyp Phasend Pol X Pol Y Fülle Liste Einstellungen Vermindere um Entfernt 1 Entfernt 2 Entfernen Reset Resultierendes Bild hat negative Dimensionen, Abbruch
 Resultierendes Bild größer als %dMB, Abbruch
 Suche Rotation Sat WetterFax als KAP sichern Wähle Fax Bild mit Klick in die (erste) Empfangs-Spalte

Zeitpläne können mit Klick auf die Kopfzeilen-Spalte sortiert werden

Die Zeitpläne müssen getestet werden, da ich nur einige der möglichen Stationen empfangen kann. Korrekturen kann man in der Datei WeatherFaxSchedules.xml machen. Änderungen können mit Github mitgeteilt werden.

Automatische Kontrolle eines SSB Radios ist wünschenswert, jedoch hat der Autor nur ein tecsun pl-600. Wird ein entsprechendes Radio gespendet, können Ergänzungen vorgenommen werden. Gewähltes Server Servers Größe Drehung Überspringe Start, Stop und 
Phasen-Prüfung Sortierung derart vieler Zeitpläne kann lange dauern Sortierung derart vieler URLs kann lange dauern Station Stationen Stop Unterstützte Dateien|*.BMP;*.bmp;*.XBM;*.xbm;*.XPM;*.xpm;*.TIF;*.tif;*.TIFF;*.tiff;*.GIF;*.gif;*.JPEG;*.jpeg;*.JPG;*.jpg;*.PNM;*.pnm;*.PNG;*.png;*.PCX;*.pcx;*.PICT;*.pict;*.TGA;*.tga|WAV Dateien (*.wav)|*.WAV;*.wav|Alle Dateien (*.*)|*.* Unterstützte Dateien|*.KAP;*.kap|Alle Dateien (*.*)|*.* Das Wetterfax Plugin für OpenCPN soll den Wetterfax-Empfang vereinfachen und sie direkt auf der Karte anzeigen.

Zu Beginn öffnen Sie entweder ein Bild oder eine vom SSB Radio aufgenommene WAV Datei.

Lizenz: GPLv3+

Quell-Code:
https://github.com/seandepagnier/weatherfax_pi

Author:
Sean D'Epagnier

Vielen Dank an alle Übersetzer und Tester. Zeit (UTC) Tranparenz Wahres Breitenverhältnis SSB Radio einstellen auf Nicht erkannter XML Knoten Url Gültige Zeit Wert 1 Wert 2 Wert 3 W/H WAV Dateien (*.wav)|*.WAV;*.wav|Alle Dateien (*.*)|*.* Warnung, Breiten auf beiden Seiten des Äquators nicht empfohlen wegen Unklarheit von Nord- oder Südpol WetterFax WetterFax Bild WetterFax Zeichner WetterFax PlugIn für OpenCPN WetterFax PlugIn für OpenCPN
Liest Wetterfaxe kodiert als Daten, Audio oder Bild. 
Überlagert diese über die Karten. 
Beste Resultate mit OpenGL aktiviert (in Optionen/Anzeige). 

Das WetterFax Plugin hat Sean D'Epagnier geschrieben
 WetterFax PlugIn Version WetterFax Einstellungen Wetterfax Zeitplan beginnt bald WetterFax WetterFax Internetsuche WetterFax Einstellungen WetterFax Zeitplan aplay /usr/local/share/opencpn/sounds/2bells.wav arecord -f S16_LE Kann unvollständige Eingangsdatei nicht verarbeiten conic Kann Eingangsdatei nicht öffnen:  fax_decode_image_line erfordert spezifische Speichergröße Festes Tiefen-Datum fixed-flat Sunden(n) khz max khz min mercator mittel Minute(n) nah polar Entfernt 3 Samplegröße ist nicht 8 oder 16 Bit:  Sekunde(n) starte in zum Faxempfang von WetterFax Wetterfax Zeitpläne weit 