//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "World.h"
#include "Renderer.h"

#include "FirstPersonCamera.h"
#include "StaticCamera.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "EventManager.h"

#include <limits>
#include "Quad.h"
using namespace std;
using namespace glm;

World::World() : spawnTimer(0) {
  // Setup Camera
  camera.push_back(new FirstPersonCamera(vec3(5.0f, 5.0f, 15.0f)));
  camera.push_back(new StaticCamera(
      vec3(3.0f, 30.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
  camera.push_back(new StaticCamera(
      vec3(0.5f, 0.5f, 5.0f), vec3(0.0f, 0.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
  currentCamera = 0;
  gravity = 9.807f;
  friction = 0.0f;
}

World::~World() {
  // Models
  for (list<Quad*>::iterator it = models.begin(); it != models.end(); ++it) {
    delete *it;
  }

  models.clear();

  // Camera
  for (vector<Camera*>::iterator it = camera.begin(); it < camera.end(); ++it) {
    delete *it;
  }
  camera.clear();
}

void World::Update(float dt) {
  // User Inputs
  // 0 1 2 to change the Camera
  if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_1) == GLFW_PRESS) {
    currentCamera = 0;
  } else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_2) == GLFW_PRESS) {
    if (camera.size() > 1) {
      currentCamera = 1;
    }
  } else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_3) == GLFW_PRESS) {
    if (camera.size() > 2) {
      currentCamera = 2;
    }
  }

  // Movement

  // Update current Camera
  camera[currentCamera]->Update(dt);

  spawnTimer += dt;
  if (spawnTimer > SPAWN_PERIOD) {
    int vertexCount;
    int vao = Quad::makeQuadVAO(vertexCount);
    Quad* model = new Quad(vao, vertexCount, 0);

    // TODO 1 - Give the particles some motion when created
    // float angle = EventManager::GetRandomFloat(0, 360);
    // vec4 randomSidewaysVelocity(0.5, 2, 0, 0);
    // randomSidewaysVelocity = glm::rotate(mat4(1), glm::radians(angle),
    // vec3(0,1,0)) * randomSidewaysVelocity;
    // model->SetVelocity(vec3(randomSidewaysVelocity));

    models.push_back(model);  // spawn
    spawnTimer -= SPAWN_PERIOD;
  }

  // TODO 4 - calculate billboard rotation parameters
  // vec3 originalAxis(0, 0, 1);
  // vec3 cameraLookAt(-GetCurrentCamera()->GetLookAt());
  // cameraLookAt.y = 0; //project onto xz plane
  // cameraLookAt.x *= -1;
  // cameraLookAt = normalize(cameraLookAt);

  // vec3 billboardRotationAxis(vec3(0,1,0));

  // float billboardRotation = glm::acos(-1 * dot(originalAxis, cameraLookAt)) *
  // 360 / (2 * pi<float>());
  //
  // if (cameraLookAt.x > 0) {
  //	billboardRotation = 360 - billboardRotation;
  //}

  // Update models
  list<Quad*>::iterator it = models.begin();
  while (it != models.end()) {
    // TODO 4 - Set the rotation
    //(*it)->SetRotation(billboardRotationAxis, billboardRotation);
    (*it)->Update(dt);

    if ((*it)->expired()) {
      it = models.erase(it);
    } else {
      ++it;
    }
  }
}

void World::Draw() {
  Renderer::BeginFrame();

  // Set shader to use
  glUseProgram(Renderer::GetShaderProgramID());

  // This looks for the MVP Uniform variable in the Vertex Program
  GLuint VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(),
                                                 "ViewProjectionTransform");

  // Send the view projection constants to the shader
  mat4 VP = camera[currentCamera]->GetViewProjectionMatrix();
  glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

  // Draw models

  // TODO 3.4 Temporarily disable depth test for transparency
  // Disable depth test
  // glDisable(GL_DEPTH_TEST);
  for (list<Quad*>::iterator it = models.begin(); it != models.end(); ++it) {
    (*it)->Draw();
  }
  // TODO 3.4 - Make sure to re-enable it when done!!!
  // Re-enable depth test
  // glEnable(GL_DEPTH_TEST);

  Renderer::EndFrame();
}

const Camera* World::GetCurrentCamera() const { return camera[currentCamera]; }
