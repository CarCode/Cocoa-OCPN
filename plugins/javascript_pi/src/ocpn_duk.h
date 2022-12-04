/* *************************************************************************
* Project:  OpenCPN
* Purpose:  JavaScript Plugin
* Author:   Tony Voss 16/05/2020
*
* Copyright Ⓒ 2021 by Tony Voss
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License, under which
* you must including a copy of these terms in all copies
* https://www.gnu.org/licenses/gpl-3.0.en.html
***************************************************************************/

#if 0
#ifndef OCPN_DUK_H
#define OCPN_DUK_H

#define DUK_DUMP true
#if DUK_DUMP
#define MAYBE_DUK_DUMP duk_push_context_dump(ctx);pConsole->message(STYLE_ORANGE, "", "Duktape context dump:\n"+duk_to_string(ctx, -1)+"\n");duk_pop(ctx);
#define ERROR_DUMP {pConsole->message(STYLE_ORANGE, "", "error dump\n"+(duk_is_error(ctx, -1)?(_("Error object with " )+duk_safe_to_string(ctx, -1)+"\n"):"No error object\n"));}\
MAYBE_DUK_DUMP
#endif  //  DUK_DUMP
 
#endif /* OCPN_DUK_H */
#endif
