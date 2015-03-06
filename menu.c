/*
 * menu.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/plugin.h>
#include <vdr/status.h>
#include "parser.h"
#include "config.h"
#include "log.h"
#include "menu.h"
#include "tools.h"
#include "common.h"

// --- Globals ----------------------------------------------------------

cRssItems RssItems;

// --- cRssItem(s) ------------------------------------------------------

cRssItem::cRssItem()
: titleM(NULL),
  urlM(NULL)
{
}

cRssItem::~cRssItem()
{
  FREE_POINTER(titleM);
  FREE_POINTER(urlM);
}

bool cRssItem::Parse(const char *strP)
{
  const char *p = strchr(strP, ':');
  if (p) {
     long l = p - strP;
     if (l > 0) {
        titleM = MALLOC(char, l + 1);
        stripspace(strn0cpy(titleM, strP, l + 1));
        if (!isempty(titleM)) {
           urlM = stripspace(strdup(skipspace(p + 1)));
           return true;
           }
        }
     }
  return false;
}

cRssItems::cRssItems()
: updatedM(false)
{
}

bool cRssItems::Load(const char *fileNameP)
{
  if (cConfig<cRssItem>::Load(fileNameP, true)) {
     updatedM = true;
     return true;
     }
  return false;
}

bool cRssItems::Updated()
{
  bool result = updatedM;
  updatedM = false;
  return result;
}

// --- cRssMenuItem --------------------------------------------------------

cRssMenuItem::cRssMenuItem(const char *streamP, const char *dateP, const char *titleP, const char *linkP, const char *descriptionP)
: cOsdMenu(*cString::sprintf("%s - %s", tr("RSS item"), streamP)),
  linkM(linkP)
{
  textM = cString::sprintf("\n%s%s%s%s%s%s%s",
                           *dateP ? strdup(dateP) : RssReaderConfig.IsHideElem() ? "" : tr("<no date available>"), (*dateP || !RssReaderConfig.IsHideElem()) ? "\n\n" : "",
                           *titleP ? strdup(titleP) : RssReaderConfig.IsHideElem() ? "" : tr("<no title available>"), (*titleP || !RssReaderConfig.IsHideElem()) ? "\n\n" : "",
                           *descriptionP ? strdup(descriptionP) : RssReaderConfig.IsHideElem() ? "" : tr("<no description available>"), (*descriptionP || !RssReaderConfig.IsHideElem()) ? "\n\n" : "",
                           *linkP ? strdup(linkP) : RssReaderConfig.IsHideElem() ? "" : tr("<no link available>")
                          );
  if (isimage(*linkM))
     typeM = TYPE_IMAGE;
  else if (ismusic(*linkM))
     typeM = TYPE_MUSIC;
  else if (isvideo(*linkM))
     typeM = TYPE_VIDEO;
  else
     typeM = TYPE_NONE;
  SetHelp(NULL, "<<", (typeM != TYPE_NONE) ? ">>" : NULL, NULL);
}

cRssMenuItem::~cRssMenuItem()
{
}

void cRssMenuItem::Display(void)
{
  cOsdMenu::Display();
  debug1("cRssMenuItem::Display(): '%s'\n", *textM);
  DisplayMenu()->SetText(textM, false);
  cStatus::MsgOsdTextItem(textM);
}

eOSState cRssMenuItem::ProcessKey(eKeys keyP)
{
  switch (int(keyP)) {
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:
    case kLeft|k_Repeat:
    case kLeft:
    case kRight|k_Repeat:
    case kRight:
         DisplayMenu()->Scroll(NORMALKEY(keyP) == kUp || NORMALKEY(keyP) == kLeft, NORMALKEY(keyP) == kLeft || NORMALKEY(keyP) == kRight);
         cStatus::MsgOsdTextItem(NULL, NORMALKEY(keyP) == kUp);
         return osContinue;
    default:
         break;
    }

  eOSState state = cOsdMenu::ProcessKey(keyP);

  if (state == osUnknown) {
     switch (keyP) {
       case kGreen:
       case kOk:
            return osBack;
       case kYellow:
            switch (typeM) {
               case TYPE_IMAGE:
                    cPluginManager::CallFirstService("ImagePlayer-1.0", (void *)*linkM);
                    break;
               case TYPE_MUSIC:
                    cPluginManager::CallFirstService("MusicPlayer-1.0", (void *)*linkM);
                    break;
               case TYPE_VIDEO:
                    cPluginManager::CallFirstService("MediaPlayer-1.0", (void *)*linkM);
                    break;
               case TYPE_NONE:
               default:
                    break;
              }
       default:
            state = osContinue;
       }
     state = osContinue;
     }
  return state;
}

// --- cRssItemsMenu --------------------------------------------------------

cRssItemsMenu::cRssItemsMenu(const char *streamP)
: cOsdMenu(*cString::sprintf("%s - %s", tr("Select RSS item"), streamP)),
  streamM(streamP)
{
  for (cItem *rssItem = Parser.Items.First(); rssItem; rssItem = Parser.Items.Next(rssItem))
     Add(new cOsdItem(rssItem->GetTitle()));
  SetHelp(NULL, "<<", ">>", NULL);
  Display();
}

eOSState cRssItemsMenu::ProcessKey(eKeys keyP)
{
  eOSState state = cOsdMenu::ProcessKey(keyP);
  if (state == osUnknown) {
     switch (keyP) {
       case kGreen:
            return osBack;
       case kYellow:
       case kOk:
            return ShowDetails();
       default:
            break;
       }
     state = osContinue;
     }
  return state;
}

eOSState cRssItemsMenu::ShowDetails(void)
{
  cItem *rssItem = reinterpret_cast<cItem *>(Parser.Items.Get(Current()));
  if (rssItem) {
     return AddSubMenu(new cRssMenuItem(*streamM, rssItem->GetDate(), rssItem->GetTitle(), rssItem->GetLink(), rssItem->GetDescription()));
     }
  return osContinue;
}

// --- cRssStreamsMenu -----------------------------------------------------

cRssStreamsMenu::cRssStreamsMenu()
:cOsdMenu(tr("Select RSS stream"))
{
  Setup();
  SetHelp(tr("Button$Load"), "<<", ">>", NULL);
}

void cRssStreamsMenu::Setup(void)
{
  int current = Current();
  Clear();

  for (cRssItem *rssItem = RssItems.First(); rssItem; rssItem = RssItems.Next(rssItem)) {
      cOsdItem *osdItem = new cOsdItem;
      if (!*rssItem->Url())
         osdItem->SetSelectable(false);
      osdItem->SetText(rssItem->Title());
      debug1("StreamsMenu: '%s' : '%s'", rssItem->Title(), rssItem->Url());
      Add(osdItem);
    }

  SetCurrent(Get(current));
  Display();
}

eOSState cRssStreamsMenu::Select(void)
{
  cRssItem *rssItem = reinterpret_cast<cRssItem *>(RssItems.Get(Current()));
  if (rssItem) {
     debug1("cRssStreamsMenu::Select(): downloading and parsing '%s'", rssItem->Title());
     // the following message generates an annoying slowdown 
     //Skins.Message(mtInfo, tr("Loading RSS stream..."));
     switch (Parser.DownloadAndParse(rssItem->Url())) {
       case (cParser::RSS_PARSING_OK):
            return AddSubMenu(new cRssItemsMenu(rssItem->Title()));
       case (cParser::RSS_PARSING_ERROR):
            Skins.Message(mtError, tr("Can't parse RSS stream!"));
            return osContinue;
       case (cParser::RSS_DOWNLOAD_ERROR):
            Skins.Message(mtError, tr("Can't download RSS stream!"));
            return osContinue;
       case (cParser::RSS_UNKNOWN_ERROR):
       default:
            Skins.Message(mtError, tr("Unknown error!"));
            return osContinue;
       }
     }
  return osEnd;
}

eOSState cRssStreamsMenu::ProcessKey(eKeys keyP)
{
  eOSState state = cOsdMenu::ProcessKey(keyP);

  if (RssItems.Updated())
     Setup();

  if (state == osUnknown) {
     switch (keyP) {
       case kRed:
            Skins.Message(mtInfo, tr("Loading configuration file..."));
            RssItems.Load(RssReaderConfig.GetConfigFile());
            Setup();
            Skins.Message(mtInfo, NULL);
            break;
       case kGreen:
            return osBack;
       case kYellow:
       case kOk:
            return Select();
       default:
            break;
       }
     state = osContinue;
     }
  return state;
}
