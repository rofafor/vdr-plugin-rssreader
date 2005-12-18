/*
 * menu.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __RSSREADER_MENU_H
#define __RSSREADER_MENU_H

#include <vdr/osdbase.h>
#include <vdr/menuitems.h>
#include <vdr/interface.h>

// --- cMenuItem --------------------------------------------------------

class cMenuItem : public cOsdMenu {
private:
  char *text;
public:
  cMenuItem(const char *Title, const char *Date, const char *Desc, const char *Link);
  virtual ~cMenuItem();
  virtual void Display(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

// --- cItemsMenu -------------------------------------------------------

class cItemsMenu: public cOsdMenu {
private:
  eOSState ShowDetails(void);
public:
  cItemsMenu(void);
  virtual eOSState ProcessKey(eKeys Key);
};

// --- cStreamsMenu -----------------------------------------------------

class cStreamsMenu: public cOsdMenu {
public:
  cStreamsMenu(void);
  virtual eOSState ProcessKey(eKeys Key);
  virtual eOSState Select(void);
};

#endif // __RSSREADER_MENU_H
