// -*- C++ -*-
/*
 * 3-D gear wheels.  This program is in the public domain.
 *
 * Brian Paul
 */

/* Conversion to gtkglextmm by Naofumi Yasufuku */

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>

#include <gtkmm.h>
#define GL_GLEXT_PROTOTYPES

#include <gtkglmm.h>

#ifdef G_OS_WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

using namespace std; 

struct SpikePoint 
{
  GLfloat x;
  GLfloat y; 
  GLfloat z; 
  GLfloat w;
  GLfloat t; 
};

struct SpikeColor 
{
  GLbyte r, g, b, a;
};

typedef std::vector<SpikePoint> SpikePointVector; 

int L = 10000; 

///////////////////////////////////////////////////////////////////////////////
//
// OpenGL frame buffer configuration utilities.
//
///////////////////////////////////////////////////////////////////////////////

struct GLConfigUtil
{
  static void print_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig,
                              const char* attrib_str,
                              int attrib,
                              bool is_boolean);

  static void examine_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig);
};

//
// Print a configuration attribute.
//
void GLConfigUtil::print_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig,
                                   const char* attrib_str,
                                   int attrib,
                                   bool is_boolean)
{
  int value;

  if (glconfig->get_attrib(attrib, value))
    {
      std::cout << attrib_str << " = ";
      if (is_boolean)
        std::cout << (value == true ? "true" : "false") << std::endl;
      else
        std::cout << value << std::endl;
    }
  else
    {
      std::cout << "*** Cannot get "
                << attrib_str
                << " attribute value\n";
    }
}

