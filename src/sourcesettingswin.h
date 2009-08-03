#ifndef SOURCESETTINGSWIN_H
#define SOURCESETTINGSWIN_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <assert.h>

#include <stdlib.h>
#include <gtkmm.h>
#include <libglademm.h>

#include <map>
#include <list>
#include <set>

#include "somanetcodec.h" 

// widget data types for mapping between channel and widget, and vice
// versa

typedef Glib::RefPtr<Gtk::Widget> pwidget_t; 
typedef int channel_t; 
typedef std::set<channel_t> chanset_t; 

// forward declarations
class GainComboSetting; 
class HPFCheckSetting; 
class FilterComboSetting; 
class TholdEntrySetting;



class SourceSettingsWin
// Generic management and configuration for the soma 
// network protocol; takes in a somanetcodec and operates asynchronously
// 
{
 public: 
  SourceSettingsWin(); 
  ~SourceSettingsWin(); 
  void setCodec(pSomaNetworkCodec_t sc); 

  void show(); 

 private:
  pSomaNetworkCodec_t pSomaNetCodec_; 

  Glib::RefPtr<Gnome::Glade::Xml> refXml_; 
  chanproplist_t chanPropList_; 

  // widgets we grab from the parent
  Gtk::Dialog * pDialog_; 
  Gtk::Table * pTableSourceSettings_; 

  
  void populate(); 
  // pointers that we need to keep a hold on
  std::list<GainComboSetting *> gcsptrs_;
  std::list<HPFCheckSetting *> hcsptrs_; 
  std::list<FilterComboSetting *> fcsptrs_; 
  std::list<TholdEntrySetting *> tesptrs_; 
 
}; 

#endif // SOURCESETTINGSWIN_H
