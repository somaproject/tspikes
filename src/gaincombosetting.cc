#include "gaincombosetting.h"

GainComboSetting::GainComboSetting(pSomaNetworkCodec_t  nc, chanset_t chanset, 
				   settingmap_t sm) :
  ComboSetting(nc, chanset, sm)
  
{
  updateSetting(); 
}

int GainComboSetting::getSettingFromState(const TSpikeChannelState & state)
{
  return state.gain; 
}

TSpikeChannelState GainComboSetting::setSettingInState(TSpikeChannelState state, 
						       int setting)
{
  state.gain = setting; 
  return state; 

}
