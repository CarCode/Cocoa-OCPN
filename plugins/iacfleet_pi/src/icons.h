#ifndef ICONS_H
#define ICONS_H 1

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#	include <wx/wx.h>
#endif

extern void initialize_images(void);
extern void deinitialize_images(void);

extern wxBitmap *_img_iacfleet_pi;

#ifdef IACFLEET_USE_SVG
extern wxString _svg_iacfleet;
extern wxString _svg_iacfleet_rollover;
extern wxString _svg_iacfleet_toggled;
#endif

#endif /* ICONS_H */
