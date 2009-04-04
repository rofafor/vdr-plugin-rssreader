/*
 * tools.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "tools.h"
#include "common.h"

// --- Static -----------------------------------------------------------

#define ELEMENTS(x) (sizeof(x) / sizeof(x[0]))

struct conv_table {
  const char *from;
  const char *to;
};

static struct conv_table pre_conv_table[] =
{
  // 'to' field must be smaller than 'from'
  {"<br />",   "\n"}
};

// Conversion page: http://www.ltg.ed.ac.uk/~richard/utf-8.cgi

static struct conv_table post_conv_table[] =
{
  // 'to' field must be smaller than 'from'
  {"&quot;",   "\x22"},
  {"&amp;",    "\x26"},
  {"&#38;",    "\x26"},
  {"&#038;",   "\x26"},
  {"&#039;",   "\x27"},
  {"&#40;",    "\x28"},
  {"&#41;",    "\x29"},
  {"&#58;",    "\x3a"},
  {"&lt;",     "\x3c"},
  {"&gt;",     "\x3e"},
  {"&#91;",    "\x5b"},
  {"&#93;",    "\x5d"},
  {"&nbsp;",   "\xc2\xa0"},
  {"&#160;",   "\xc2\xa0"},
  {"&#176;",   "\xc2\xb0"},
  {"&acute;",  "\xc2\xb4"},
  {"&#180;",   "\xc2\xb4"},
  {"&#196;",   "\xc3\x84"},
  {"&Auml;",   "\xc3\x84"},
  {"&#197;",   "\xc3\x85"},
  {"&#214;",   "\xc3\x96"},
  {"&Ouml;",   "\xc3\x96"},
  {"&#220;",   "\xc3\x9c"},
  {"&Uuml;",   "\xc3\x9c"},
  {"&#223;",   "\xc3\x9f"},
  {"&szlig;",  "\xc3\x9f"},
  {"&#226;",   "\xc3\xa2"},
  {"&acirc;",  "\xc3\xa2"},
  {"&#228;",   "\xc3\xa4"},
  {"&auml;",   "\xc3\xa4"},
  {"&#229;",   "\xc3\xa5"},
  {"&#231;",   "\xc3\xa7"},
  {"&ccedil;", "\xc3\xa7"},
  {"&#233;",   "\xc3\xa9"},
  {"&eacute;", "\xc3\xa9"},
  {"&#234;",   "\xc3\xaa"},
  {"&#246;",   "\xc3\xb6"},
  {"&ouml;",   "\xc3\xb6"},
  {"&#252;",   "\xc3\xbc"},
  {"&uuml;",   "\xc3\xbc"},
  {"&ndash;",  "\xe2\x80\x93"},
  {"&#8211;",  "\xe2\x80\x93"},
  {"&#8216;",  "\xe2\x80\x98"},
  {"&#8217;",  "\xe2\x80\x99"},
  {"&#8220;",  "\xe2\x80\x9c"},
  {"&#8221;",  "\xe2\x80\x9d"},
  {"&#8222;",  "\xe2\x80\x9e"},
  {"&#8243;",  "\xe2\x80\xb3"},
  {"&euro;",   "\xe2\x82\xac"},
  {"&#8364;",  "\xe2\x82\xac"},
  {"\n\n",     "\n"}, // let's also strip multiple linefeeds
};

static char *htmlcharconv(char *str, struct conv_table *conv, unsigned int elem)
{
  if (str && conv) {
     for (unsigned int i = 0; i < elem; ++i) {
        char *ptr = strstr(str, conv[i].from);
        while (ptr) {
           int of = ptr - str;
           int l  = strlen(str);
           int l1 = strlen(conv[i].from);
           int l2 = strlen(conv[i].to);
           if (l2 > l1) {
              error("htmlcharconv(): cannot reallocate string");
              return str;
              }
           if (l2 != l1)
              memmove(str + of + l2, str + of + l1, l - of - l1 + 1);
           strncpy(str + of, conv[i].to, l2);
           ptr = strstr(str, conv[i].from);
           }
        }
     return str;
     }
  return NULL;
}

// --- General functions ------------------------------------------------

char *striphtml(char *str)
{
  if (str) {
     char *c, t = 0, *r;
     str = htmlcharconv(str, pre_conv_table, ELEMENTS(pre_conv_table));
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
     return htmlcharconv(r, post_conv_table, ELEMENTS(post_conv_table));
     }
  return NULL;
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

bool isimage(const char *text)
{
  if (endswith(text, ".jpg") || endswith(text, ".gif") || endswith(text, ".png"))
     return true; 
  return false;
}

bool isvideo(const char *text)
{
  if (endswith(text, ".mpg") || endswith(text, ".avi") ||  endswith(text, ".ts"))
     return true;
  return false;
}

bool ismusic(const char *text)
{
  if (endswith(text, ".mp3") || endswith(text, ".wav") || endswith(text, ".ogg"))
     return true;
  return false;
}
