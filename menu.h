/*
 * menu.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __RSSREADER_MENU_H
#define __RSSREADER_MENU_H

#include <vdr/osdbase.h>
#include <vdr/tools.h>

// --- cRssItem(s) ------------------------------------------------------

class cRssItem : public cListObject {
private:
  char *title;
  char *url;
public:
  cRssItem();
  virtual ~cRssItem();
  bool Parse(const char *s);
  const char *Title(void) { return title; }
  const char *Url(void) { return url; }
};

class cRssItems : public cConfig<cRssItem> {
private:
  bool updated;
public:
  cRssItems();
  virtual bool Load(const char *filename);
  bool Updated();
};

extern cRssItems RssItems;

// --- cRssMenuItem --------------------------------------------------------

class cRssMenuItem : public cOsdMenu {
private:
  enum {
    TYPE_NONE,
    TYPE_IMAGE,
    TYPE_VIDEO,
    TYPE_MUSIC
  };
  int type;
  cString text;
  cString link;
public:
  cRssMenuItem(const char *Stream, const char *Date, const char *Title, const char *Link, const char *Description);
  virtual ~cRssMenuItem();
  virtual void Display(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

// --- cRssItemsMenu -------------------------------------------------------

class cRssItemsMenu: public cOsdMenu {
private:
  cString stream;
  eOSState ShowDetails(void);
public:
  cRssItemsMenu(const char *Stream);
  virtual eOSState ProcessKey(eKeys Key);
};

// --- cRssStreamsMenu -----------------------------------------------------

class cRssStreamsMenu: public cOsdMenu {
private:
  void Setup(void);
  eOSState Select(void);
public:
  cRssStreamsMenu();
  virtual eOSState ProcessKey(eKeys Key);
};

#endif // __RSSREADER_MENU_H
