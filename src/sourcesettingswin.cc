#include <boost/foreach.hpp>
#include "sourcesettingswin.h"
#include <string>

#include "combosetting.h" 
#include "gaincombosetting.h"
#include "filtercombosetting.h"
#include "hpfchecksetting.h"
#include "tholdentrysetting.h"
#include "config.h"

SourceSettingsWin::SourceSettingsWin() :
  pDialog_(0), 
  pTableSourceSettings_(0)
{
  std::string basedir = GLADE_PATH; 
  std::string totalfile = basedir + "/" + "tspikes.glade"; 
  
  try { 
    refXml_ = Gnome::Glade::Xml::create(totalfile.c_str());//, "windowSourceSettings");
  } catch (Gnome::Glade::XmlError & e) {
    refXml_ = Gnome::Glade::Xml::create("tspikes.glade");
  }
  // populate widget pointers
  refXml_->get_widget("dialogSourceSettings", pDialog_); 
  refXml_->get_widget("tableSourceSettings", pTableSourceSettings_); 

}

SourceSettingsWin::~SourceSettingsWin()
{
  BOOST_FOREACH(GainComboSetting * gcs, gcsptrs_) 
    {
      delete gcs; 
    }
  
  BOOST_FOREACH(HPFCheckSetting * hcs, hcsptrs_) 
    {
      delete hcs; 
    } 

  BOOST_FOREACH(FilterComboSetting * fcs,  fcsptrs_)
    {
      delete fcs; 
    }
  
  BOOST_FOREACH(TholdEntrySetting * tes,  tesptrs_)
    {
      delete tes; 
    }

}


void SourceSettingsWin::setCodec(pSomaNetworkCodec_t nc) {
  pSomaNetCodec_ = nc; 
  chanPropList_  = pSomaNetCodec_->getChans(); 
  
}
void SourceSettingsWin::show()
{
  if (pSomaNetCodec_ == 0) {
    throw std::runtime_error("Can't show SourceSettingsWin without attached SomaNetworkCodec"); 
  }
  populate();   
  pDialog_->show(); 
}

void SourceSettingsWin::populate()
{

  // FIXME: Move these to some remote configuration interface
  settingmap_t gainMap; 
  gainMap[0] = "0"; 
  gainMap[100] = "100"; 
  gainMap[200] = "200"; 
  gainMap[500] = "500"; 
  gainMap[1000] = "1000"; 
  gainMap[2000] = "2000"; 
  gainMap[5000] = "5000"; 
  gainMap[10000] = "10000"; 
  
  settingmap_t filterMap; 
  filterMap[0] = "none"; 
  filterMap[1] = "DC-9kHz"; 
  filterMap[2] = "100-200Hz"; 


  // populate the settings -- ugly and a lot of work, but oh well. 
  chanproplist_t::iterator c; 


  // Add "all channel" setting: 
  Gtk::Label * l = new Gtk::Label("All"); 
  pTableSourceSettings_->attach(*l, 0, 1, 1, 2); 	

  chanset_t chansall; 
  for (c = chanPropList_.begin(); c != chanPropList_.end(); c++) {
    chansall.insert(c->chan); 
  }

  GainComboSetting * gcsall = new GainComboSetting(pSomaNetCodec_, chansall, gainMap); 
  pTableSourceSettings_->attach(*gcsall, 1, 2, 1, 2); 
  gcsptrs_.push_back(gcsall); 
  
  HPFCheckSetting * hcsall = new HPFCheckSetting(pSomaNetCodec_, chansall); 
  pTableSourceSettings_->attach(*hcsall, 2, 3, 1, 2); 
  hcsptrs_.push_back(hcsall); 

  FilterComboSetting * fcsall = new FilterComboSetting(pSomaNetCodec_, chansall, filterMap); 
  pTableSourceSettings_->attach(*fcsall, 3, 4, 1, 2); 
  fcsptrs_.push_back(fcsall); 
 
  TholdEntrySetting * tesall = new TholdEntrySetting(pSomaNetCodec_, chansall); 
  pTableSourceSettings_->attach(*tesall, 4, 5, 1, 2); 
  tesptrs_.push_back(tesall); 
 
 
  int rowpos = 2; 
  for (c = chanPropList_.begin(); c != chanPropList_.end(); c++) {
    Gtk::Label * l = new Gtk::Label(c->name); 
    pTableSourceSettings_->attach(*l, 0, 1, rowpos, rowpos+1); 	
    
    // add gain settings
    chanset_t chans; 
    chans.insert(c->chan); 

    GainComboSetting * gcs = new GainComboSetting(pSomaNetCodec_, chans, gainMap); 
    pTableSourceSettings_->attach(*gcs, 1, 2, rowpos, rowpos +1); 
    gcsptrs_.push_back(gcs); 

    HPFCheckSetting * hcs = new HPFCheckSetting(pSomaNetCodec_, chans); 
    pTableSourceSettings_->attach(*hcs, 2, 3, rowpos, rowpos +1); 
    hcsptrs_.push_back(hcs); 

    FilterComboSetting * fcs = new FilterComboSetting(pSomaNetCodec_, chans, filterMap); 
    pTableSourceSettings_->attach(*fcs, 3, 4, rowpos, rowpos +1); 
    fcsptrs_.push_back(fcs); 

    TholdEntrySetting * tes = new TholdEntrySetting(pSomaNetCodec_, chans); 
    pTableSourceSettings_->attach(*tes, 4, 5, rowpos, rowpos+1); 
    tesptrs_.push_back(tes); 
 
    
    rowpos++; 
  }
  pTableSourceSettings_->show_all(); 


}


