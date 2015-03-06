/*
 * setup.c: RSS Reader plugin for the Video Disk Recorder
 *
 */

#include <vdr/menu.h>

#include "config.h"
#include "menu.h"
#include "setup.h"

cRssReaderSetup::cRssReaderSetup()
: hideMenuM(RssReaderConfig.IsHideMenu()),
  hideElemM(RssReaderConfig.IsHideElem()),
  useProxyM(RssReaderConfig.IsUseProxy())
{
  strn0cpy(httpProxyM, RssReaderConfig.GetHttpProxy(), sizeof(httpProxyM));
  SetMenuCategory(mcSetupPlugins);
  Setup();
  SetHelp(tr("Button$Load"), NULL, NULL, NULL);
}

cRssReaderSetup::~cRssReaderSetup()
{
}

void cRssReaderSetup::Setup(void)
{
  int current = Current();

  Clear();
  helpM.Clear();

  Add(new cMenuEditBoolItem(tr("Hide main menu entry"), &hideMenuM));
  helpM.Append(tr("Define whether the main manu entry is hidden."));

  Add(new cMenuEditBoolItem(tr("Hide non-existent elements"), &hideElemM));
  helpM.Append(tr("Define whether all non-existent RSS stream elements are hidden."));

  Add(new cMenuEditBoolItem(tr("Use HTTP proxy server"), &useProxyM));
  helpM.Append(tr("Define whether a HTTP proxy server is used."));

  if (useProxyM) {
     Add(new cMenuEditStrItem( tr("HTTP proxy server"), httpProxyM, sizeof(httpProxyM), tr(FileNameChars)));
     helpM.Append(tr("Define an address and port of the HTTP proxy server:\n\n\"proxy.domain.com:8000\""));
     }

  SetCurrent(Get(current));
  Display();
}

eOSState cRssReaderSetup::ProcessKey(eKeys keyP)
{
  int oldUseProxy = useProxyM;
  eOSState state = cMenuSetupPage::ProcessKey(keyP);

  if (keyP != kNone && (useProxyM != oldUseProxy))
     Setup();

  if (state == osUnknown) {
     switch (keyP) {
       case kRed:
            Skins.Message(mtInfo, tr("Loading configuration file..."));
            RssItems.Load(RssReaderConfig.GetConfigFile());
            Skins.Message(mtInfo, NULL);
            state = osContinue;
            break;
       case kInfo:
            if (Current() < helpM.Size())
               return AddSubMenu(new cMenuText(cString::sprintf("%s - %s '%s'", tr("Help"), trVDR("Plugin"), PLUGIN_NAME_I18N), helpM[Current()]));
            break;
       default:
            break;
       }
     }

  return state;
}

void cRssReaderSetup::Store(void)
{
  // Store values into setup.conf
  SetupStore("HideMenu",  hideMenuM);
  SetupStore("HideElem",  hideElemM);
  SetupStore("UseProxy",  useProxyM);
  SetupStore("HttpProxy", httpProxyM);
  // Update global config
  RssReaderConfig.SetHideMenu(hideMenuM);
  RssReaderConfig.SetHideElem(hideElemM);
  RssReaderConfig.SetUseProxy(useProxyM);
  RssReaderConfig.SetHttpProxy(httpProxyM);
}
