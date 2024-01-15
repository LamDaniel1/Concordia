//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include <list>
#include <vector>

class Camera;
class Model;
class Quad;

class World {
 public:
  World();
  ~World();

  void Update(float dt);
  void Draw();

  const Camera* GetCurrentCamera() const;

 private:
  std::list<Quad*> models;
  std::vector<Camera*> camera;
  unsigned int currentCamera;

  float gravity;
  float friction;

  float spawnTimer;

  const float SPAWN_PERIOD = 0.25;
};
