/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin
 * Author:   David Register
 *           Dave Cowell
 *           Kees Verruijt
 *           Douwe Fokkema
 *           Sean D'Epagnier
 *           Martin Hassellov: testing the Raymarine radar
 *           Matt McShea: testing the Raymarine radar
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register              bdbcat@yahoo.com *
 *   Copyright (C) 2012-2013 by Dave Cowell                                *
 *   Copyright (C) 2012-2016 by Kees Verruijt         canboat@verruijt.net *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 */

//
// This defines the capabilities of some Raymarine radars, e.g. the common parts
// Ranges are not included because those are radar dependent.
//

#include "SoftwareControlSet.h"

//
// Make an entry for all control types. Specify all supported (or all?) controls.
//
// Macro:   HAVE_CONTROL (if it has this)
// Field 1: Control identifier
// Field 2: 0 = No auto, n = maximum auto value
//          For instance, Navico radars have multiple values for Auto Sea State.
// Field 3: Default value for initial setting (most values are stored in settings)
// Field 4: Minimal value
// Field 5: Maximum value
// Field 6: Step (for instance, min 0 / max 100 / step 10 gives 11 possible values)
// Field 7: Names
//
// Note that for range the min, max, step values are ignored, see TODO.

#ifndef OFF_LOW_MEDIUM_HIGH_NAMES
#define OFF_LOW_MEDIUM_HIGH_NAMES \
  { _("Off"), _("Low"), _("Medium"), _("High") }
#endif
#ifndef OFF_LOW_HIGH_NAMES
#define OFF_LOW_HIGH_NAMES \
  { _("Off"), _("Low"), _("High") }
#endif

#ifndef SLOW_FAST_NAMES
#define SLOW_FAST_NAMES \
  { _("Slow"), _("Fast") }
#endif
#ifndef RM_SEA_AUTO_NAMES
#define RM_SEA_AUTO_NAMES \
  { _("Harbor"), _("Offshore"), _("Coastal") }
#endif

#ifndef RM_MODE_NAMES
#define RM_MODE_NAMES \
  {_("Harbor"), _("Coastal"), _("Offshore"), _("Weather"),}
#endif

HAVE_CONTROL(CT_BEARING_ALIGNMENT, CTD_AUTO_NO, CTD_DEF_ZERO, -1795, +1800, CTD_STEP_5, {wxT("*0.1 degrees")})
HAVE_CONTROL(CT_GAIN, CTD_AUTO_YES, 50, CTD_MIN_ZERO, CTD_MAX_100, CTD_STEP_1, CTD_PERCENTAGE)
HAVE_CONTROL(CT_COLOR_GAIN, CTD_AUTO_YES, 50, CTD_MIN_ZERO, CTD_MAX_100, CTD_STEP_1, CTD_PERCENTAGE)
HAVE_CONTROL(CT_INTERFERENCE_REJECTION, CTD_AUTO_NO, CTD_DEF_ZERO, CTD_MIN_ZERO, 5, CTD_STEP_1, CTD_NUMERIC)

// HAVE_CONTROL(CT_LOCAL_INTERFERENCE_REJECTION, CTD_AUTO_NO, CTD_DEF_ZERO, CTD_MIN_ZERO, 3, CTD_STEP_1, OFF_LOW_MEDIUM_HIGH_NAMES)

HAVE_CONTROL(CT_RAIN, CTD_AUTO_NO, CTD_DEF_OFF, CTD_MIN_ZERO, CTD_MAX_100, CTD_STEP_2, CTD_PERCENTAGE)
HAVE_CONTROL(CT_RANGE, CTD_AUTO_YES, 1000, CTD_MIN_ZERO, 0, CTD_STEP_1, CTD_NUMERIC)

HAVE_CONTROL(CT_SEA, CTD_AUTO_YES, CTD_DEF_ZERO, CTD_MIN_ZERO, CTD_MAX_100, CTD_STEP_1, CTD_PERCENTAGE)

HAVE_CONTROL  (CT_MODE, CTD_AUTO_NO, CTD_DEF_ZERO, CTD_MIN_ZERO, 4, CTD_STEP_1, RM_MODE_NAMES)

HAVE_CONTROL(CT_ALL_TO_AUTO, CTD_AUTO_YES, CTD_DEF_ZERO, CTD_MIN_ZERO, CTD_MAX_1, CTD_STEP_1, OFF_ON_NAMES)

HAVE_CONTROL(CT_TARGET_EXPANSION, CTD_AUTO_NO, CTD_DEF_ZERO, CTD_MIN_ZERO, CTD_MAX_1, CTD_STEP_1, OFF_ON_NAMES)

// Don't forget to enable buttons and update labels in ControlsDialog.cpp around 1810 and 1960
// And initialise m_xxx_button in ControlsDialog.h
