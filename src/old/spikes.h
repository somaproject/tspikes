#ifndef SPIKES_H
#define SPIKES_H

#include <math.h>

#include <list>
#include <vector>

typedef std::vector<int> PointList; 

class SpikePoint 
{
 public: 
  SpikePoint(unsigned int ts, PointList & points, int trigChan);
  unsigned int getts(void);
  int getPoint(int chan); 
  int getTrigChan(void); 
 private:
  PointList points_; 
  unsigned int ts_; 
  int trigChan_; 
};

inline
 unsigned int SpikePoint::getts(void)
{
  return ts_; 
}

inline
 int SpikePoint::getPoint(int chan) 
{
  return points_[chan];
}

inline
 int SpikePoint::getTrigChan(void)
{
  return trigChan_;
}

typedef std::vector<SpikePoint> SpikePointList; 
    
#endif //SPIKES_H
