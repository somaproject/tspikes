// -*- C++ -*-
/*
 * share-lists.cc:
 * Simple display list sharing example.
 *
 * written by Naofumi Yasufuku  <naofumi@users.sourceforge.net>
 */

#include <iostream>
#include <cstdlib>
#define GL_GLEXT_PROTOTYPES
#include <gtkmm.h>

#include <gtkglmm.h>

#ifdef G_OS_WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif



#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

using namespace std; 

///////////////////////////////////////////////////////////////////////////////
//
// Simple OpenGL scene base class.
//
///////////////////////////////////////////////////////////////////////////////

class SimpleGLScene : public Gtk::GL::DrawingArea
{
public:
  explicit SimpleGLScene(const Glib::RefPtr<const Gdk::GL::Config>& config);

  SimpleGLScene(const Glib::RefPtr<const Gdk::GL::Config>& config,
                const Glib::RefPtr<const Gdk::GL::Context>& share_list);

  virtual ~SimpleGLScene();

protected:
  virtual void init_gl();

protected:
  virtual bool on_configure_event(GdkEventConfigure* event);
  virtual bool on_expose_event(GdkEventExpose* event);

public:
  // Gtk::Widget::realize() is protected, so that ...
  void realize() { Gtk::GL::DrawingArea::realize(); }

public:
  void set_light_diffuse(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
  { m_LightDiffuse[0] = r; m_LightDiffuse[1] = g; m_LightDiffuse[2] = b; m_LightDiffuse[3] = a; }

  void set_light_position(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
  { m_LightPosition[0] = x; m_LightPosition[1] = y; m_LightPosition[2] = z; m_LightPosition[3] = w; }

protected:
  GLfloat m_LightDiffuse[4];
  GLfloat m_LightPosition[4];

};

SimpleGLScene::SimpleGLScene(const Glib::RefPtr<const Gdk::GL::Config>& config)
  : Gtk::GL::DrawingArea(config)
{
  set_light_diffuse(1.0, 1.0, 1.0, 1.0);
  set_light_position(1.0, 1.0, 1.0, 0.0);
}

SimpleGLScene::SimpleGLScene(const Glib::RefPtr<const Gdk::GL::Config>& config,
                             const Glib::RefPtr<const Gdk::GL::Context>& share_list)
  : Gtk::GL::DrawingArea(config, share_list)
{
  set_light_diffuse(1.0, 1.0, 1.0, 1.0);
  set_light_position(1.0, 1.0, 1.0, 0.0);
}

SimpleGLScene::~SimpleGLScene()
{
}

void SimpleGLScene::init_gl()
{
  glLightfv(GL_LIGHT0, GL_DIFFUSE, m_LightDiffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, m_LightPosition);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);

  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClearDepth(1.0);

  glViewport(0, 0, get_width(), get_height());

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, 1.0, 1.0, 10.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 3.0,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0);
  glTranslatef(0.0, 0.0, -3.0);
}

bool SimpleGLScene::on_configure_event(GdkEventConfigure* event)
{
  //
  // Get GL::Window.
  //

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  //
  // GL calls.
  //

  // *** OpenGL BEGIN ***
  if (!glwindow->gl_begin(get_gl_context()))
    return false;

  glViewport(0, 0, get_width(), get_height());

  glwindow->gl_end();
  // *** OpenGL END ***

  return true;
}

