/*
 * menu.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
 
#include <vdr/plugin.h>
#include <vdr/menu.h>
#include <vdr/status.h>

#include "menu.h"
#include "i18n.h"
#include "parser.h"
#include "item.h"
#include "common.h"

// --- cMenuItem --------------------------------------------------------

cMenuItem::cMenuItem(const char *Title, const char *Date, const char *Desc, const char *Link)
:cOsdMenu(tr("RSS item"))
{
  asprintf(&text, "\n%s\n\n%s\n\n%s\n\n%s",
           *Date ? strdup(Date) : tr("<no date available>"),
           *Title ? strdup(Title) : tr("<no title available>"),
           *Desc ? strdup(Desc) : tr("<no description available>"),
           *Link ? strdup(Link) : tr("<no link available>"));
}

cMenuItem::~cMenuItem()
{
  free(text);
}

void cMenuItem::Display(void)
{
  cOsdMenu::Display();
  debug("cMenuItem::Display(): '%s'\n", text);
  DisplayMenu()->SetText(text, true);
  cStatus::MsgOsdTextItem(text);
}

eOSState cMenuItem::ProcessKey(eKeys Key)
{
  switch (Key) {
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:
    case kLeft|k_Repeat:
    case kLeft:
    case kRight|k_Repeat:
    case kRight:
         DisplayMenu()->Scroll(NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft, NORMALKEY(Key) == kLeft || NORMALKEY(Key) == kRight);
         cStatus::MsgOsdTextItem(NULL, NORMALKEY(Key) == kUp);
         return osContinue;
    default:
         break;
    }

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kOk:
            return osBack;
       default:
            state = osContinue;
       }
     state = osContinue;
     }
  return state;
}

// --- cItemsMenu --------------------------------------------------------

cItemsMenu::cItemsMenu(void)
:cOsdMenu(tr("Select RSS item"))
{
  for (cItem *Item = Items.First(); Item; Item = Items.Next(Item))
     Add(new cOsdItem(Item->GetTitle()));
  Display();
}

eOSState cItemsMenu::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
     switch (Key) {
       case kOk:
            return ShowDetails();
       default:
            break;
       }
     state = osContinue;
     }
  return state;
}

eOSState cItemsMenu::ShowDetails(void)
{
  cItem *item = (cItem *)Items.Get(Current());
  return AddSubMenu(new cMenuItem(item->GetTitle(), item->GetDate(), item->GetDesc(), item->GetLink()));
}

// --- cStreamsMenu -----------------------------------------------------

cStreamsMenu::cStreamsMenu(void)
:cOsdMenu(tr("Select RSS stream"))
{
  for (cRssItem *rssItem = RssItems.First(); rssItem; rssItem = RssItems.Next(rssItem)) {
      cOsdItem *osdItem = new cOsdItem;
      if (!*rssItem->Url())
         osdItem->SetSelectable(false);
      osdItem->SetText(rssItem->Title(), false);
      Add(osdItem);
    }
  Display();
}

eOSState cStreamsMenu::Select(void)
{
  cRssItem *rssItem = (cRssItem *)RssItems.Get(Current());
  if (rssItem) {
     debug("cStreamsMenu::Select(): downloading and parsing '%s'", rssItem->Title());
     if (rss_downloader(rssItem->Url()) && rss_parser(RSSTEMPFILE)) {
        return AddSubMenu(new cItemsMenu);
        }
     else {
        Skins.Message(mtError, tr("Can't parse RSS stream!"));
        return osContinue;
        }
     }
  return osEnd;
}

eOSState cStreamsMenu::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
     switch (Key) {
       case kOk:
            return Select();
       default:
            break;
       }
     state = osContinue;
     }
  return state;
}
