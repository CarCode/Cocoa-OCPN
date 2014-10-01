// file: csv_parser.h
// The code is from http://forum.codecall.net/topic/45404-parsing-csv/?p=454107

#ifndef _CSV_PARSER_H_
#define _CSV_PARSER_H_

#include <stdio.h>  // for fopen, fclose, etc.


#define MAX_LINE_LEN (1024*512)
#define MAX_COLUMN_COUNT 1024
/* digest from CSV wiki: http://en.wikipedia.org/wiki/Comma-separated_values

    * fields that contain commas, double-quotes, or line-breaks must be quoted,
    * a quote within a field must be escaped with an additional quote
      immediately preceding the literal quote,
    * space before and after delimiter commas may be trimmed (which is
      prohibited by RFC 4180), and
    * a line break within an element must be preserved.
*/

enum { E_LINE_TOO_WIDE=-2, // error code for line width >= MAX_LINE_LEN
       E_QUOTED_STRING     // error code for ill-formatted quoted string
};

// mimic sqlite callback interface
//
typedef int (*CSV_CB_record_handler)
(
    void * params,
    int colum_cnt,
    const char ** column_values
);

int csv_parse (FILE *fp, CSV_CB_record_handler cb, void *params);

#endif
