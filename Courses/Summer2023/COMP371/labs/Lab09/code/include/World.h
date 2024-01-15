//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include <vector>

class Camera;
class Model;

class World
{
public:
	World();
	~World();
	
    static World* GetInstance();

	void Update(float dt);
	void Draw();

	void LoadScene(const char * scene_path);

    const Camera* GetCurrentCamera() const;
    
private:
    static World* instance;
    
	std::vector<Model*> mModel;
	std::vector<Camera*> mCamera;
	unsigned int mCurrentCamera;

	float gravity = 0.0f;
	float friction = 0.0f;

};
