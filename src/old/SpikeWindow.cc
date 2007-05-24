#include "SpikeWindow.h"
#include <iostream>
using namespace std; 

SpikeWindow::SpikeWindow(SpikePointList* spl) :
  sv1_(0, 0, 200, 200, 2000.0, 1, 2), 
  sv2_(0, 0, 200, 200, 2000.0, 1, 3),
  sv3_(0, 0, 200, 200, 2000.0, 1, 4),
  sv4_(0, 0, 200, 200, 2000.0, 2, 3), 
  sv5_(0, 0, 200, 200, 2000.0, 2, 4), 
  sv6_(0, 0, 200, 200, 2000.0, 3, 4), 
  area1_(spl, sv1_, tl_), 
  area2_(spl, sv2_, tl_), 
  area3_(spl, sv3_, tl_), 
  area4_(spl, sv4_, tl_), 
  area5_(spl, sv5_, tl_), 
  area6_(spl, sv6_, tl_), 
  button_("rebuild"), 
  rateAdj_(10, 0, 200), 
  endTimeAdj_(000, 0, 50000000),  
  rateScroll_(rateAdj_), 
  endTimeScroll_(endTimeAdj_), 
  cltable_(3, 2, true) 
  
{
  set_border_width(10);
  cltable_.attach(area1_, 0, 1, 0, 1); 
  cltable_.attach(area2_, 1, 2, 0, 1);
  cltable_.attach(area3_, 2, 3, 0, 1);
  cltable_.attach(area4_, 0, 1, 1, 2); 
  cltable_.attach(area5_, 1, 2, 1, 2); 
  cltable_.attach(area6_, 2, 3, 1, 2); 
  tl_.granularity = 600000;
  tl_.endTime = std::numeric_limits<unsigned int>::max();
  tl_.rate = 20;
  box_.pack_start(cltable_); 
  box_.pack_start(controlbox_); 
  controlbox_.pack_start(rateScroll_);
  controlbox_.pack_start(endTimeScroll_); 
  controlbox_.pack_start(button_); 
  cltable_.show();
  box_.show(); 
  controlbox_.show();
  add(box_); 
  button_.signal_clicked().connect( sigc::mem_fun(*this, &SpikeWindow::buttonclick) );
}

SpikeWindow::~SpikeWindow()
{

}

void SpikeWindow::buttonclick()
{
  tl_.endTime = endTimeAdj_.get_value();
  tl_.rate = rateAdj_.get_value(); 
  
  cout << "setting endTime = " <<  tl_.endTime << " rate = " << tl_.rate 
       << endl; 

  area1_.setTimeline(tl_); 
  area2_.setTimeline(tl_); 
  area3_.setTimeline(tl_); 
  area4_.setTimeline(tl_); 
  area5_.setTimeline(tl_); 
  area6_.setTimeline(tl_); 

  area1_.rebuild(); 
  area2_.rebuild(); 
  area3_.rebuild(); 
  area4_.rebuild(); 
  area5_.rebuild(); 
  area6_.rebuild(); 
}
