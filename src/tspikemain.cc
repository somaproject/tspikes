#include "tspikewin.h"


int main(int argc, char** argv)
{
  Gtk::Main kit(argc, argv);

  Gtk::GL::init(argc, argv);
  
  Network net; 

  TSpikeWin tspikewin(&net);

  kit.run(tspikewin);

  return 0;
}