bool SimpleGLScene::on_expose_event(GdkEventExpose* event)
{
  //
  // Get GL::Window.
  //

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  //
  // GL calls.
  //

  // *** OpenGL BEGIN ***
  if (!glwindow->gl_begin(get_gl_context()))
    return false;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glCallList(1);

  // Swap buffers.
  if (glwindow->is_double_buffered())
    glwindow->swap_buffers();
  else
    glFlush();

  glwindow->gl_end();
  // *** OpenGL END ***

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
// SimpleGLSceneMain (creates display lists)
//
///////////////////////////////////////////////////////////////////////////////

class SimpleGLSceneMain : public SimpleGLScene
{
public:
  explicit SimpleGLSceneMain(const Glib::RefPtr<const Gdk::GL::Config>& config);
  virtual ~SimpleGLSceneMain();

protected:
  virtual void on_realize();

};

SimpleGLSceneMain::SimpleGLSceneMain(const Glib::RefPtr<const Gdk::GL::Config>& config)
  : SimpleGLScene(config)
{
}

SimpleGLSceneMain::~SimpleGLSceneMain()
{
}

void SimpleGLSceneMain::on_realize()
{
  // We need to call the base on_realize()
  SimpleGLScene::on_realize();

  //
  // Get GL::Window.
  //

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  //
  // GL calls.
  //

  // *** OpenGL BEGIN ***
  if (!glwindow->gl_begin(get_gl_context()))
    return;

  // Create display list #1
  GLUquadricObj* qobj = gluNewQuadric();
  gluQuadricDrawStyle(qobj, GLU_FILL);
  glNewList(1, GL_COMPILE);
  gluSphere(qobj, 1.0, 20, 20);
  glEndList();


  // try and create a VBO
    // create buffer objects

  GLuint bufarray[1]; 
  glGenBuffersARB(1, bufarray);
  GLuint b = bufarray[0]; 
  cout << " We have acquired buffer object " << b << endl; 
  //glBindBufferARB(GL_ARRAY_BUFFER, glBufferObject_); 
  //glBufferDataARB(GL_ARRAY_BUFFER, sizeof(SpikePoint)*L*3,
  //	       &((*spb_)[0]), GL_STATIC_DRAW);
  


  // Initialize rendering context
  init_gl();

  glwindow->gl_end();
  // *** OpenGL END ***
}


///////////////////////////////////////////////////////////////////////////////
//
// SimpleGLSceneSub (shares display lists)
//
///////////////////////////////////////////////////////////////////////////////

class SimpleGLSceneSub : public SimpleGLScene
{
public:
  SimpleGLSceneSub(const Glib::RefPtr<const Gdk::GL::Config>& config,
                   const Glib::RefPtr<const Gdk::GL::Context>& share_list);
  virtual ~SimpleGLSceneSub();

protected:
  virtual void on_realize();

};

SimpleGLSceneSub::SimpleGLSceneSub(const Glib::RefPtr<const Gdk::GL::Config>& config,
                                   const Glib::RefPtr<const Gdk::GL::Context>& share_list)
  : SimpleGLScene(config, share_list)
{
}

SimpleGLSceneSub::~SimpleGLSceneSub()
{
}

void SimpleGLSceneSub::on_realize()
{
  // We need to call the base on_realize()
  SimpleGLScene::on_realize();

  //
  // Get GL::Window.
  //

  Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();

  //
  // GL calls.
  //

  // check for vertex buffers
  for (int i = 0; i < 4; i++) {
    if (glIsBuffer(i) ) {
      cout << i << " is a vertex buffer" << endl;
    } else {
      cout << i  << " is not a vertex buffer"  << endl; 
    }
  }

  
  // *** OpenGL BEGIN ***
  if (!glwindow->gl_begin(get_gl_context()))
    return;

  // Initialize rendering context
  init_gl();

  glwindow->gl_end();
  // *** OpenGL END ***
}


///////////////////////////////////////////////////////////////////////////////
//
// The application class.
//
///////////////////////////////////////////////////////////////////////////////

class Simple : public Gtk::Window
{
public:
  explicit Simple(const Glib::RefPtr<const Gdk::GL::Config>& config);
  virtual ~Simple();

protected:
  // signal handlers:
  void on_button_quit_clicked();

protected:
  // member widgets:
  Gtk::VBox m_VBox;
  SimpleGLScene *m_GLScene1;
  SimpleGLScene *m_GLScene2;
  SimpleGLScene *m_GLScene3;
  Gtk::Button m_ButtonQuit;
};

Simple::Simple(const Glib::RefPtr<const Gdk::GL::Config>& config)
  : m_VBox(false, 10), m_ButtonQuit("Quit")
{
  //
  // Top-level window.
  //

  set_title("share-lists");

  // Get automatically redrawn if any of their children changed allocation.
  set_reallocate_redraws(true);
  // Set border width.
  set_border_width(10);

  add(m_VBox);

  //
  // OpenGL scene #1 (SimpleGLSceneMain: creates display lists)
  //

  m_GLScene1 = new SimpleGLSceneMain(config);
  m_GLScene1->set_size_request(120, 120);
  m_GLScene1->set_light_diffuse(1.0, 0.0, 0.0, 0.0); // red

  m_VBox.pack_start(*m_GLScene1);

  //
  // Get OpenGL rendering context.
  //

  m_GLScene1->realize();
  Glib::RefPtr<Gdk::GL::Context> glcontext = m_GLScene1->get_gl_context();

  //
  // OpenGL scene #2 (SimpleGLSceneSub: shares display lists)
  //

  m_GLScene2 = new SimpleGLSceneSub(config, glcontext);
  m_GLScene2->set_size_request(120, 120);
  m_GLScene2->set_light_diffuse(1.0, 1.0, 0.0, 0.0); // yellow

  m_VBox.pack_start(*m_GLScene2);

  //
  // OpenGL scene #3 (SimpleGLSceneSub: shares display lists)
  //

  m_GLScene3 = new SimpleGLSceneSub(config, glcontext);
  m_GLScene3->set_size_request(120, 120);
  m_GLScene3->set_light_diffuse(0.0, 1.0, 0.0, 0.0); // green

  m_VBox.pack_start(*m_GLScene3);

  //
  // Simple quit button.
  //

  m_ButtonQuit.signal_clicked().connect(
    sigc::mem_fun(*this, &Simple::on_button_quit_clicked));

  m_VBox.pack_start(m_ButtonQuit, Gtk::PACK_SHRINK, 0);

  //
  // Show window.
  //

  show_all();
}

Simple::~Simple()
{
  delete m_GLScene1;
  delete m_GLScene2;
  delete m_GLScene3;
}

void Simple::on_button_quit_clicked()
{
  Gtk::Main::quit();
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
  // Configure OpenGL-capable visual.
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

  Simple simple(glconfig);

  kit.run(simple);

  return 0;
}
