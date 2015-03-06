/*
 * rssreader.c: RSS Reader plugin for the Video Disk Recorder
 *
 */

#include <getopt.h>
#include <vdr/config.h>
#include <vdr/menu.h>
#include <vdr/plugin.h>

#include "config.h"
#include "common.h"
#include "menu.h"
#include "setup.h"

#if defined(APIVERSNUM) && APIVERSNUM < 20200
#error "VDR-2.2.0 API version or greater is required!"
#endif

#ifndef GITVERSION
#define GITVERSION ""
#endif

static const char VERSION[]       = "2.2.1" GITVERSION;
static const char DESCRIPTION[]   = trNOOP("RSS Reader for OSD");
static const char MAINMENUENTRY[] = trNOOP("RSS Reader");

class cPluginRssReader : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginRssReader(void);
  virtual ~cPluginRssReader();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void) {}
  virtual cString Active(void) { return NULL; }
  virtual const char *MainMenuEntry(void) { return (RssReaderConfig.IsHideMenu() ? NULL : tr(MAINMENUENTRY)); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginRssReader::cPluginRssReader(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginRssReader::~cPluginRssReader()
{
  // Clean up after yourself!
}

const char *cPluginRssReader::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginRssReader::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginRssReader::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  RssReaderConfig.SetConfigFile(*AddDirectory(ConfigDirectory(PLUGIN_NAME_I18N), RSSREADER_CONF));
  return true;
}

bool cPluginRssReader::Start(void)
{
  // Start any background activities the plugin shall perform.
  if (!RssItems.Load(RssReaderConfig.GetConfigFile()))
     error("configuration file '" RSSREADER_CONF "' not found!");
  return true;
}

void cPluginRssReader::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginRssReader::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginRssReader::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cRssStreamsMenu();
}

cMenuSetupPage *cPluginRssReader::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cPluginRssReaderSetup();
}

bool cPluginRssReader::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if (!strcasecmp(Name, "HideMenu"))
     RssReaderConfig.SetHideMenu(atoi(Value));
  else if (!strcasecmp(Name, "HideElem"))
     RssReaderConfig.SetHideElem(atoi(Value));
  else if (!strcasecmp(Name, "UseProxy"))
     RssReaderConfig.SetUseProxy(atoi(Value));
  else if (!strcasecmp(Name, "HttpProxy"))
     RssReaderConfig.SetHttpProxy(Value);
  else
     return false;

  return true;
}

bool cPluginRssReader::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginRssReader::SVDRPHelpPages(void)
{
  static const char *HelpPages[] = {
    "LOAD\n"
    "    Load RSS feed configuration file.",
    NULL
    };
  return HelpPages;
}

cString cPluginRssReader::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  if (strcasecmp(Command, "LOAD") == 0) {
     if (!RssItems.Load(RssReaderConfig.GetConfigFile())) {
        ReplyCode = 550; // Requested action not taken
        return cString("Configuration file not found!");
        }
     return cString("Configuration file loaded");
     }
  return NULL;
}

VDRPLUGINCREATOR(cPluginRssReader); // Don't touch this!
