#include "filtercombosetting.h"

FilterComboSetting::FilterComboSetting(SomaNetworkCodec * nc, chanset_t chanset, 
				   settingmap_t sm) :
  ComboSetting(nc, chanset, sm)
  
{
  updateSetting(); 
}

int FilterComboSetting::getSettingFromState(const TSpikeChannelState & state)
{
  return state.filtid; 
}

TSpikeChannelState FilterComboSetting::setSettingInState(TSpikeChannelState state, 
						       int setting)
{
  state.filtid = setting; 
  return state; 

}
