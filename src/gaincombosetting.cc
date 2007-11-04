#include "gaincombosetting.h"

GainComboSetting::GainComboSetting(SomaNetworkCodec * nc, chanset_t chanset, 
				   settingsmap_t sm) 
  
{
  
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
