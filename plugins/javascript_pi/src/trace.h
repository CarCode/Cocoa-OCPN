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

#ifndef trace_h
#define trace_h
// 
//USE:  TRACE(level, wxString); will display if level >= TRACE_LEVEL
// output will go to log or, if in harness, to stdout
#define TRACE_LEVEL 0   // set to zero to omit all tracing code
#ifdef IN_HARNESS
#define TRACE_LEVEL 5   // trace level override for harness
#endif
#define TRACE_TO_WINDOW true     // if true, trace will be to a window
//                                 if false, then to the log file or if in the harness to stdout
#if TRACE_TO_WINDOW
    // trace to window goes here
    void windowTrace(int level, wxString text);
    inline void ptrace(int level, wxString text){
        if (level <= TRACE_LEVEL) windowTrace(level, wxString::Format("L%d\t%s", level, text));
        }
#else   // TRACE_TO_WINDOW false

    #ifdef IN_HARNESS
        #include <iostream>
        inline void ptrace(int level, wxString text){
            if (level <= TRACE_LEVEL) std::cout << "Trace L" << level << ":\t" << text << "\n";
            }
    #else   // not IN_HARNESS
        inline void ptrace(int level, wxString text){
            if (level <= TRACE_LEVEL) wxLogMessage(wxString::Format("JavaScript_pi L%d\t%s", level, text));
            }
    #endif  // IN_HARNESS
#endif    // TRACE_TO_WINDOW false

#if TRACE_LEVEL>0
    #define TRACE ptrace
#else
    #define TRACE
#endif  // TRACE_LEVEL

#endif // trace_h

