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
#include "log.h"
#include "menu.h"
#include "setup.h"

#if defined(APIVERSNUM) && APIVERSNUM < 20200
#error "VDR-2.2.0 API version or greater is required!"
#endif

#ifndef GITVERSION
#define GITVERSION ""
#endif

       const char VERSION[]       = "2.2.1" GITVERSION;
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
  virtual bool SetupParse(const char *nameP, const char *valueP);
  virtual bool Service(const char *idP, void *dataP = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *commandP, const char *optionP, int &replyCodeP);
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
  return "  -t <mode>, --trace=<mode>  set the tracing mode\n";
}

bool cPluginRssReader::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  static const struct option long_options[] = {
    { "trace",    required_argument, NULL, 't' },
    { NULL,       no_argument,       NULL,  0  }
    };

  cString server;
  int c;
  while ((c = getopt_long(argc, argv, "t:", long_options, NULL)) != -1) {
    switch (c) {
      case 't':
           RssReaderConfig.SetTraceMode(strtol(optarg, NULL, 0));
           break;
      default:
           return false;
      }
    }
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

bool cPluginRssReader::SetupParse(const char *nameP, const char *valueP)
{
  // Parse your own setup parameters and store their values.
  if (!strcasecmp(nameP, "HideMenu"))
     RssReaderConfig.SetHideMenu(atoi(valueP));
  else if (!strcasecmp(nameP, "HideElem"))
     RssReaderConfig.SetHideElem(atoi(valueP));
  else if (!strcasecmp(nameP, "UseProxy"))
     RssReaderConfig.SetUseProxy(atoi(valueP));
  else if (!strcasecmp(nameP, "HttpProxy"))
     RssReaderConfig.SetHttpProxy(valueP);
  else
     return false;

  return true;
}

bool cPluginRssReader::Service(const char *idP, void *dataP)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginRssReader::SVDRPHelpPages(void)
{
  static const char *HelpPages[] = {
    "LOAD\n"
    "    Load RSS feed configuration file.",
    "TRAC [ <mode> ]\n"
    "    Gets and/or sets used tracing mode.\n",
    NULL
    };
  return HelpPages;
}

cString cPluginRssReader::SVDRPCommand(const char *commandP, const char *optionP, int &replyCodeP)
{
  if (strcasecmp(commandP, "LOAD") == 0) {
     if (!RssItems.Load(RssReaderConfig.GetConfigFile())) {
        replyCodeP = 550; // Requested action not taken
        return cString("Configuration file not found!");
        }
     return cString("Configuration file loaded");
     }
  else if (strcasecmp(commandP, "TRAC") == 0) {
     if (optionP && *optionP)
        RssReaderConfig.SetTraceMode(strtol(optionP, NULL, 0));
     return cString::sprintf("Tracing mode: 0x%04X\n", RssReaderConfig.GetTraceMode());
     }
  return NULL;
}

VDRPLUGINCREATOR(cPluginRssReader); // Don't touch this!
