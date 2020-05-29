#include <iostream>
#include <thread>

#include <gtkmm.h>

#include "predefined.h"

#include "camera.h"

int main(int argc, char* argv[]) {
  auto app =
      Gtk::Application::create(argc, argv, "Mycelium Contamination Detector");

  ns(Camera)* camera = nullptr;

  Glib::RefPtr<Gtk::Builder> ref_builder = Gtk::Builder::create();
  try {
    ref_builder->add_from_file("ui.glade");
  } catch (const Glib::FileError& ex) {
    std::cerr << "FileError: " << ex.what() << std::endl;
    return 1;
  } catch (const Glib::MarkupError& ex) {
    std::cerr << "MarkupError: " << ex.what() << std::endl;
    return 1;
  } catch (const Gtk::BuilderError& ex) {
    std::cerr << "BuilderError: " << ex.what() << std::endl;
    return 1;
  }

  ref_builder->get_widget_derived("MainWindow", camera);

  if (!camera) {
    return 1;
  }

  camera->initialize(2);
  std::thread camera_thread = std::thread([&] { camera->loop(); });
  int         result = app->run(*camera);
  camera_thread.join();

  delete camera;
  return result;
}
