#include "sourcesettingswin.h"

#include "combosetting.h" 
#include "gaincombosetting.h"
#include "filtercombosetting.h"
#include "hpfchecksetting.h"

SourceSettingsWin::SourceSettingsWin(SomaNetworkCodec * pSomaNetCodec) :
  pSomaNetCodec_(pSomaNetCodec), 
  pDialog_(0), 
  pTableSourceSettings_(0)
{
  chanPropList_  = pSomaNetCodec_->getChans(); 
  
  refXml_ = Gnome::Glade::Xml::create("tspikes.glade");//, "windowSourceSettings");
  
  // populate widget pointers
  refXml_->get_widget("dialogSourceSettings", pDialog_); 
  refXml_->get_widget("tableSourceSettings", pTableSourceSettings_); 

}

void SourceSettingsWin::show()
{
  std::cout << "presenting window" << std::endl; 
  populate();   
  pDialog_->run(); 
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
  
  HPFCheckSetting * hcsall = new HPFCheckSetting(pSomaNetCodec_, chansall); 
  pTableSourceSettings_->attach(*hcsall, 2, 3, 1, 2); 

  FilterComboSetting * fcsall = new FilterComboSetting(pSomaNetCodec_, chansall, filterMap); 
  pTableSourceSettings_->attach(*fcsall, 3, 4, 1, 2); 
 
 
  int rowpos = 2; 
  for (c = chanPropList_.begin(); c != chanPropList_.end(); c++) {
    Gtk::Label * l = new Gtk::Label(c->name); 
    pTableSourceSettings_->attach(*l, 0, 1, rowpos, rowpos+1); 	
    
    // add gain settings
    chanset_t chans; 
    chans.insert(c->chan); 

    GainComboSetting * gcs = new GainComboSetting(pSomaNetCodec_, chans, gainMap); 
    pTableSourceSettings_->attach(*gcs, 1, 2, rowpos, rowpos +1); 

    HPFCheckSetting * hcs = new HPFCheckSetting(pSomaNetCodec_, chans); 
    pTableSourceSettings_->attach(*hcs, 2, 3, rowpos, rowpos +1); 

    FilterComboSetting * fcs = new FilterComboSetting(pSomaNetCodec_, chans, filterMap); 
    pTableSourceSettings_->attach(*fcs, 3, 4, rowpos, rowpos +1); 

    
    rowpos++; 
  }
  pTableSourceSettings_->show_all(); 


}


