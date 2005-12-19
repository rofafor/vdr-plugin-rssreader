/*
 * item.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
 
#include <vdr/plugin.h>
#include "item.h"
#include "parser.h"

// --- cRssItem --------------------------------------------------------------

cRssItem::cRssItem(void)
{ 
  title = url = NULL;
}

cRssItem::~cRssItem()
{ 
  free(title);
  free(url);
}

bool cRssItem::Parse(const char *s)                                                                                                          
{
  const char *p = strchr(s, ':');
  if (p) {   
     int l = p - s;
     if (l > 0) {
        title = MALLOC(char, l + 1);
        stripspace(strn0cpy(title, s, l + 1));
        if (!isempty(title)) {
           url = stripspace(strdup(skipspace(p + 1)));
           return true;
           }
        }
     }
  return false;
}

// --- cRssItems -------------------------------------------------------------

cRssItems RssItems;

bool cRssItems::Load(const char *filename)
{
  if (cConfig<cRssItem>::Load(filename, true, true)) {
     return true;
     }
  return false;
}
