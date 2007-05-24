#include "clusterarea.h"

ClusterArea::ClusterArea(SpikePointList * spl, SpikeView sv, SpikeTimeline tl) :
  cd_(spl, sv, tl)
  
{
  // get_window() would return 0 because the Gdk::Window has not yet been realized
  // So we can only allocate the colors here - the rest will happen in on_realize().
  Glib::RefPtr<Gdk::Colormap> colormap = get_default_colormap ();

  blue_ = Gdk::Color("blue");
  red_ = Gdk::Color("red");

  colormap->alloc_color(blue_);
  colormap->alloc_color(red_);


}

ClusterArea::~ClusterArea()
{
}

void ClusterArea::on_realize()
{
  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  // Now we can allocate any additional resources we need
  Glib::RefPtr<Gdk::Window> window = get_window();
  gc_ = Gdk::GC::create(window);
  //window->set_background(red_);
  //window->clear();
  //gc_->set_foreground(blue_);
  set_size_request(200, 200); 
  rebuild(); 
  
}

void ClusterArea::rebuild()
{
  cd_.rebuild(); 
  
  queue_draw_area (0 ,0, 200, 200); 
}

void ClusterArea::setTimeline(SpikeTimeline tl) {
  cd_.setTimeline(tl); 
}

bool ClusterArea::on_expose_event(GdkEventExpose* /* event */)
{
  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  cd_.renderToWindow(window, gc_); 

  
  return true;
}


