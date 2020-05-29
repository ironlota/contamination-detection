#include "camera.h"

NAMESPACE_BEGIN

Camera::Camera(BaseObjectType*                   cobject,
               const Glib::RefPtr<Gtk::Builder>& refGlade)
    : Gtk::Window(cobject), builder(refGlade) {
  builder->get_widget("rgb_canvas", rgb_image);
  builder->get_widget("hsv_canvas", hsv_image);
  builder->get_widget("close_button", close_button);
  builder->get_widget("toggle_pause_button", toggle_pause_button);
  builder->get_widget("capture_button", capture_button);
  close_button->signal_clicked().connect(
      sigc::mem_fun(*this, &ns(Camera::stop)));
  toggle_pause_button->signal_clicked().connect(
      sigc::mem_fun(*this, &ns(Camera::toggle_pause)));
  capture_button->signal_clicked().connect(
      sigc::mem_fun(*this, &ns(Camera::capture)));

  device = new cv::VideoCapture();
  mutex = new std::mutex();
  rgb_mutex = new std::mutex();
  rgb_frame = new cv::Mat();
  hsv_frame = new cv::Mat();
  hsv_dispatcher = new Glib::Dispatcher();
  rgb_dispatcher = new Glib::Dispatcher();

  rgb_dispatcher->connect([&]() {
    mutex->lock();
    update_rgb_image();
    mutex->unlock();
  });

  hsv_dispatcher->connect([&]() {
    rgb_mutex->lock();
    update_hsv_image();
    rgb_mutex->unlock();
  });
}

Camera::~Camera() {
  if (device->isOpened())
    device->release();
  delete device;
  delete mutex;
  delete hsv_frame;
  delete rgb_frame;
  delete hsv_dispatcher;
  delete rgb_mutex;
  delete rgb_dispatcher;
}

void Camera::initialize(int camera_idx) {
  bool    result = device->open(camera_idx);
  cv::Mat frame_bgr;

  if (result) {
    device->set(cv::CAP_PROP_FRAME_WIDTH, 640);
    device->set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    device->set(cv::CAP_PROP_FPS, 30);

    for (int i = 0; i < 3; i++) {
      device->grab();
    }

    for (int i = 0; result && i < 3; i++) {
      result = result && device->read(frame_bgr);
    }
  }
}

