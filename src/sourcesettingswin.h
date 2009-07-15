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

class SourceSettingsWin
// Generic management and configuration for the soma 
// network protocol; takes in a somanetcodec and operates asynchronously
// 
{
 public: 
  SourceSettingsWin(); 
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

  
}; 

#endif // SOURCESETTINGSWIN_H
