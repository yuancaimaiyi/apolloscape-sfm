// Copytirght Pavlo 2018
#include "cv_gl/gl_window.h"

#include <iostream>
#include <thread>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>


GLWindow::GLWindow(const std::string& name, const unsigned int width,
    const unsigned int height) 
      : width_(width), height_(height), delta_time_sum_(0.0),
      frames_(0) {

  // Load GLFW and Create a Window
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  window_ = glfwCreateWindow(width_, height_,
      name.c_str(), nullptr, nullptr);

  if (window_ != nullptr) {
      std::cout << "OpenGL Context Created" << std::endl;
  } else {
    return;
  }

  glfwMakeContextCurrent(window_);

  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwSetWindowUserPointer(window_, this);

  auto mouse_callback_stub = [](GLFWwindow *w, double xpos, double ypos) {
    static_cast<GLWindow*>(glfwGetWindowUserPointer(w))->MouseCallback(
        xpos, ypos);
  };

  glfwSetCursorPosCallback(window_, mouse_callback_stub);

  gladLoadGL();
  fprintf(stderr, "OpenGL %s, %s, %s\n", glGetString(GL_VERSION),
      glGetString(GL_VENDOR), glGetString(GL_RENDERER));


  /* ===== COMMON DRAWING PARAMS ============== */
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_PROGRAM_POINT_SIZE);
  glPointSize(1.0);

  // TODO: Do I need transparency?
  glEnable(GL_BLEND);

  // https://stackoverflow.com/questions/5984887/opengl-z-sorting-transparency
  // TODO: Render with sorting and transparency
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  // Check for Valid Context
  // if (window_ == nullptr) {
  //     fprintf(stderr, "Failed to Create OpenGL Context");
  //     glfwTerminate();
  //     return EXIT_FAILURE;
  // }

  last_time_ = glfwGetTime();

  std::cout << "GLWindow - (con)" << std::endl;
}

bool GLWindow::IsValid() {
  return (window_ != nullptr);
}

bool GLWindow::IsRunning() {
  if (glfwWindowShouldClose(window_)) {
    // finish loop
    return false;
  }

  // Delta time + FPS
  float time_value = glfwGetTime();
  delta_time = time_value - last_time_;

  if (delta_time < 0.015) {
    long sleep_for_micro = static_cast<long>((0.015 - delta_time) * 1e+6);
    std::this_thread::sleep_for(std::chrono::microseconds(sleep_for_micro));
    // std::cout << "---> SLEEP_FOR_MICRO = " << sleep_for_micro << std::endl;
    time_value = glfwGetTime();
    delta_time = time_value - last_time_;
    sleep_for_micro_sum_ += sleep_for_micro;
  }

  last_time_ = time_value;

  // FPS calc and output cam pos
  
  delta_time_sum_ += delta_time;
  int freq = 400;
  if (frames_ > 0 && frames_ % freq == 0) {
    float fps = freq / delta_time_sum_;
    std::cout << "\n\n======== FPS = " << fps << ","
              << ", sleep_for_micro = " << sleep_for_micro_sum_ / freq
              << std::endl;

    if (camera_) {
      std::cout << "========= Cam.position = {" 
                << glm::to_string(camera_->GetPosition())
                << "}, {"
                << camera_->GetRotationPitch()
                << ", " << camera_->GetRotationYaw()
                << "}"
                << std::endl;
      // std::cout << "========= Cam.pitch,yaw = " 
      // << camera_->GetRotationPitch()
      // << ", " << camera_->GetRotationYaw()
      // << std::endl;
    }

    delta_time_sum_ = 0;
    sleep_for_micro_sum_ = 0;
  }
  
  ++frames_;

  // Clear everything
  glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ProcessInput();

  //do stuff, we are running
  return true;
}

// int GLWindow::macMoved = 0;
void GLWindow::RunLoop() {
  glfwSwapBuffers(window_);
  glfwPollEvents();

#ifdef __APPLE__
  // std::cout << "Apple" << std::endl;

  // if (macMoved < 2) {
  //     int x, y;
  //     glfwGetWindowPos(this->window_, &x, &y);
  //     glfwSetWindowPos(this->window_, macMoved % 2 ? ++x : --x, y);
  //     macMoved++;
  // }
#endif    

}


void GLWindow::ProcessInput() {
  if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window_, true);
  } else if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
    camera_->ProcessKeyboard(FORWARD, delta_time);
  } else if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
    camera_->ProcessKeyboard(BACKWARD, delta_time);
  } else if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
    camera_->ProcessKeyboard(LEFT, delta_time);
  } else if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
    camera_->ProcessKeyboard(RIGHT, delta_time);
  } else if (glfwGetKey(window_, GLFW_KEY_0) == GLFW_PRESS) {
    camera_->ProcessKeyboard(MOVE_ORIGIN, delta_time);
  } else if (glfwGetKey(window_, GLFW_KEY_1) == GLFW_PRESS) {
    camera_->ProcessKeyboard(MOVE_TOP, delta_time);
  } else if (glfwGetKey(window_, GLFW_KEY_2) == GLFW_PRESS) {
    camera_->ProcessKeyboard(MOVE_SIDEWAYS_RIGHT, delta_time);
  } else if (glfwGetKey(window_, GLFW_KEY_3) == GLFW_PRESS) {
    camera_->ProcessKeyboard(MOVE_SIDEWAYS_LEFT, delta_time);
  }

  // Process all other functions that was attached
  this->ProcessInputFunctions();
  
}

void GLWindow::ProcessInputFunctions() {
  if (!this->process_input_funcs_.empty()) {
    for (const ProcessInputFunc& input_func: process_input_funcs_) {
      if (glfwGetKey(window_, input_func.key) == GLFW_PRESS) {
        input_func.func(delta_time);
      }
    }
  }
}


void GLWindow::MouseCallback(double xpos, double ypos) {
  if (first_mouse_) {
    last_x_ = xpos;
    last_y_ = ypos;
    first_mouse_ = false;
  }

  float xoffset = xpos - last_x_;
  float yoffset = last_y_ - ypos;

  last_x_ = xpos;
  last_y_ = ypos;

  camera_->ProcessMouseInput(xoffset, yoffset);
}

void GLWindow::SetCamera(const std::shared_ptr<Camera> camera) {
  camera_ = camera;
}

void GLWindow::Terminate() {
  std::cout << "GLWindow - Terminate!!!" << std::endl;
  glfwDestroyWindow(window_);
  glfwTerminate();
  // window_ = nullptr;
}

GLWindow::~GLWindow() {
  std::cout << "GLWindow - (dest)" << std::endl;
  // glfwTerminate();
  Terminate();
  window_ = nullptr;
}