void Camera::loop() {
  stopped = false;
  int     sensitivity = 20;
  int     ratio = 3;
  int     low_threshold = 0;
  int     max_low_threshold = 100;
  int     kernel_size = 3;
  cv::Mat ori_img, hsv_img, masked_img, blurred_img, edges, gray_img, thres_img,
      thres_drawing, edge_lines;
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i>              hierarchy;
  std::vector<cv::Vec2f>              lines;
  std::vector<cv::Vec4i>              lines_p;
  cv::RNG                             rng(12345);

  while (!stopped) {
    if (!paused) {
      *device >> ori_img;
      mutex->lock();

      // cv::cvtColor(ori_img, hsv_img, cv::COLOR_BGR2HSV);
      // cv::inRange(hsv_img, cv::Scalar(0, 80, 50), cv::Scalar(50, 255, 255),
      //             masked_img);
      // cv::blur(masked_img, blurred_img, cv::Size(9, 9));
      // cv::Canny(blurred_img, edges, low_threshold, 50, kernel_size);
      // cv::findContours(edges, contours, hierarchy, cv::RETR_TREE,
      //                  cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

      // cv::Mat drawing = cv::Mat::zeros(edges.size(), CV_8UC3);
      // for (int i = 0; i < contours.size(); ++i) {
      //   cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0,
      //   255),
      //                                 rng.uniform(0, 255));
      //   cv::drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0,
      //                    cv::Point());
      // }

      cv::cvtColor(ori_img, *rgb_frame, cv::COLOR_BGR2RGB);

      // Grayscale
      cv::cvtColor(ori_img, gray_img, cv::COLOR_BGR2GRAY);
      // cv::GaussianBlur(gray_img, blurred_img, cv::Size(1, 1), 0, 0);
      cv::threshold(gray_img, thres_drawing, 0, 255,
                    cv::THRESH_BINARY + cv::THRESH_OTSU);

      cv::cvtColor(thres_drawing, *hsv_frame, cv::COLOR_GRAY2BGR);

      // Blob detector
      // cv::Ptr<cv::BRISK>        ptrBrisk = cv::BRISK::create(90, 0);
      // std::vector<cv::KeyPoint> keypoints;
      // ptrBrisk->detect(gray_img, keypoints);

      // cv::drawKeypoints(*hsv_frame, keypoints, *hsv_frame,
      //                   cv::Scalar(0, 0, 255),
      //                   cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

      // size_t count = 0;
      // for (size_t i = 0; i < keypoints.size(); ++i) {
      //   // if (count >= 5)
      //   //   break;
      //   cv::KeyPoint keypoint = keypoints[i];
      //   char         buf[50];
      //   sprintf(buf, "X: %.2f Y: %.2f", keypoint.pt.x, keypoint.pt.y);
      //   cv::putText(*hsv_frame, buf, cv::Point(keypoint.pt.x, keypoint.pt.y),
      //               cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(0, 255,
      //               0), 1, cv::LINE_AA);
      //   std::cout << "X : " << keypoint.pt.x << " Y : " << keypoint.pt.y
      //             << std::endl;
      //   // i += 3;
      //   count++;
      // }

      // cv::cvtColor(im_with_keypoints, *hsv_frame, cv::COLOR_GRAY2BGR);

      // Edge detection
      // cv::Canny(gray_img, edges, 20, 150, 3);
      // cv::HoughLines(edges, lines, 1, CV_PI / 180, 100, 0, 0);

      // cv::adaptiveThreshold(gray_img, thres_drawing, 255,
      //                       cv::ADAPTIVE_THRESH_GAUSSIAN_C,
      //                       cv::THRESH_BINARY, 11, 2);

      // for (size_t i = 0; i < lines.size(); i++) {
      //   float     rho = lines[i][0], theta = lines[i][1];
      //   cv::Point pt1, pt2;
      //   double    a = cos(theta), b = sin(theta);
      //   double    x0 = a * rho, y0 = b * rho;
      //   pt1.x = cvRound(x0 + 1000 * (-b));
      //   pt1.y = cvRound(y0 + 1000 * (a));
      //   pt2.x = cvRound(x0 - 1000 * (-b));
      //   pt2.y = cvRound(y0 - 1000 * (a));
      //   cv::line(*hsv_frame, pt1, pt2, cv::Scalar(0, 0, 255), 3,
      //   cv::LINE_AA);
      // }

      mutex->unlock();
      rgb_dispatcher->emit();
      hsv_dispatcher->emit();
    }
  }
}

void Camera::toggle_pause() {
  paused = !paused;
  if (paused) {
    toggle_pause_button->set_label("resume");
  } else {
    toggle_pause_button->set_label("pause");
  }
}

void Camera::capture() {
  cv::imwrite("capture_ori.jpg", *rgb_frame);
  cv::imwrite("capture_otsu.jpg", *hsv_frame);
}

void Camera::stop() {
  stopped = true;
  Window::close();
}

void Camera::update_hsv_image() {
  if (!hsv_frame->empty()) {
    hsv_image->set(Gdk::Pixbuf::create_from_data(
        hsv_frame->data, Gdk::COLORSPACE_RGB, false, 8, hsv_frame->cols,
        hsv_frame->rows, hsv_frame->step));
    hsv_image->queue_draw();
  }
}

void Camera::update_rgb_image() {
  if (!rgb_frame->empty()) {
    rgb_image->set(Gdk::Pixbuf::create_from_data(
        rgb_frame->data, Gdk::COLORSPACE_RGB, false, 8, rgb_frame->cols,
        rgb_frame->rows, rgb_frame->step));
    rgb_image->queue_draw();
  }
}

NAMESPACE_END
