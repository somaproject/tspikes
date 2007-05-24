#include "clusterview.h"
#include <stdexcept>

ClusterView::ClusterView(int voriginx, int voriginy, 
			 int width, int height, 
			 int nvxperpixel, int nvyperpixel, 
			 int xchan, int ychan) :
  vOriginX_ (voriginx),
  vOriginY_ (voriginy), 
  width_ (width), 
  height_ (height), 
  nvXPerPixel_ (nvxperpixel),
  nvYPerPixel_ (nvyperpixel), 
  xChan_ (xchan), 
  yChan_ (ychan) 
{
  if ( width <= 0) 
    throw std::logic_error("ClusterView width must be greater than 0"); 

  if ( height <= 0) 
    throw std::logic_error("ClusterView height must be greater than 0"); 


}
