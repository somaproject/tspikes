#include <boost/foreach.hpp>

#include "clusterviewcontroller.h"



ClusterViewController::ClusterViewController(clusterViews_t  & cvs) : 
  cviews_(cvs)
{
  
  BOOST_FOREACH(ClusterView & cv, cviews_) {
    
    
//     cv.xViewChangeRequestSignal().connect(sigc::bind(sigc::mem_fun(this, 
// 								   &ClusterViewController::extractAbs), 
// 						     sigc::mem_fun(cv, &ClusterView::setXView))); 
    
//     cv.yViewChangeRequestSignal().connect(sigc::bind(sigc::mem_fun(this, 
// 								   &ClusterViewController::extractAbs), 
// 						     sigc::mem_fun(cv, &ClusterView::setYView))); 

    cv.xViewChangeRequestSignal().connect(sigc::mem_fun(this, 
							&ClusterViewController::globalFractionX));

    
    cv.yViewChangeRequestSignal().connect(sigc::mem_fun(this, 
							&ClusterViewController::globalFractionY)); 
    
  }
  

}

void ClusterViewController::extractFraction(float abs, float frac, boost::function<void (float)> tgt)
{
  tgt(frac); 

}

void ClusterViewController::extractAbs(float abs, float frac, boost::function<void (float)> tgt)
{
  tgt(abs); 

}


void ClusterViewController::globalFractionX(float abs, float frac)
{

  BOOST_FOREACH(ClusterView & cv, cviews_) {
    cv.setXViewFraction(frac); 
  }

}

void ClusterViewController::globalFractionY(float abs, float frac)
{

  BOOST_FOREACH(ClusterView & cv, cviews_) {
    cv.setYViewFraction(frac); 
  }

}
