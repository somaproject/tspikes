
#include "spikes.h"

SpikePoint::SpikePoint(unsigned int ts, PointList & points, int trigChan) :
  points_(points), 
  ts_ (ts), 
  trigChan_(trigChan)
{
}
