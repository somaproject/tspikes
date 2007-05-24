#include "clusterbox.h"
#include <iostream>
#include <sstream>

ClusterBox::ClusterBox(SpikeBuffer & spikes, int chanA, int chanB)
  : spikes_(spikes),
    time_(0)
{
  
  chanA_ = chanA - 1; 
  chanB_ = chanB - 1; 
  
  winWidth_ = 200;
  winHeight_= 200; 

  currentBuf_ = Gdk::Pixbuf::create(
      Gdk::COLORSPACE_RGB, false, 8, winWidth_, winHeight_);


  // size request
  set_size_request(winWidth_, winHeight_);
  
  pixData_ = new guint8[winWidth_*winHeight_*3];  

}


ClusterBox::~ClusterBox()
{
      
  delete [] pixData_; 
}

inline int ClusterBox::findPoint(int x, int y) 
{
  return y * winWidth_ * 3 + x*3; 
}  

inline void ClusterBox::plotSpike(int x, int y, int chan, int cpos) 
{
  int pos =y * winWidth_ * 3 + x*3; 
  int rval(0), gval(0), bval(0); 
  switch(chan)
    {
    case 0 :
      bval = cpos; 
      break;
    case 1: 
      gval = cpos; 
      break;
    case 2:
      rval = cpos; 
      break; 
    default:
      rval = cpos; 
      gval = cpos;
      break; 
    }
  pixData_[pos] = min(255, pixData_[pos] + rval); 
  pixData_[pos+1] = min(255, pixData_[pos+1] + gval); 
  pixData_[pos+2] = min(255, pixData_[pos+2] + bval); 
}  

void ClusterBox::on_realize()
{
  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

    
  // Now we can allocate any additional resources we need
  Glib::RefPtr<Gdk::Window> window = get_window();
  gc_ = Gdk::GC::create(window);
  window->set_background(cBlack_);
  window->clear();
  gc_->set_foreground(cWhite_);

}

bool ClusterBox::on_expose_event(GdkEventExpose* event)
{
    // This is where we draw on the window

  const int rowstride = currentBuf_->get_rowstride();
  const size_t offset = rowstride * event->area.y + 3 * event->area.x;

  get_window()->draw_rgb_image_dithalign(
      get_style()->get_black_gc(),
      event->area.x, event->area.y, event->area.width, event->area.height,
      Gdk::RGB_DITHER_NORMAL,
      &currentBuf_->get_pixels()[offset], rowstride,
      event->area.x, event->area.y);

  return true;

}


void ClusterBox::drawClusterBox(Glib::RefPtr<Gdk::Window> & window)
{
  // create text
  std::ostringstream ost;
  ost << chanA_ + 1 << ':' << chanB_ + 1;
  Glib::ustring usChans;
  usChans = ost.str(); 

  Glib::RefPtr<Pango::Layout> pChanTextLayout =  create_pango_layout(usChans);
 
  Pango::FontDescription chanTextFont;
  chanTextFont.set_size(80000); 
  pChanTextLayout->set_font_description(chanTextFont);

  Pango::Rectangle inkRect, logicalRect; 
  pChanTextLayout->get_extents(inkRect, logicalRect); 

  // get window 
  int width, height; 
  window->get_size(width, height); 
  // now, we try and draw this in the middle, something something
  
  gc_->set_foreground(cChanText_);
  window->draw_layout(gc_, (width-(logicalRect.get_width()/Pango::SCALE))/2, 
		      (height - (logicalRect.get_height()/Pango::SCALE))/2, 
		      pChanTextLayout); 
  

}

