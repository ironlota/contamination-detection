#ifndef CAMERA_H
#define CAMERA_H

#include <mutex>

#include <gtkmm.h>
#include <opencv4/opencv2/opencv.hpp>

#include "predefined.h"

NAMESPACE_BEGIN

enum Color { RED, GREEN, BLUE };

class Camera : public Gtk::Window {
 private:
  Glib::RefPtr<Gtk::Builder> builder;
  Gtk::Image*                rgb_image;
  Gtk::Image*                hsv_image;
  Gtk::Button*               close_button;
  Gtk::ToggleButton*         toggle_pause_button;
  Gtk::Button*               capture_button;
  volatile bool              paused = false;
  volatile bool              stopped = true;
  cv::VideoCapture*          device;
  cv::Mat*                   rgb_frame;
  cv::Mat*                   hsv_frame;
  Glib::Dispatcher*          hsv_dispatcher;
  Glib::Dispatcher*          rgb_dispatcher;
  std::mutex*                mutex;
  std::mutex*                rgb_mutex;
  void                       update_hsv_image();
  void                       update_rgb_image();

 public:
  Camera(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
  virtual ~Camera();
  void initialize(int camera_idx);
  void capture();
  void loop();
  void stop();
  void toggle_pause();
  void resume();
};

NAMESPACE_END

#endif
