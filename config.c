/*
 * config.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "config.h"
#include "common.h"

cRssReaderConfig RssConfig;

cRssReaderConfig::cRssReaderConfig(void)
: hidemenu(0), hideelem(0), tempfile(RSSTEMPFILE)
{
}
