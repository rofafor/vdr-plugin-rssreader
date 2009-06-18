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
  {"&#34;",    "\x22"},
  {"&amp;",    "\x26"},
  {"&#38;",    "\x26"},
  {"&#038;",   "\x26"},
  {"&#039;",   "\x27"},
  {"&#40;",    "\x28"},
  {"&#41;",    "\x29"},
  {"&#58;",    "\x3a"},
  {"&lt;",     "\x3c"},
  {"&#60;",    "\x3c"},
  {"&gt;",     "\x3e"},
  {"&#62;",    "\x3e"},
  {"&#91;",    "\x5b"},
  {"&#93;",    "\x5d"},
  {"&nbsp;",   "\xc2\xa0"},
  {"&#160;",   "\xc2\xa0"},
  {"&deg;",    "\xc2\xb0"},
  {"&#176;",   "\xc2\xb0"},
  {"&acute;",  "\xc2\xb4"},
  {"&#180;",   "\xc2\xb4"},
  {"&Auml;",   "\xc3\x84"},
  {"&#196;",   "\xc3\x84"},
  {"&Aring;",  "\xc3\x85"},
  {"&#197;",   "\xc3\x85"},
  {"&Ouml;",   "\xc3\x96"},
  {"&#214;",   "\xc3\x96"},
  {"&Uuml;",   "\xc3\x9c"},
  {"&#220;",   "\xc3\x9c"},
  {"&szlig;",  "\xc3\x9f"},
  {"&#223;",   "\xc3\x9f"},
  {"&acirc;",  "\xc3\xa2"},
  {"&#226;",   "\xc3\xa2"},
  {"&auml;",   "\xc3\xa4"},
  {"&#228;",   "\xc3\xa4"},
  {"&aring;",  "\xc3\xa5"},
  {"&#229;",   "\xc3\xa5"},
  {"&ccedil;", "\xc3\xa7"},
  {"&#231;",   "\xc3\xa7"},
  {"&eacute;", "\xc3\xa9"},
  {"&#233;",   "\xc3\xa9"},
  {"&ecirc;",  "\xc3\xaa"},
  {"&#234;",   "\xc3\xaa"},
  {"&ouml;",   "\xc3\xb6"},
  {"&#246;",   "\xc3\xb6"},
  {"&uuml;",   "\xc3\xbc"},
  {"&#252;",   "\xc3\xbc"},
  {"&ndash;",  "\xe2\x80\x93"},
  {"&#8211;",  "\xe2\x80\x93"},
  {"&mdash;",  "\xe2\x80\x94"},
  {"&#8212;",  "\xe2\x80\x94"},
  {"&lsquo;",  "\xe2\x80\x98"},
  {"&#8216;",  "\xe2\x80\x98"},
  {"&rsquo;",  "\xe2\x80\x99"},
  {"&#8217;",  "\xe2\x80\x99"},
  {"&sbquo;",  "\xe2\x80\x9a"},
  {"&#8218;",  "\xe2\x80\x9a"},
  {"&ldquo;",  "\xe2\x80\x9c"},
  {"&#8220;",  "\xe2\x80\x9c"},
  {"&rdquo;",  "\xe2\x80\x9d"},
  {"&#8221;",  "\xe2\x80\x9d"},
  {"&bdquo;",  "\xe2\x80\x9e"},
  {"&#8222;",  "\xe2\x80\x9e"},
  {"&prime;",  "\xe2\x80\xb3"},
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
           long of = ptr - str;
           size_t l  = strlen(str);
           size_t l1 = strlen(conv[i].from);
           size_t l2 = strlen(conv[i].to);
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
