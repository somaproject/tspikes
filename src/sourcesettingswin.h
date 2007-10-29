#ifndef SOURCESETTINGSWIN_H
#define SOURCESETTINGSWIN_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <assert.h>

#include <stdlib.h>
#include <gtkmm.h>
#include <libglademm.h>

#include "somanetcodec.h" 


class SourceSettingsWin
// Generic management and configuration for the soma 
// network protocol; takes in a somanetcodec and operates asynchronously
// 
{
 public: 
  SourceSettingsWin(SomaNetworkCodec * pSomaNetCodec); 
  void show(); 

 private:
  SomaNetworkCodec * pSomaNetCodec_; 

  Glib::RefPtr<Gnome::Glade::Xml> refXml_; 
  chanproplist_t chanPropList_; 

  // widgets we grab from the parent
  Gtk::Dialog * pDialog_; 
  Gtk::Table * pTableSourceSettings_; 
  void populate(); 

}; 

#endif // SOURCESETTINGSWIN_H
