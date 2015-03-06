/*
 * tools.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "tools.h"
#include "common.h"

// --- Static -----------------------------------------------------------

struct conv_table {
  const char *from;
  const char *to;
};

static struct conv_table preConversionTableS[] =
{
  // 'to' field must be smaller than 'from'
  {"<br />",   "\n"}
};

// Conversion page: http://www.ltg.ed.ac.uk/~richard/utf-8.cgi

static struct conv_table postConversionTableS[] =
{
  // 'to' field must be smaller than 'from'
  {"&quot;",   "\x22"},
  {"&#34;",    "\x22"},
  {"&amp;",    "\x26"},
  {"&#38;",    "\x26"},
  {"&#038;",   "\x26"},
  {"&#39;",    "\x27"},
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

static char *htmlcharconv(char *strP, struct conv_table *convP, unsigned int elemP)
{
  if (strP && convP) {
     for (unsigned int i = 0; i < elemP; ++i) {
        char *ptr = strstr(strP, convP[i].from);
        while (ptr) {
           long of = ptr - strP;
           size_t l  = strlen(strP);
           size_t l1 = strlen(convP[i].from);
           size_t l2 = strlen(convP[i].to);
           if (l2 > l1) {
              error("htmlcharconv(): cannot reallocate string");
              return strP;
              }
           if (l2 != l1)
              memmove(strP + of + l2, strP + of + l1, l - of - l1 + 1);
           strncpy(strP + of, convP[i].to, l2);
           ptr = strstr(strP, convP[i].from);
           }
        }
     return strP;
     }
  return NULL;
}

// --- General functions ------------------------------------------------

char *striphtml(char *strP)
{
  if (strP) {
     char *c, t = 0, *r;
     strP = htmlcharconv(strP, preConversionTableS, ELEMENTS(preConversionTableS));
     c = strP;
     r = strP;
     while (*strP != '\0') {
       if (*strP == '<')
          t++;
       else if (*strP == '>')
          t--;
       else if (t < 1)
          *(c++) = *strP;
       strP++;
       }
     *c = '\0';
     return htmlcharconv(r, postConversionTableS, ELEMENTS(postConversionTableS));
     }
  return NULL;
}

void *myrealloc(void *ptrP, size_t sizeP)
{
  /* There might be a realloc() out there that doesn't like reallocing
     NULL pointers, so we take care of it here */
  if (ptrP)
     return realloc(ptrP, sizeP);
  else
     return malloc(sizeP);
}

bool isimage(const char *textP)
{
  if (endswith(textP, ".jpg") || endswith(textP, ".gif") || endswith(textP, ".png"))
     return true;
  return false;
}

bool isvideo(const char *textP)
{
  if (endswith(textP, ".mpg") || endswith(textP, ".avi") ||  endswith(textP, ".ts"))
     return true;
  return false;
}

bool ismusic(const char *textP)
{
  if (endswith(textP, ".mp3") || endswith(textP, ".wav") || endswith(textP, ".ogg"))
     return true;
  return false;
}
