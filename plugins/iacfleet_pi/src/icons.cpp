#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filename.h>
#include <wx/mstream.h>
#include "icons.h"

wxBitmap *_img_iacfleet_pi;

#ifdef IACFLEET_USE_SVG
#include "../../../include/ocpn_plugin.h"
wxString _svg_iacfleet;
wxString _svg_iacfleet_rollover;
wxString _svg_iacfleet_toggled;
#endif

void initialize_images(void)
{
	{
		wxMemoryInputStream sm("\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000 \000\000\000 \b\006\000\000\000szz\364\000\000\000\001sRGB\000\256\316\034\351\000\000\000\006bKGD\000\377\000\377\000\377\240\275\247\223\000\000\000\011pHYs\000\000\013\023\000\000\013\023\001\000\232\234\030\000\000\000\atIME\a\333\n\036\004\027\n\254\004\273\025\000\000\0013IDATX\303\325WK\216\005!\b\004\303\375\257\\o\361\342\304\241\371\252\235\311\260\264\265)\212\002\221\001\200\22413\031\313\341wf&\"J\317\255{\230\231\204\232\3469\217\034O[\035\003 \000O\000\321\317\252\216f\244\031#DD\203\016M\203Z\243\003\360\013\314\nj\256\217\023\352\365wk\217\a\342\n\003\332\341t\246\035Z \346\332\330\311\361M\0337\350\177\035\300\r\200\236\026\006\375\261\275\006\300\022c\n +\231.\275\2254I\307\201W\343\272\024;\272\371\377\032X\351_\323\260j\300k\327&\200J\377\366\200\350=^\352\332\032\350\260\3401\342\315\014\014\a\236\207\274r)=\234\004\303\n\177\327{\216\252 \254a\344\321+\000`'\332\356\031O\210\262S\377V\336\273\315j\236\221\252Z\263\031owD\373\021\341\355Y0\002\352VA\006\"\242\333\232x*\377\221]\272#F:\251\224\252\350\274\331\376tb\222[\335oG\274&\200nEDwGE\254\322\255\377\352\353'\273\270\322\273 \353\353\273\242\323\201\311\215Fs2EK\247\261\234\276\r\254@>\233\227W,\3376\245\231\000\000\000\000IEND\256B`\202", 435);
		_img_iacfleet_pi = new wxBitmap(wxImage(sm));
	}

#ifdef IACFLEET_USE_SVG
    wxFileName fn;
#ifdef __WXOSX__
    fn.SetPath(*GetpPrivateApplicationDataLocation());
#else
    fn.SetPath(*GetpSharedDataLocation());
#endif
    fn.AppendDir(_T("plugins"));
    fn.AppendDir(_T("iacfleet_pi"));
    fn.AppendDir(_T("data"));
    fn.SetFullName(_T("iacfleet_pi.svg"));
    _svg_iacfleet = fn.GetFullPath();
    fn.SetFullName(_T("iacfleet_pi_rollover.svg"));
    _svg_iacfleet_rollover = fn.GetFullPath();
    fn.SetFullName(_T("iacfleet_pi_toggled.svg"));
    _svg_iacfleet_toggled = fn.GetFullPath();
#endif

	return;
}

void deinitialize_images(void){
      if(NULL !=_img_iacfleet_pi){
            delete(_img_iacfleet_pi);
      }
}
