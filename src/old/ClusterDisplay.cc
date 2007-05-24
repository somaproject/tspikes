#include <iostream>
#include <gdkmm.h>
#include <limits>
#include <gdkmm/pixbuf.h>
#include "ClusterDisplay.h"
using namespace std; 

ClusterDisplay::ClusterDisplay(SpikePointList *sl, SpikeView sv, SpikeTimeline tl) :
  sv_ (sv), 
  sl_(sl),
  tl_(tl), 
  frame_(sv.getWidth() * sv.getHeight()*3), 
  slpos_ ( sl_->begin())
{

}

void ClusterDisplay::save() 
{
  // create_from_data (const guint8* data, Colorspace colorspace, bool has_alpha, int bits_per_sample, int width, int height, int rowstride)
  Glib::RefPtr<Gdk::Pixbuf> test = 
    Gdk::Pixbuf::create_from_data((guint8 *) &frame_[0], 
				  Gdk::COLORSPACE_RGB , false, 
				  8, sv_.getWidth(), sv_.getHeight(), 
				  sv_.getWidth()*3); 

  test->save("test.png", "png"); 
  
}

void ClusterDisplay::renderToWindow(const Glib::RefPtr<Gdk::Drawable>& drawable, const Glib::RefPtr<Gdk::GC>& gc)
{
  Glib::RefPtr<Gdk::Pixbuf> test = 
    Gdk::Pixbuf::create_from_data((guint8 *) &frame_[0], 
				  Gdk::COLORSPACE_RGB , false, 
				  8, sv_.getWidth(), sv_.getHeight(), 
				  sv_.getWidth()*3); 
  test->render_to_drawable(drawable, gc, 0, 0, 0, 0, 
			   sv_.getWidth(), sv_.getHeight(),
			   Gdk::RGB_DITHER_NONE, 0, 0); 

  
}
						 
void ClusterDisplay::setTimeline(SpikeTimeline tl) {
  cout << "ClusterDisplay:: set timeline" << endl;
  tl_ = tl; 
}
 
void ClusterDisplay::rebuild() {

  int count (0);
  std::fill(frame_.begin(), frame_.end(), 0); 

  if (sl_->size() == 0) {
    return; 
  }

  SpikePointList::reverse_iterator slrevpos = sl_->rbegin(); 

  int incamount = 255; 
  unsigned int endTime;  
  if (tl_.endTime == std::numeric_limits<unsigned int>::max()) {
    cout << "Here" << endl;
    endTime = (*slrevpos).ts; 
  } else {
    endTime = tl_.endTime;
  }
  cout << "tl_.granularity = " << tl_.granularity << endl;

  while (slrevpos != sl_->rend()) {
    if (slrevpos->ts < endTime ) {
      int x = sv_.getX(*slrevpos); 
      int y = sv_.getY(*slrevpos); 
      int pos = x*3 + y * sv_.getWidth()*3; 
      if (tl_.granularity > 0) {
	incamount = 128 - (endTime - slrevpos->ts) 
	  / tl_.granularity * tl_.rate ; 
	//cout << endTime << ' ' << slrevpos->ts << ' ' 
	//     << tl_.granularity <<  ' ' << tl_.rate <<  ' ' 
	//     << incamount << endl;; 
      }
      
      if (incamount < 0) {
	break;
      }
      
      if (pos < frame_.size()) {
	int rval = frame_[pos];
	
	int gval = frame_[pos + 1];
	int bval = frame_[pos + 2];
	
	switch ( slrevpos->trigChan ) {
	case 1:
	  rval += incamount; 
	  break; 
	case 2:
	  gval += incamount; 
	  break; 
	case 3:
	  bval += incamount; 
	  break; 
	case 4:
	  rval += incamount; 
	  gval += incamount; 
	  break; 
	}
	
	frame_[pos] = std::min(255, rval); 
	frame_[pos+1] = std::min(255, gval); 
	frame_[pos+2] = std::min(255, bval); 
	
      }
     
    }
    slrevpos++; 
  }
  slpos_ = sl_->end();

} 
