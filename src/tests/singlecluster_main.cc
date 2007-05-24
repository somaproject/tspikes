#include "singlecluster.h" 
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm.h>
#include <iostream>

using namespace std; 

int main(int argc, char** argv)
{

  cout << "what?" << endl; 
  Gtk::Main kit(argc, argv);
  
  cout << "and more " << endl; 
  
  SingleCluster sc; 
  cout << "still more" << endl; 

  
  Gtk::Main::run(sc); 
  cout << "Are we done yet" << endl;
  
  
  
  
  return 0;
}
