/*
 * setup.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __RSSREADER_SETUP_H
#define __RSSREADER_SETUP_H

#include <vdr/menuitems.h>
#include "common.h"

class cRssReaderSetup : public cMenuSetupPage
{
private:
  int hideMenuM;
  int hideElemM;
  int useProxyM;
  char httpProxyM[256];
  char configFileM[256];
  cVector<const char*> helpM;
  void Setup(void);

protected:
  virtual eOSState ProcessKey(eKeys keyP);
  virtual void Store(void);

public:
  cRssReaderSetup();
  ~cRssReaderSetup();
};

#endif // __RSSREADER_SETUP_H
