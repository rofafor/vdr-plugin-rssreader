/*
 * tools.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <iconv.h>
#include <vdr/tools.h>
#include "common.h"

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

char *striphtml(char *s)
{
  char *c, t = 0, *r;
  c = s;
  r = s;
  while (*s != '\0') {
    if (*s == '<')
       t++;
    else if (*s == '>')
       t--;
    else if (t < 1)
       *(c++) = *s;
    s++;
    }
  *c = '\0';
  return r;
}

char *stripspaces(char *str)
{
  char tmp[MAXLONGTEXTLEN];
  char *ptr;

  if (str == NULL)
     return str;

  strncpy(tmp, str, MAXLONGTEXTLEN);
  strcpy(str, "");
  ptr = strtok(tmp, " \n\t\r\x3F");
  while (ptr) {
     strcat(str, ptr);
     strcat(str, " ");
     ptr = strtok(NULL, " \n\t\r\x3F");
     }
  return str;
}

