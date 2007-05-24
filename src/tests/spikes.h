#ifndef GLSPIKES_H
#define GLSPIKES_H

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
  PointList points_; 
  unsigned int ts; 
 int p1, p2, p3, p4; 
  int trigChan; 
};

inline
 unsigned int SpikePoint::getts(void)
{
  return ts; 
}

inline
 int SpikePoint::getPoint(int chan) 
{
  return points_[chan];
}

inline
 int SpikePoint::getTrigChan(void)
{
  return trigChan;
}

typedef std::vector<SpikePoint> SpikePointList; 
    
#endif //GLSPIKES_H
