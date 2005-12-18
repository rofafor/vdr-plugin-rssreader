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

cMenuItem::cMenuItem(const char *Title, const char *Date, const char *Desc, const char *Target)
:cOsdMenu(tr("RSS item"))
{
  asprintf(&text, "\n%s\n\n%s\n\n%s\n\n%s",
           *Date ? strdup(Date) : tr("<no date available>"),
           *Title ? strdup(Title) : tr("<no title available>"),
           *Desc ? strdup(Desc) : tr("<no description available>"),
           *Target ? strdup(Target) : tr("<no target available>"));
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
  }
  return state;
}

eOSState cItemsMenu::ShowDetails(void)
{
  cItem *item = (cItem *)Items.Get(Current());
  return AddSubMenu(new cMenuItem(item->GetTitle(), item->GetDate(), item->GetDesc(), item->GetTarget()));
}

// --- cStreamsMenu -----------------------------------------------------

cStreamsMenu::cStreamsMenu(void)
:cOsdMenu(tr("Select RSS stream"))
{
  for (cRssItem *Item = RssItems.First(); Item; Item = RssItems.Next(Item))
    Add(new cOsdItem(Item->Title()));
  Display();
}

eOSState cStreamsMenu::Select(void)
{
  cRssItem *i = (cRssItem *)RssItems.Get(Current());
  if (i) {
     debug("cStreamsMenu::Select(): downloading and parsing '%s'", i->Title());
     if (rss_downloader(i->Url()) && rss_parser(RSSTEMPFILE)) {
        return AddSubMenu(new cItemsMenu);
     } else {
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
  }
  return state;
}
