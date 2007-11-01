#include "sourcesettingswin.h"
#include "gaincombosetting.h"

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
  // populate the settings -- ugly and a lot of work, but oh well. 
  chanproplist_t::iterator c; 
  int rowpos = 1; 
  for (c = chanPropList_.begin(); c != chanPropList_.end(); c++) {
    std::cout << "adding " << c->name << std::endl; 
    Gtk::Label * l = new Gtk::Label(c->name); 
    pTableSourceSettings_->attach(*l, 0, 1, rowpos, rowpos+1); 	
    
    // add gain settings
    chanset_t chans; 
    chans.insert(c->chan); 
    GainComboSetting * gcs = new GainComboSetting(pSomaNetCodec_, chans); 
    pTableSourceSettings_->attach(*gcs, 1, 2, rowpos, rowpos +1); 

    rowpos++; 
  }
    pTableSourceSettings_->show_all(); 


}


