/*
 * item.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __RSSREADER_ITEM_H
#define __RSSREADER_ITEM_H

#include <vdr/plugin.h>

class cRssItem : public cListObject {
private:
  char *title;
  char *url;
public:
  cRssItem(void);
  virtual ~cRssItem();
  bool Parse(const char *s);
  const char *Title(void) { return title; }
  const char *Url(void) { return url; }
};

class cRssItems : public cConfig<cRssItem> {
public:
  virtual bool Load(const char *filename);
};

extern cRssItems RssItems;

#endif //__RSSREADER_ITEM_H

