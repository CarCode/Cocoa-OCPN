// file: csv_parser.c
// The code is from http://forum.codecall.net/topic/45404-parsing-csv/?p=454107

#include "csv_parser.h"
#include <ctype.h>   // for isspace

// private:
struct csv_parser_data
{
    CSV_CB_record_handler callback;
    void                * params;
    char                  buff[MAX_LINE_LEN];
    int                   field_count;
    const char *          column_values[MAX_COLUMN_COUNT];
};

/* digest form CSV wiki http://en.wikipedia.org/wiki/Comma-separated_values

    * fields that contain commas, double-quotes, or line-breaks must be quoted,
    * a quote within a field must be escaped with an additional quote
      immediately preceding the literal quote,
    * space before and after delimiter commas may be trimmed (which is
      prohibited by RFC 4180), and
    * a line break within an element must be preserved.
*/



// returns 0 on success, E_QUOTED_STRING ON improperly quoted string
//
// pre-condition: **buff points to the beginning quote
// post-condition: **buff points to just before either a comma,
//              or a newline, or E_QUOTED_STRING is returned.
//
static int csv_process_quoted_string(char **buff)
{
    char * q = *buff;
    char * p = q++;
    while (1)
    {
        switch(*q)
        {
        case '\0': // end of line in Quoted String, it's an error
            return E_QUOTED_STRING;
        case '"': // if the next char is not a '"', the QuotedString ends.
            if(*++q!='"')
                goto done_quoted_string;
            // here we deliberately let the else case fall through to default
            // processing
            //
        default:
            *p=*q;
            break;
        }
        ++p, ++q;
    }
done_quoted_string:
    *p='\0';

    while( *q==' ' || *q=='\t' )
        ++q;
    if( *q!=',' && *q!='\n' && *q!='\0')
        return E_QUOTED_STRING;
    *buff=--q;
    return 0;

}

//  returns
//   0 : to continue parse next record
//   non-zero:  abort processing
//       E_QUOTED_STRING is a special case of non-zero return values
//
static int csv_parse_line (struct csv_parser_data * d)
{
    char c;
    char * buff=d->buff;
    d->column_values[0]= buff;
    d->field_count=1;

    while ( (c=*buff)!='\n' )
    {
        switch (c)
        {
        case ',': // mark the end of the current field, and beginning of next field
            *buff='\0';
            d->column_values[d->field_count++]=buff+1;
            break;
        case '"': // beginning a quoted string
            if( E_QUOTED_STRING==csv_process_quoted_string(&buff) )
                return E_QUOTED_STRING;
            break;
        //default: no action

        }
        ++buff;
    }
    // now buff points to '\n', replace it with a '\0'
    *buff='\0';

    if (d->callback==NULL) 
        return 0;
    return d->callback (d->params, d->field_count, d->column_values);

}
// returns
//  0: on success
//  E_LINE_TOO_WIDE: on line too wide
//  E_QUOTED_STRING: at least 1 Quoted String is ill-formatted
//
int csv_parse(FILE *fp, CSV_CB_record_handler cb, void *params)
{
    //char buff[MAX_LINE_LEN];
    struct csv_parser_data d;
    
    d.callback = cb;
    d.params   = params;

    while ( d.buff[MAX_LINE_LEN-1]='*',
            NULL!= fgets (d.buff, MAX_LINE_LEN, fp)
    ){
        int r;
        if(d.buff[MAX_LINE_LEN-1]=='\0' && d.buff[MAX_LINE_LEN-2]!='\n')
            return E_LINE_TOO_WIDE;
        if (E_QUOTED_STRING==(r=csv_parse_line (&d) ) )
            return E_QUOTED_STRING;
        else if (r!=0)
            break;
    }
    return 0;
}
