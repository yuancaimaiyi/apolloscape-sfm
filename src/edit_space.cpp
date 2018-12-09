// Copyright Pavlo 2018

#include <iostream>
#include <memory>
#include <iomanip>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "cv_gl/camera.h"
#include "cv_gl/mesh.h"
#include "cv_gl/shader.h"
#include "cv_gl/utils.hpp"

#include "cv_gl/gl_window.h"

#include "cv_gl/renderer.h"
#include "cv_gl/dobject.h"
#include "cv_gl/object_factory.hpp"


const int kWindowWidth = 1226/2;
const int kWindowHeight = 1028/2;


int main(int argc, char* argv[]) {

  std::shared_ptr<Camera> camera =
      std::make_shared<Camera>(glm::vec3(-3.0f, 0.0f, 1.5f));

  std::cout << "Hello edit space" << std::endl;

  float width_ratio = camera->GetImageWidth()/camera->GetImageHeight();
  CameraIntrinsics camera_intr = camera->GetCameraIntrinsics();

  // float fx = camera->GetFx() / camera->GetImageWidth();
  // float fy = camera->GetFy() / camera->GetImageHeight();
  std::cout << "fx = " << camera_intr.fx << std::endl;
  std::cout << "fy = " << camera_intr.fy << std::endl;
  float f = std::max(camera_intr.fx, camera_intr.fy);

  glm::vec4 v(1.0f);
  std::cout << "v = " << v << std::endl;

  GLWindow gl_window("OpenGL: Edit Space", kWindowWidth, kWindowHeight);
  gl_window.SetCamera(camera);

  // Renderer
  std::unique_ptr<Renderer> renderer(new Renderer(camera));

  std::shared_ptr<DObject> root = std::make_shared<DObject>();

  std::shared_ptr<ColorObject> floor_obj(ObjectFactory::CreateFloor(1.0, 50));

  std::shared_ptr<CameraObject> camera_obj(new CameraObject(width_ratio, camera_intr));
  camera_obj->SetTranslation(glm::vec3(3.0f, 0.0f, 1.0f));
  // camera_obj->SetRotation(-1.7889f, 0.0250f, 0.0f);
  // camera_obj->SetRotation(-1.7889f, 0.0250f, -1.4811f);
  camera_obj->SetRotation(static_cast<float>(M_PI_2), 0.0f, static_cast<float>(M_PI_2));
  // root->AddChild(camera_obj);

  // Main Camera pos/rot
  // camera->SetPosition(glm::vec3(3.0f, 0.0f, 1.0f));
  camera->SetOrigin(glm::vec3(3.0f, 0.0f, 1.0f));
  // camera->SetRotation(-1.7889f, 0.0250f, -1.4811f);
  // camera->SetRotation(0.0f, 0.0f, 0.0f);
  // camera->SetDirection(glm::vec3(0.0f, 0.0f, 1.0f));

  std::shared_ptr<DObject> axes_obj(ObjectFactory::CreateAxes(1.0f));
  // camera_obj->AddChild(axes_obj);
  // root->AddChild(axes_obj);


  std::vector<glm::vec3> points = {glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(2.0f), glm::vec3(3.0f), glm::vec3(4.0f)};
  for (int i = 0; i < points.size(); ++i) {
    points[i] += glm::vec3(10.0f, 0.0f, 1.0f);
  }

  // std::vector<glm::vec3> points = {
  //   glm::vec3(0.0f, 0.0f, 0.0f),
  //   glm::vec3( - camera_intr.cx * width_ratio, - camera_intr.cy, 0.0f)
  // };
  // for (int i = 0; i < points.size(); ++i) {
  //   points[i] += glm::vec3(0.0f, 0.0f, f);
  // }
  
  std::shared_ptr<DObject> points_obj(ObjectFactory::CreatePoints(points));
  // points_obj->SetTranslation(glm::vec3(10.0f, 0.0f, 0.0f));
  // points_obj->AddChild(axes_obj);
  root->AddChild(points_obj);
  root->AddChild(axes_obj);

  camera_obj->AddProjectedPoints(points);




  std::shared_ptr<ModelObject> debug_cube_obj(
      ObjectFactory::CreateModelObject(
          "../data/objects/debug_cube/debug_cube.obj"));
//   debug_cube_obj->SetScale(glm::vec3(0.2f));
  debug_cube_obj->SetTranslation(glm::vec3(3.0f, 3.0f, 3.0f));
  root->AddChild(debug_cube_obj);



/*
  std::shared_ptr<ModelObject> nanosuit_obj(
      ObjectFactory::CreateModelObject(
          "../data/objects/nanosuit/nanosuit.obj"));
  nanosuit_obj->SetScale(glm::vec3(0.2f));
  nanosuit_obj->SetTranslation(glm::vec3(5.0f, 2.0f, 0.0f));

  std::shared_ptr<ModelObject> cyborg_obj(
      ObjectFactory::CreateModelObject(
          "../data/objects/cyborg/cyborg.obj"));
  cyborg_obj->SetScale(glm::vec3(0.8f));
  cyborg_obj->SetTranslation(glm::vec3(6.0f, 1.0f, 0.0f));

  std::shared_ptr<ModelObject> planet_obj(
      ObjectFactory::CreateModelObject(
          "../data/objects/planet/planet.obj"));
  // planet_obj->SetScale(glm::vec3(0.8f));
  planet_obj->SetTranslation(glm::vec3(10.0f, -5.0f, 6.0f));

  std::shared_ptr<ModelObject> rock_obj(
      ObjectFactory::CreateModelObject(
          "../data/objects/rock/rock.obj"));
  rock_obj->SetScale(glm::vec3(0.3f));
  rock_obj->SetTranslation(glm::vec3(5.0f, -3.0f, 0.0f));
  */

//   std::cout << "Floor = " << floor_obj << std::endl;
//   std::cout << "Camera = " << camera_obj << std::endl;
//   std::cout << "Debug Cube = " << debug_cube_obj << std::endl;

  /*
  std::cout << "Nanosuit = " << nanosuit_obj << std::endl;
  std::cout << "Cyborg = " << cyborg_obj << std::endl;
  std::cout << "Planet = " << planet_obj << std::endl;
  std::cout << "Rock = " << rock_obj << std::endl;
  */

  while(gl_window.IsRunning()) {
    // std::cout << "delta_time = " << gl_window.delta_time << std::endl;

    /* ====================== Render ===================== */
    renderer->Draw(floor_obj);
    
    renderer->Draw(debug_cube_obj);

    renderer->Draw(root);

    renderer->Draw(camera_obj);

    // camera->Print();

    // renderer->Draw(axes);

    /*
    renderer->Draw(nanosuit_obj);
    renderer->Draw(cyborg_obj);
    renderer->Draw(planet_obj);
    renderer->Draw(rock_obj);
    */

    gl_window.RunLoop();
  }

  return EXIT_SUCCESS;

}
