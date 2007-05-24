#ifndef CLUSTERBOX_H
#define CLUSTERBOX_H

#include <gtkmm/drawingarea.h>
#include <gtkmm/main.h>
#include <gdkmm/colormap.h>
#include <gtkmm/window.h>
#include <gtkmm/widget.h>
#include <vector>

#include "spikes.h"





class ClusterBox : public Gtk::DrawingArea
{

 protected:

  WinView clustWin_;
  WinView totalWin_; 
  SpikeBuffer & spikes_;

  // cluster box size; 
  int clusterBoxSize_; // actually in pixels 
  
  // our channels:
  int chanA_, chanB_;
  int winWidth_, winHeight_; 

  // member functions
  void drawClusterSpikes(Gdk::Window*);
  void drawZoomSpikes(Gdk::Window*); 
  void drawClusterBox(Glib::RefPtr<Gdk::Window>&); 
  void drawZoomBox(Glib::RefPtr<Gdk::Window>&); 
  void drawSpikes(); 

  guint8*  pixData_; 
  int findPoint(int, int);
  void plotSpike(int, int, int, int); 
  Glib::RefPtr<Gdk::Pixbuf> currentBuf_;

  Glib::RefPtr<Gdk::GC> gc_;
  // color code:
  Gdk::Color cBlue_, cRed_, cGreen_, cYellow_, cWhite_, cBlack_, cChanText_;
  vector< vector <Gdk::Color> >  chanColor_;
  
  // drawing code:
  void on_realize(); 
  bool on_expose_event(GdkEventExpose*); 
  
  // time
  unsigned int time_; 
  
 public:
  ClusterBox(SpikeBuffer&, int, int);
  ~ClusterBox(); 
  
  void setClustWin(WinView & );
  void setTotalWin(WinView & );
  void setTime(unsigned int); 
};


#endif // CLUSTERBOX_H
