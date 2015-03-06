/*
 * config.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __RSSREADER_CONFIG_H
#define __RSSREADER_CONFIG_H

#include "tools.h"

class cRssReaderConfig
{
private:
  int traceModeM;
  int hideMenuM;
  int hideElemM;
  int useProxyM;
  char httpProxyM[NAME_MAX];
  char configFileM[NAME_MAX];

public:
  enum eTraceMode {
    eTraceModeNormal  = 0x0000,
    eTraceModeDebug1  = 0x0001,
    eTraceModeDebug2  = 0x0002,
    eTraceModeDebug3  = 0x0004,
    eTraceModeDebug4  = 0x0008,
    eTraceModeDebug5  = 0x0010,
    eTraceModeDebug6  = 0x0020,
    eTraceModeDebug7  = 0x0040,
    eTraceModeDebug8  = 0x0080,
    eTraceModeDebug9  = 0x0100,
    eTraceModeDebug10 = 0x0200,
    eTraceModeDebug11 = 0x0400,
    eTraceModeDebug12 = 0x0800,
    eTraceModeDebug13 = 0x1000,
    eTraceModeDebug14 = 0x2000,
    eTraceModeDebug15 = 0x4000,
    eTraceModeDebug16 = 0x8000,
    eTraceModeMask    = 0xFFFF
  };
  cRssReaderConfig();
  unsigned int GetTraceMode(void) const { return traceModeM; }
  bool IsTraceMode(eTraceMode modeP) const { return (traceModeM & modeP); }
  bool IsHideElem(void) const { return !!hideElemM; }
  bool IsHideMenu(void) const { return !!hideMenuM; }
  bool IsUseProxy(void) const { return !!useProxyM; }
  const char *GetHttpProxy(void) { return httpProxyM; }
  const char *GetConfigFile(void) { return configFileM; }

  void SetTraceMode(unsigned int modeP) { traceModeM = (modeP & eTraceModeMask); }
  void SetHideElem(unsigned int onoffP) { hideElemM = onoffP; }
  void SetHideMenu(unsigned int onoffP) { hideMenuM = onoffP; }
  void SetUseProxy(unsigned int onoffP) { useProxyM = onoffP; }
  void SetHttpProxy(const char *strP);
  void SetConfigFile(const char *strP);
};

extern cRssReaderConfig RssReaderConfig;

#endif // __RSSREADER_CONFIG_H
