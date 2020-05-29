#ifndef APP_H
#define APP_H

#include <gtkmm.h>

#include "predefined.h"

NAMESPACE_BEGIN

class App : public Gtk::Application {
protected:
  App();
  
public:
  static Glib::RefPtr<App> create();

protected:
  // Override default signal handlers:
  void on_startup() override;
  void on_activate() override;
  void on_open(const Gio::Application::type_vec_files& files, const Glib::ustring& hint) override;

private:
  App* create_appwindow();
  void on_hide_window(Gtk::Window* window);
  void on_action_preferences();
  void on_action_quit();
}

NAMESPACE_END

#endif