void ClusterBox::drawZoomBox(Glib::RefPtr<Gdk::Window> & window)
{
  // cluster box is always in upper 4th:
  
  gc_->set_foreground(cWhite_);

  int width, height; 
  window->get_size(width, height); 
  
  int boxwidth = int(width * 0.25); 
  int boxheight = int(height * 0.25); 
  window->draw_rectangle(gc_, 0, width -boxwidth , 0, 
			 boxwidth, boxheight); 

  // draw zoomed-area
  
  int selWidth = int((clustWin_.x2 - clustWin_.x1) / 
		     (totalWin_.x2 - totalWin_.x1) * boxwidth);
  int selHeight = int((clustWin_.y2 - clustWin_.y1) / 
		      (totalWin_.y2 - totalWin_.y1) * boxheight);
  int cluXpos = int((clustWin_.x1 / (totalWin_.x2 - totalWin_.x1))*boxwidth); 
  int cluYpos = int((clustWin_.y1 / (totalWin_.y2 - totalWin_.y1))*boxheight); 

  gc_->set_foreground(cRed_); 
  window->draw_rectangle(gc_, 0, width -boxwidth+cluXpos, 
			 boxheight - cluYpos-selHeight, 
			 selWidth, selHeight); 
  
  
}

void ClusterBox::setClustWin(WinView & win)
{
  clustWin_ = win; 
}

void ClusterBox::setTotalWin(WinView & win)
{
  totalWin_ = win; 
}


void ClusterBox::drawSpikes()
{
  // we draw all the spikes here
  // get the necessary iterators for the circular buffer
  SpikeBuffer::iterator sp; 

  for(int i = 0; i < winHeight_; i++)
    for(int j = 0; j < winWidth_; j++)
      {
	pixData_[j*3  + i*3*winWidth_] = 0; 
	pixData_[j*3  + i*3*winWidth_ + 1] = 0; 
	pixData_[j*3  + i*3*winWidth_ + 2]  = 0; 
      }

  
  for (sp = spikes_.begin(); sp != spikes_.end(); sp++) 
    {
      // plot main window spikes
      //SpikePoint * psp = sp; 
      long timedif = time_ - sp->getTime(); 

      if ((sp->getTime() <= time_)   // don't plot points from the future
	  and (timedif <= MAXTIME))  // nor spikes that are too old 
      {
	  
	  float chanApeak = sp->getPeak(chanA_);
	  float chanBpeak = sp->getPeak(chanB_); 

	  int colorpos = int(float(timedif)/(MAXTIME)*256.0); 
	  	       
	  colorpos = min(colorpos, 255); 
	  colorpos = max(colorpos, 0); 
	  colorpos = (255 - colorpos); 
	  
	  int zBoxWidth = int(winWidth_ * 0.25); 
	  int zBoxHeight = int(winHeight_ * 0.25); 
	  
	  int zXpos = (winWidth_ - zBoxWidth) 
	    + int((chanApeak / (totalWin_.x2 - totalWin_.x1))*zBoxWidth);

	  int zYpos = int((chanBpeak / (totalWin_.y2 - totalWin_.y1))*zBoxHeight);
	  //window->draw_point(gc_, zXpos, zBoxHeight - zYpos); 
	  

	    // plot main clusterbox spikes
	  if ( ((chanApeak <= clustWin_.x2) and (chanApeak > clustWin_.x1)) and
	       ((chanBpeak <= clustWin_.y2) and (chanBpeak > clustWin_.y1)))
	    {
	      
	      int xpos = int((chanApeak / 
			      (clustWin_.x2 - clustWin_.x1))*winWidth_); 
	      int ypos = int((chanBpeak / 
			      (clustWin_.y2 - clustWin_.y1))*winHeight_); 
	      plotSpike(xpos, winHeight_-ypos, sp->getTriggerChan(), colorpos);
	      
	   }
	  
	  }
      
    }
  currentBuf_ = Gdk::Pixbuf::create_from_data
    (pixData_, Gdk::COLORSPACE_RGB, false, 8, winWidth_, winHeight_, winWidth_*3);  
					
}


void ClusterBox::setTime(unsigned int newTime)
{
  time_ = newTime; 
  drawSpikes(); 

  queue_draw(); 
}
