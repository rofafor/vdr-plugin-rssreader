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
  char *titleM;
  char *urlM;
public:
  cRssItem();
  virtual ~cRssItem();
  bool Parse(const char *strP);
  const char *Title(void) { return titleM; }
  const char *Url(void) { return urlM; }
};

class cRssItems : public cConfig<cRssItem> {
private:
  bool updatedM;
public:
  cRssItems();
  virtual bool Load(const char *fileNameP);
  bool Updated(void);
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
  int typeM;
  cString textM;
  cString linkM;
public:
  cRssMenuItem(const char *streamP, const char *dateP, const char *titleP, const char *linkP, const char *descriptionP);
  virtual ~cRssMenuItem();
  virtual void Display(void);
  virtual eOSState ProcessKey(eKeys keyP);
  };

// --- cRssItemsMenu -------------------------------------------------------

class cRssItemsMenu: public cOsdMenu {
private:
  cString streamM;
  eOSState ShowDetails(void);
public:
  cRssItemsMenu(const char *streamP);
  virtual eOSState ProcessKey(eKeys keyP);
};

// --- cRssStreamsMenu -----------------------------------------------------

class cRssStreamsMenu: public cOsdMenu {
private:
  void Setup(void);
  eOSState Select(void);
public:
  cRssStreamsMenu();
  virtual eOSState ProcessKey(eKeys keyP);
};

#endif // __RSSREADER_MENU_H