//
// Print configuration attributes.
//
void GLConfigUtil::examine_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig)
{
  std::cout << "\nOpenGL visual configurations :\n\n";

  std::cout << "glconfig->is_rgba() = "
            << (glconfig->is_rgba() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->is_double_buffered() = "
            << (glconfig->is_double_buffered() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->is_stereo() = "
            << (glconfig->is_stereo() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->has_alpha() = "
            << (glconfig->has_alpha() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->has_depth_buffer() = "
            << (glconfig->has_depth_buffer() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->has_stencil_buffer() = "
            << (glconfig->has_stencil_buffer() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->has_accum_buffer() = "
            << (glconfig->has_accum_buffer() ? "true" : "false")
            << std::endl;

  std::cout << std::endl;

  print_gl_attrib(glconfig, "Gdk::GL::USE_GL",           Gdk::GL::USE_GL,           true);
  print_gl_attrib(glconfig, "Gdk::GL::BUFFER_SIZE",      Gdk::GL::BUFFER_SIZE,      false);
  print_gl_attrib(glconfig, "Gdk::GL::LEVEL",            Gdk::GL::LEVEL,            false);
  print_gl_attrib(glconfig, "Gdk::GL::RGBA",             Gdk::GL::RGBA,             true);
  print_gl_attrib(glconfig, "Gdk::GL::DOUBLEBUFFER",     Gdk::GL::DOUBLEBUFFER,     true);
  print_gl_attrib(glconfig, "Gdk::GL::STEREO",           Gdk::GL::STEREO,           true);
  print_gl_attrib(glconfig, "Gdk::GL::AUX_BUFFERS",      Gdk::GL::AUX_BUFFERS,      false);
  print_gl_attrib(glconfig, "Gdk::GL::RED_SIZE",         Gdk::GL::RED_SIZE,         false);
  print_gl_attrib(glconfig, "Gdk::GL::GREEN_SIZE",       Gdk::GL::GREEN_SIZE,       false);
  print_gl_attrib(glconfig, "Gdk::GL::BLUE_SIZE",        Gdk::GL::BLUE_SIZE,        false);
  print_gl_attrib(glconfig, "Gdk::GL::ALPHA_SIZE",       Gdk::GL::ALPHA_SIZE,       false);
  print_gl_attrib(glconfig, "Gdk::GL::DEPTH_SIZE",       Gdk::GL::DEPTH_SIZE,       false);
  print_gl_attrib(glconfig, "Gdk::GL::STENCIL_SIZE",     Gdk::GL::STENCIL_SIZE,     false);
  print_gl_attrib(glconfig, "Gdk::GL::ACCUM_RED_SIZE",   Gdk::GL::ACCUM_RED_SIZE,   false);
  print_gl_attrib(glconfig, "Gdk::GL::ACCUM_GREEN_SIZE", Gdk::GL::ACCUM_GREEN_SIZE, false);
  print_gl_attrib(glconfig, "Gdk::GL::ACCUM_BLUE_SIZE",  Gdk::GL::ACCUM_BLUE_SIZE,  false);
  print_gl_attrib(glconfig, "Gdk::GL::ACCUM_ALPHA_SIZE", Gdk::GL::ACCUM_ALPHA_SIZE, false);

  std::cout << std::endl;
}


class SpikeWindow : public Gtk::GL::DrawingArea
{
public:
  explicit SpikeWindow(bool is_sync, 
		       int xaxis, int yaxis, 
		       SpikePointVector * spb,
		       const Glib::RefPtr<const Gdk::GL::Config>& config);
  explicit SpikeWindow(bool is_sync, 
		       int xaxis, int yaxis, 
		       SpikePointVector * spb,
		       const Glib::RefPtr<const Gdk::GL::Config>& config,
		       const Glib::RefPtr<const Gdk::GL::Context>& share_list);

  virtual ~SpikeWindow();

protected:
  virtual bool on_idle();

protected:
  // signal handlers:
  virtual void on_realize();
  virtual bool on_configure_event(GdkEventConfigure* event);
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_map_event(GdkEventAny* event);
  virtual bool on_unmap_event(GdkEventAny* event);
  virtual bool on_visibility_notify_event(GdkEventVisibility* event);
  float m_pos; 
  string m_name; 
  int xaxis_, yaxis_; 
  GLuint  glBufferObject_;
  SpikePointVector* spb_; 
public:
  // Invalidate whole window.
  void invalidate() {
    get_window()->invalidate_rect(get_allocation(), false);
  }
  void realize() { Gtk::GL::DrawingArea::realize(); }

  // Update window synchronously (fast).
  void update()
  { get_window()->process_updates(false); }

protected:

public:

protected:
  // OpenGL scene related variables:

  bool m_IsSync;
  int m_N; 
  int m_skip; 
  GLuint gpuProg_;
protected:
  // frame rate evaluation stuff:
  Glib::Timer m_Timer;
  int m_Frames;
};

SpikeWindow::SpikeWindow(bool is_sync, int xaxis, int yaxis, 
			 SpikePointVector * spb,
			 const Glib::RefPtr<const Gdk::GL::Config>& config)
  :       
  Gtk::GL::DrawingArea(config), 
  xaxis_(xaxis), 
  yaxis_(yaxis),
  spb_(spb),
  m_IsSync(is_sync),
  m_skip(0),
  m_Frames(0)
{
  //
  // Configure OpenGL-capable visual.
  //


//   Glib::RefPtr<Gdk::GL::Config> glconfig;

//   // Try double-buffered visual
//   glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |
//                                      Gdk::GL::MODE_DEPTH  |
//                                      Gdk::GL::MODE_DOUBLE);
//   if (!glconfig)
//     {
//       std::cerr << "*** Cannot find the double-buffered visual.\n"
//                 << "*** Trying single-buffered visual.\n";

//       // Try single-buffered visual
//       glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |
//                                          Gdk::GL::MODE_DEPTH);
//       if (!glconfig)
//         {
//           std::cerr << "*** Cannot find any OpenGL-capable visual.\n";
//           std::exit(1);
//         }
//     }

//   // print frame buffer attributes.
//   GLConfigUtil::examine_gl_attrib(glconfig);

//   //
//   // Set OpenGL-capability to the widget.
//   //

//   set_gl_capability(glconfig);

//   // Add events.
   add_events(Gdk::VISIBILITY_NOTIFY_MASK);
}


SpikeWindow::SpikeWindow(bool is_sync, int xaxis, int yaxis, 
			 SpikePointVector * spb,
			 const Glib::RefPtr<const Gdk::GL::Config>& config,
			 const Glib::RefPtr<const Gdk::GL::Context>& share_list)
  :       Gtk::GL::DrawingArea(config, share_list), 
	  xaxis_(xaxis), 
     yaxis_(yaxis),
     spb_(spb),
     m_IsSync(is_sync),
     m_skip(0),
     m_Frames(0)
{
  //
  // Configure OpenGL-capable visual.
  //


//   Glib::RefPtr<Gdk::GL::Config> glconfig;

//   // Try double-buffered visual
//   glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |
//                                      Gdk::GL::MODE_DEPTH  |
//                                      Gdk::GL::MODE_DOUBLE);
//   if (!glconfig)
//     {
//       std::cerr << "*** Cannot find the double-buffered visual.\n"
//                 << "*** Trying single-buffered visual.\n";

//       // Try single-buffered visual
//       glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |
//                                          Gdk::GL::MODE_DEPTH);
//       if (!glconfig)
//         {
//           std::cerr << "*** Cannot find any OpenGL-capable visual.\n";
//           std::exit(1);
//         }
//     }

//   // print frame buffer attributes.
//   GLConfigUtil::examine_gl_attrib(glconfig);

//   //
//   // Set OpenGL-capability to the widget.
//   //

//   set_gl_capability(glconfig);

//   // Add events.
  cout << "glcontext = "  << share_list << endl; 

   add_events(Gdk::VISIBILITY_NOTIFY_MASK);
}

SpikeWindow::~SpikeWindow()
{
}




void SpikeWindow::on_realize()
{
  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  //
  // Get GL::Drawable.
  //

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  //
  // GL calls.
  //

  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return;


  // glLightfv(GL_LIGHT0, GL_POSITION, pos);
//   glEnable(GL_CULL_FACE);
//   glEnable(GL_LIGHTING);
//   glEnable(GL_LIGHT0);
  //glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_NORMALIZE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

//   std::cout << "GL_RENDERER   = " << glGetString(GL_RENDERER)   << std::endl;
//   std::cout << "GL_VERSION    = " << glGetString(GL_VERSION)    << std::endl;
//   std::cout << "GL_VENDOR     = " << glGetString(GL_VENDOR)     << std::endl;
//   std::cout << "GL_EXTENSIONS = " << glGetString(GL_EXTENSIONS) << std::endl;




  std::cout << std::endl;
  glOrtho(-1, 1, -1, 1, -1, 1); 
  gldrawable->gl_end();
  // *** OpenGL END ***

  // create buffer objects

  GLuint bufarray[1]; 
  glGenBuffersARB(1, bufarray);
  glBufferObject_ = bufarray[0]; 
  //cout << " We have acquired buffer object " << glBufferObject_ << endl; 
  glBufferObject_ = 1; 
  glBindBufferARB(GL_ARRAY_BUFFER, glBufferObject_); 
  glBufferDataARB(GL_ARRAY_BUFFER, sizeof(SpikePoint)*L,
	       &((*spb_)[0]), GL_STATIC_DRAW);
  

  // check for memory error
  GLenum e = glGetError(); 
  if (e != GL_NO_ERROR) {
    cerr << "GL ERROR!: "; 
    char * errstr = (char *)gluErrorString(e); 
    cerr << errstr << endl; 
    exit(1); 
  }
  // 

  
  
  glVertexPointer(4, GL_FLOAT, sizeof(SpikePoint), 0 ); 
  //glFogCoordPointer(GL_FLOAT, sizeof(SpikePoint),  &((*spb_)[0].t));
  glEnableClientState(GL_VERTEX_ARRAY); 
  //glEnableClientState(GL_FOG_COORD_ARRAY); 
  // Start timer.
}

bool SpikeWindow::on_configure_event(GdkEventConfigure* event)
{
  //
  // Get GL::Drawable.
  //

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();
  
  //
  // GL calls.
  //

  if (!gldrawable->gl_begin(get_gl_context())) {
    cout << "Holy crap another error" << endl;
    return false;
  }
  char * ver = (char *)glGetString(GL_VERSION); 
  if (ver == 0) {
    cout << "JESUS ANOTHER ERROR" << endl;
  } else {
    cout << ver << endl; 
  }
  int param; 

  GLuint vShdr = glCreateShader(GL_VERTEX_SHADER); 
  
  ifstream vShaderFile, fShaderFile;
  //cout << "reading file" << endl; 
  vShaderFile.open("test.vert"); 
  string vShaderSrc = ""; 
  while (vShaderFile and not vShaderFile.eof()) {
    string line; 
    getline(vShaderFile, line); 
    vShaderSrc += line + '\n'; 
  };
  char * program[1];
  program[0] = new char[vShaderSrc.size() + 10];
  strcpy(program[0], vShaderSrc.c_str()); 
  //cout << program[0] << endl; 


  //cout << "reading file...done" << endl; 
  vShaderFile.close();
  glShaderSource(vShdr, 1, (const GLchar**) program, NULL); 
  glCompileShader(vShdr); 

  glGetShaderiv(vShdr, GL_COMPILE_STATUS, &param); 
  if (param == GL_TRUE) {
    //cout << "Successfully compiled vertex shader" << endl; 
  } else {
    cout << "Error compiling vertex shader" << endl; 
    char logmessage[1000];
    int loglen; 
    glGetShaderInfoLog(vShdr, 1000, &loglen, logmessage); 
    cerr << logmessage << endl; 
    exit(1);
  }


    GLuint fShdr = glCreateShader(GL_FRAGMENT_SHADER); 

    //cout << "reading file" << endl; 
   fShaderFile.open("test.frag"); 
   string fShaderSrc; 
   while (fShaderFile and not fShaderFile.eof()) {
     string line; 
     getline(fShaderFile, line); 
     fShaderSrc += line + '\n'; 
   };
  program[0] = new char[fShaderSrc.size() + 1];
  strcpy(program[0], fShaderSrc.c_str()); 
  //cout << program[0] << endl; 

  //cout << "reading file...done" << endl; 
  glShaderSource(fShdr, 1, (const GLchar**) program, NULL); 

  glCompileShader(fShdr); 
  glGetShaderiv(fShdr, GL_COMPILE_STATUS, &param); 
  if (param == GL_TRUE) {
    //cout << "Successfully compiled fragment shader" << endl; 
  } else {
    cout << "Error compiling fragment shader" << endl; 
    char logmessage[1000];
    int loglen; 
    glGetShaderInfoLog(fShdr, 1000, &loglen, logmessage); 
    cerr << logmessage << endl; 
    exit(1);
  }


  gpuProg_ = glCreateProgram(); 
  glAttachShader(gpuProg_, vShdr); 
  glAttachShader(gpuProg_, fShdr); 
  glLinkProgram(gpuProg_);
  glUseProgram(gpuProg_); 
  //cout << "vShdr = " << vShdr << " fShdr = " << fShdr << " gpuProg = " << gpuProg_ << endl; 
  int progLogLen;
  char progLog[1000]; 

  glGetProgramiv(gpuProg_, GL_LINK_STATUS, &param); 
  if (param != GL_TRUE) {
    glGetProgramInfoLog(gpuProg_, 10000, &progLogLen, progLog); 
    cout << progLog << endl; 
    exit(1); 
  }

  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  
  gldrawable->gl_end();
  // *** OpenGL END ***



  return true;
}


bool SpikeWindow::on_expose_event(GdkEventExpose* event)
{
  //
  // Get GL::Drawable.
  //

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  //
  // GL calls.
  //

  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f(1.0, 1.0, 1.0);



//   glBegin(GL_POINTS);
//   for (int i = 0; i < (m_N/4) - 20; i++) {
//     glVertex2f(m_pixelsx[i*4+m_skip], m_pixelsy[i*4+m_skip]); 
//   }
  
//   glEnd(); 

  int BSIZE = 20; 
  GLint vp = glGetUniformLocation(gpuProg_, "axes"); 
  glUniform1i(vp, xaxis_); 
  glDrawArrays(GL_POINTS, 0,
	       L); 

  for (int i = 0; i < 10; i++){
    //cout << (*spb_)[0].x << (*spb_)[0].y << endl; 
  }
  m_skip = (m_skip+1) % BSIZE; 
  //glDisableClientState(GL_VERTEX_ARRAY); 

  // Swap buffers.
  if (gldrawable->is_double_buffered())
    gldrawable->swap_buffers();
  else
    glFlush();

  gldrawable->gl_end();
  // *** OpenGL END ***

  //
  // Print frame rate.
  //
  
  ++m_Frames;

  double seconds = m_Timer.elapsed();
  if (seconds >= 5.0)
    {
      // std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
      std::cout.setf(std::ios::fixed, std::ios::floatfield);
      std::cout.precision(3);
      std::cout << m_Frames << " frames in "
                << seconds << " seconds = "
                << (m_Frames / seconds) << " FPS\n";
      m_Timer.reset();
      m_Frames = 0;
    }

  return true;
}

bool SpikeWindow::on_map_event(GdkEventAny* event)
{

  return true;
}

bool SpikeWindow::on_unmap_event(GdkEventAny* event)
{

  return true;
}

bool SpikeWindow::on_visibility_notify_event(GdkEventVisibility* event)
{
  if (event->state == GDK_VISIBILITY_FULLY_OBSCURED)
    {
      //if (m_ConnectionIdle.connected())
        //m_ConnectionIdle.disconnect();
    }
  else
    {
      //if (!m_ConnectionIdle.connected())
        //m_ConnectionIdle = Glib::signal_idle().connect(
        //  SigC::slot(*this, &SpikeWindow::on_idle), GDK_PRIORITY_REDRAW);
    }

  return true;
}

bool SpikeWindow::on_idle()
{

  // Invalidate the whole window.
    invalidate();

  // Update window synchronously (fast).
  if (m_IsSync)
    update();

  return true;
}


////////////////////////g///////////////////////////////////////////////////////
//
// The application class.
//
///////////////////////////////////////////////////////////////////////////////

class Spikes : public Gtk::Window
{
public:
  explicit Spikes(bool is_sync,
		  const Glib::RefPtr<const Gdk::GL::Config>& config);
  virtual ~Spikes();

protected:
  // signal handlers:
  void on_button_quit_clicked();
  virtual bool on_key_press_event(GdkEventKey* event);

protected:
  // member widgets:
  Gtk::HBox m_HBox1, m_HBox2;
  Gtk::VBox m_VBox; 
  SpikeWindow * m_SpikeWindow1;
  SpikeWindow * m_SpikeWindow2;
  SpikeWindow * m_SpikeWindow3; 
  SpikeWindow * m_SpikeWindow4;
  SpikeWindow * m_SpikeWindow5; 
  SpikeWindow * m_SpikeWindow6;
  Gtk::Button m_ButtonQuit;
  sigc::connection m_ConnectionIdle;
  virtual bool on_idle(void);
  SpikePointVector spv_; 
};

Spikes::Spikes(bool is_sync, 
	       const Glib::RefPtr<const Gdk::GL::Config>& config)
  : m_HBox1(false, 0),
    m_HBox2(false, 0),
    m_VBox(false, 0), 
    m_ButtonQuit("Quit"),
    spv_(L)
{
  set_title("share-lists");

  set_reallocate_redraws(true);
  // Set border width.
  set_border_width(10);


  add(m_VBox);
  m_VBox.pack_start(m_HBox1);
  //  m_VBox.pack_start(m_HBox2);

  
  m_SpikeWindow1 = new SpikeWindow(is_sync, 0, 1, &spv_, config);
  m_SpikeWindow1->set_size_request(200, 200);
  m_HBox1.pack_start(*m_SpikeWindow1);
  //show_all(); 

  m_SpikeWindow1->realize(); 
  Glib::RefPtr<Gdk::GL::Context> glcontext = m_SpikeWindow1->get_gl_context();
  cout << "glcontext = "  << glcontext << endl; 
  
  m_SpikeWindow2 = new SpikeWindow(is_sync, 1, 2, &spv_, config, glcontext);
  m_SpikeWindow3 = new SpikeWindow(is_sync, 2, 3, &spv_, config, glcontext);
//   m_SpikeWindow4 = new SpikeWindow(is_sync, 3, 1, &spv_, config, glcontext);
//   m_SpikeWindow5 = new SpikeWindow(is_sync, 4, 2, &spv_, config, glcontext);
//   m_SpikeWindow6 = new SpikeWindow(is_sync, 5, 3, &spv_, config, glcontext);


  //
  // Top-level window.
  //

  set_title("Spike Window");

  // Get automatically redrawn if any of their children changed allocation.
  //set_reallocate_redraws(true);


  
  // generate fake data:
  SpikePointVector::iterator pspv; 
  for (pspv = spv_.begin(); pspv != spv_.end(); pspv++) {
    (*pspv).x  = 0.0;
    (*pspv).y  = 0.0;
    (*pspv).z  = 0.0; 
    (*pspv).w  = 0.0; 
    (*pspv).t = 0.0; 
  }


  for (int i = 0; i < L; i++) {

    float r = rand() / (RAND_MAX + 1.0); 
    float phi1 = 3.1415*2 * rand() / (RAND_MAX + 1.0) ;
    float phi2 = 3.1415*2 * rand() / (RAND_MAX + 1.0) ;
    float phi3 = 3.1415*2 * rand() / (RAND_MAX + 1.0) ;
    float phi4 = 3.1415*2 * rand() / (RAND_MAX + 1.0) ;

    float p1 = r * cos(phi1); 
    float p2 = r * sin(phi1) * cos(phi2); 
    float p3 = r * sin(phi1) * sin(phi2) * cos(phi3); 
    float p4 = r * sin(phi1) * sin(phi2) * sin(phi3) * sin(phi4); 
    spv_[i].x = p1; 
    spv_[i].y = p2; 
    spv_[i].z = p3; 
    spv_[i].w = p4; 
    spv_[i].t = 1.0;


  }
  
  //
  // Spikes scene.
  //


  m_SpikeWindow2->set_size_request(200, 200);
  m_SpikeWindow3->set_size_request(200, 200);
//   m_SpikeWindow4->set_size_request(200, 200);
//   m_SpikeWindow5->set_size_request(200, 200);
//   m_SpikeWindow6->set_size_request(200, 200);

  //m_HBox1.pack_start(*m_SpikeWindow1);
  m_HBox1.pack_start(*m_SpikeWindow2);
  m_HBox1.pack_start(*m_SpikeWindow3);

//   m_HBox2.pack_start(*m_SpikeWindow4);
//   m_HBox2.pack_start(*m_SpikeWindow5);
//   m_HBox2.pack_start(*m_SpikeWindow6);

  //
  // Simple quit button.
  //

  m_ButtonQuit.signal_clicked().connect(
    sigc::mem_fun(*this, &Spikes::on_button_quit_clicked));

  m_VBox.pack_start(m_ButtonQuit, Gtk::PACK_SHRINK, 0);

  m_ConnectionIdle = Glib::signal_idle().connect(
      sigc::mem_fun(*this, &Spikes::on_idle), GDK_PRIORITY_REDRAW);
  //
  // Show window.
  //

  show_all();
}

bool Spikes::on_idle()
{
  get_window()->invalidate_rect(get_allocation(), true);

  return true; 
}

Spikes::~Spikes()
{
}

void Spikes::on_button_quit_clicked()
{
  Gtk::Main::quit();
}

bool Spikes::on_key_press_event(GdkEventKey* event)
{

  switch (event->keyval)
    {
    case GDK_Escape:
      Gtk::Main::quit();
      break;
    default:
      return true;
    }

  m_SpikeWindow1->invalidate();
  m_SpikeWindow2->invalidate();
  m_SpikeWindow3->invalidate();
  m_SpikeWindow4->invalidate();
  m_SpikeWindow5->invalidate();
  m_SpikeWindow6->invalidate();

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
// Main.
//
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  Gtk::Main kit(argc, argv);

  //
  // Init gtkglextmm.
  //

  Gtk::GL::init(argc, argv);

  //
  // Parse arguments.
  //

  bool is_sync = true;

  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "--async") == 0)
      is_sync = false;
  }


  //
  // Instantiate and run the application.
  //


  Glib::RefPtr<Gdk::GL::Config> glconfig;

  // Try double-buffered visual
  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |
                                     Gdk::GL::MODE_DEPTH  |
                                     Gdk::GL::MODE_DOUBLE);
  if (!glconfig)
    {
      std::cerr << "*** Cannot find the double-buffered visual.\n"
                << "*** Trying single-buffered visual.\n";

      // Try single-buffered visual
      glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |
                                         Gdk::GL::MODE_DEPTH);
      if (!glconfig)
        {
          std::cerr << "*** Cannot find any OpenGL-capable visual.\n";
          std::exit(1);
        }
    }

  //
  // Instantiate and run the application.
  //


  Spikes gears(is_sync, glconfig);

  kit.run(gears);

  return 0;
}
