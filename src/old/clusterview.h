#ifndef CLUSTERVIEW_H
#define CLUSTERVIEW_H

#include "spikes.h"

class ClusterView 
{
 public: 
  ClusterView(int voriginx, int voriginy, int width, int height, 
	      int nvxperpixel, int nvyperpixel, int xchan, int ychan); 
  
  int getX(SpikePoint & sp); 
  int getY(SpikePoint & sp);  
  int getWidth(); 
  int getHeight(); 

  int getVoriginX() {return vOriginX_;}
  int getVoriginY() {return vOriginY_;}
  
  
 private:
  int vOriginX_; 
  int vOriginY_;
  int width_; 
  int height_; 
  int nvXPerPixel_; 
  int nvYPerPixel_; 
  int xChan_; 
  int yChan_; 
  

}; 

inline
int ClusterView::getX(SpikePoint & sp) {
  
  int val = sp.getPoint(xChan_); 
  return (val - vOriginX_) / nvXPerPixel_; 

}

inline
int ClusterView::getY(SpikePoint & sp) {

  int val = sp.getPoint(yChan_); 
  return (val - vOriginY_) / nvYPerPixel_; 

}

inline 
int ClusterView::getWidth() {
  return width_; 
}

inline 
int ClusterView::getHeight() {
  return height_; 
}

#endif // CLUSTERVIEW_H
