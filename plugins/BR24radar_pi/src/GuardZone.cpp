/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Navico BR24 Radar Plugin
 * Author:   David Register
 *           Dave Cowell
 *           Kees Verruijt
 *
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
 ***************************************************************************/

#include "br24radar_pi.h"

PLUGIN_BEGIN_NAMESPACE

#undef TEST_GUARD_ZONE_LOCATION

void GuardZone::ProcessSpoke(SpokeBearing angle, UINT8* data, UINT8* hist, size_t len, int range) {
  size_t range_start = m_inner_range * RETURNS_PER_LINE / range;  // Convert from meters to 0..511
  size_t range_end = m_outer_range * RETURNS_PER_LINE / range;    // Convert from meters to 0..511
  bool in_guard_zone = false;

  switch (m_type) {
    case GZ_ARC:
      if ((angle >= m_start_bearing && angle < m_end_bearing) ||
          (m_start_bearing >= m_end_bearing && (angle >= m_start_bearing || angle < m_end_bearing))) {
        if (range_start < RETURNS_PER_LINE) {
          if (range_end > RETURNS_PER_LINE) {
            range_end = RETURNS_PER_LINE;
          }

          for (size_t r = range_start; r <= range_end; r++) {
            if (!m_multi_sweep_filter || HISTORY_FILTER_ALLOW(hist[r])) {
              if (data[r] >= m_pi->m_settings.threshold_blue) {
                m_running_count++;
              }
#ifdef TEST_GUARD_ZONE_LOCATION
              // Zap guard zone computation location to green so this is visible on screen
              else {
                data[r] = m_pi->m_settings.threshold_green;
              }
#endif
            }
          }
        }
        in_guard_zone = true;
      }
      break;

    case GZ_CIRCLE:
      if (range_start < RETURNS_PER_LINE) {
        if (range_end > RETURNS_PER_LINE) {
          range_end = RETURNS_PER_LINE;
        }

        for (size_t r = range_start; r <= range_end; r++) {
          if (!m_multi_sweep_filter || HISTORY_FILTER_ALLOW(hist[r])) {
            if (data[r] >= m_pi->m_settings.threshold_blue) {
              m_running_count++;
            }
#ifdef TEST_GUARD_ZONE_LOCATION
            // Zap guard zone computation location to green so this is visible on screen
            else {
              data[r] = m_pi->m_settings.threshold_green;
            }
#endif
          }
        }
        if (angle > m_last_angle) {
          in_guard_zone = true;
        }
      }
      break;

    default:
      in_guard_zone = false;
      break;
  }

  if (m_last_in_guard_zone && !in_guard_zone) {
    // last bearing that could add to m_running_count, so store as bogey_count;
    m_bogey_count = m_running_count;
    m_running_count = 0;
    LOG_GUARD(wxT("%s angle=%d last_angle=%d range=%d guardzone=%d..%d (%d - %d) bogey_count=%d"), m_log_name.c_str(), angle,
              m_last_angle, range, range_start, range_end, m_inner_range, m_outer_range, m_bogey_count);

    // When debugging with a static ship it is hard to find moving targets, so move
    // the guard zone instead. This slowly rotates the guard zone.
    if (m_pi->m_settings.guard_zone_debug_inc && m_type == GZ_ARC) {
      m_start_bearing += LINES_PER_ROTATION - m_pi->m_settings.guard_zone_debug_inc;
      m_end_bearing += LINES_PER_ROTATION - m_pi->m_settings.guard_zone_debug_inc;
      m_start_bearing %= LINES_PER_ROTATION;
      m_end_bearing %= LINES_PER_ROTATION;
    }
  }

  m_last_in_guard_zone = in_guard_zone;
  m_last_angle = angle;
}

PLUGIN_END_NAMESPACE
