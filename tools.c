/*
 * tools.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <stdlib.h>
#include <iconv.h>
#include "tools.h"
#include "common.h"

// --- Static -----------------------------------------------------------

struct conv_table {
  char *from;
  char *to;
  };

static conv_table html_to_iso_table[] =
{
  // 'to' field must be smaller than 'from'
  {"&#228;",   "ä"},
  {"&auml;",   "ä"},
  {"&#196;",   "Ä"},
  {"&Auml;",   "Ä"},
  {"&#246;",   "ö"},
  {"&ouml;",   "ö"},
  {"&#214;",   "Ö"},
  {"&Ouml;",   "Ö"},
  {"&#229;",   "å"},
  {"&åuml;",   "å"},
  {"&#197;",   "Å"},
  {"&Åuml;",   "Å"},
  {"&#220;",   "Ü"},
  {"&Uuml;",   "Ü"},
  {"&#252;",   "ü"},
  {"&uuml;",   "ü"},
  {"&#223;",   "ß"},
  {"&szlig;",  "ß"},
  {"&#8211;",  "-"},
  {"&ndash;",  "-"},
  {"&#38;",    "&"},
  {"&amp;",    "&"},
  {"&#180;",   "'"},
  {"&acute;",  "'"},
  {"&#231;",   "ç"},
  {"&ccedil;", "ç"},
  {"&#233;",   "é"},
  {"&eacute;", "é"},
  {"&#226;",   "â"},
  {"&acirc;",  "â"},
  {"&#8364;",  "¤"},
  {"&euro;",   "¤"},
  {"&#8220;",  "\""},
  {"&#8221;",  "\""},
  {"&#8222;",  "\""},
  {"&nbsp;",   " "},
  {"&lt;",     "<"},
  {"&gt;",     ">"},
  {"\n\n",     "\n"}, // let's also strip multiple linefeeds
};

// --- General functions ------------------------------------------------

int charsetconv(const char *buffer, int buf_len, const char *str, int str_len, const char *from, const char *to)
{
  if (to && from) {
     iconv_t ic = iconv_open(to, from);
     if (ic >= 0) {
        size_t inbytesleft = str_len;
        size_t outbytesleft = buf_len;
        char *out = (char*)buffer;
        int ret;
        if ((ret = iconv(ic, (char**)&str, &inbytesleft, &out, &outbytesleft)) >= 0) {
           iconv_close(ic);
           return buf_len - outbytesleft;
           }
        iconv_close(ic);
        }
     }
  else {
     error("charsetconv(): charset is not valid");
     }
  return -1;
}

char *htmlcharconv(char *str)
{
  for (unsigned int i = 0; i < (sizeof(html_to_iso_table) / sizeof(html_to_iso_table[0])); ++i) {
     char *ptr = strstr(str, html_to_iso_table[i].from);
     while (ptr) {
        int of = ptr - str;
        int l  = strlen(str);
        int l1 = strlen(html_to_iso_table[i].from);
        int l2 = strlen(html_to_iso_table[i].to);
        if (l2 > l1) {
           error("htmlcharconv(): cannot reallocate string");
           return str;
           }
        if (l2 != l1)
           memmove(str + of + l2, str + of + l1, l - of - l1 + 1);
        strncpy(str + of, html_to_iso_table[i].to, l2);
        ptr = strstr(str, html_to_iso_table[i].from);
        }
     }
  return str;
}

char *striphtml(char *str)
{
  char *c, t = 0, *r;
  c = str;
  r = str;
  while (*str != '\0') {
     if (*str == '<')
        t++;
     else if (*str == '>')
        t--;
     else if (t < 1)
        *(c++) = *str;
     str++;
     }
  *c = '\0';
  return r;
}

void *myrealloc(void *ptr, size_t size)
{
  /* There might be a realloc() out there that doesn't like reallocing
     NULL pointers, so we take care of it here */
  if (ptr)
     return realloc(ptr, size);
  else
     return malloc(size);
}
