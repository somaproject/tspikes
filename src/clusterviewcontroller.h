#ifndef __TSPIKES_CLUSTERVIEWCONTROLLER_H__
#define __TSPIKES_CLUSTERVIEWCONTROLLER_H__

#include <list>
#include <clusterview.h>
#include <boost/function.hpp>

class ClusterViewController
{
  /*
    The Cluster View Controller ties the cluster views
    together such that their zooming is synchronized. 

   */ 
  
public:

  ClusterViewController(clusterViews_t & cvs);
  


private:
  clusterViews_t & cviews_; 

  void extractFraction(float abs, float frac, boost::function<void (float)> tgt); 
  void extractAbs(float abs, float frac, boost::function<void (float)> tgt); 
  
  void globalFractionX(float abs, float frac); 
  void globalFractionY(float abs, float frac); 

}; 



#endif // __TSPIKES_CLUSTERVIEWCONTROLLER_H__
