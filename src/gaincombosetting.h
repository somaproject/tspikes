#ifndef GAINCOMBOSETTING_H
#define GAINCOMBOSETTING_H

#include <gtkmm.h>
#include "somanetcodec.h" 
#include "sourcesettingswin.h"
#include "combosetting.h" 


class GainComboSetting : public ComboSetting;
{
 public:
  GainComboSetting(SomaNetworkCodec * nc, chanset_t chanset, 
		   settingsmap_t sm); 
  
 private:
  int getSettingFromState(const TSpikeChannelState &);
  TSpikeChannelState  setSettingInState(TSpikeChannelState, int); 


}; 
  


#endif 
