//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "../include/World.h"
#include "../include/Renderer.h"
#include "../include/ParsingHelper.h"

#include "../include/StaticCamera.h"
#include "../include/FirstPersonCamera.h"

#include "../include/CubeModel.h"
#include "../include/SphereModel.h"
#include <GLFW/glfw3.h>
#include "../include/EventManager.h"

#include <limits>

using namespace std;
using namespace glm;

World* World::instance;

float worldChangeTimer = 0.0f; //to avoid seizure-inducing flashing, enforces a "cooldown" on scene changes.


World::World()
{
    instance = this;

	// Setup Camera
	mCamera.push_back(new FirstPersonCamera(vec3(3.0f, 5.0f, 20.0f)));
	mCamera.push_back(new StaticCamera(vec3(3.0f, 30.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
	mCamera.push_back(new StaticCamera(vec3(0.5f,  0.5f, 5.0f), vec3(0.0f, 0.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
	mCurrentCamera = 0;
	gravity = 9.807;
	friction = 0.0f;
}

World::~World()
{
	// Models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		delete *it;
	}

	mModel.clear();

	// Camera
	for (vector<Camera*>::iterator it = mCamera.begin(); it < mCamera.end(); ++it)
	{
		delete *it;
	}
	mCamera.clear();
    
}

World* World::GetInstance()
{
    return instance;
}

void World::Update(float dt)
{
	// User Inputs
	// 0 1 2 to change the Camera
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_1 ) == GLFW_PRESS)
	{
		mCurrentCamera = 0;
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_2 ) == GLFW_PRESS)
	{
		if (mCamera.size() > 1)
		{
			mCurrentCamera = 1;
		}
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_3 ) == GLFW_PRESS)
	{
		if (mCamera.size() > 2)
		{
			mCurrentCamera = 2;
		}
	}

	// N M for earth and moon gravity
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_N) == GLFW_PRESS)
	{
		gravity = 9.807;
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_M) == GLFW_PRESS)
	{
		gravity = 1.625;
	}
	// 9 0 for ball scene and box scene
	if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_9) == GLFW_PRESS && worldChangeTimer <= 0.0f)
	{
		mModel.clear();
		gravity = 9.807f;
		friction = 0.0f;
		worldChangeTimer = 0.5f;
		LoadScene("assets/scenes/Balls.scene");
	}
	else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_0) == GLFW_PRESS)
	{
		gravity = 0.0f;
		friction = 1.0f;
		mModel.clear();
		worldChangeTimer = 0.5f;
		LoadScene("assets/scenes/Boxes.scene");
	}
	worldChangeTimer -= dt;

	//Ray Casting - in Box scene
	if (glfwGetMouseButton(EventManager::GetWindow(), GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		glm::mat4 inverseView = glm::inverse(GetCurrentCamera()->GetViewMatrix());
		glm::vec3 rayOrigin = inverseView[3]; //the camera position
		glm::vec3 rayDirection = -1.0f * glm::normalize(inverseView[2]); //the camera lookat

		//Try intersecting ray with every model
		//We are shooting a ray from the camera, directly forward
		float closestIntersection = std::numeric_limits<float>::infinity();
		Model* intersectingModel = nullptr;
		for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
		{
			Model* currentTarget = *it;
			float intersectionPoint = currentTarget->IntersectsRay(rayOrigin, rayDirection);
			if (intersectionPoint >= 0.0f && intersectionPoint < closestIntersection) {
				closestIntersection = currentTarget->IntersectsRay(rayOrigin, rayDirection);
				intersectingModel = currentTarget;
			}

		}

		if (intersectingModel != nullptr)
		{
			glm::vec3 intersectingPoint = rayOrigin + closestIntersection * rayDirection;
			glm::vec3 force = 5.0f * glm::normalize(intersectingModel->GetPosition() - intersectingPoint);
			intersectingModel->Accelerate(force, dt);

			glm::vec3 torque = 2.0f * (glm::cross(glm::normalize(force), glm::normalize(intersectingPoint)));
			intersectingModel->Angulate(torque, dt);
		}
	}


	//Movement

	// Update current Camera
	mCamera[mCurrentCamera]->Update(dt);


	//Physics

	// Apply Gravity
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		glm::vec3 gravityVector(0.0f, -gravity, 0.0f);
		(*it)->Accelerate(gravityVector, dt);
	}

	//Check collisions
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		glm::vec3 groundPoint = glm::vec3(0.0f);
		glm::vec3 groundUp = glm::vec3(0.0f, 1.0f, 0.0f);

		//Collisions with ground
	    //complexity: O(n)
		if ((*it)->IntersectsPlane(groundPoint, groundUp))
		{
			(*it)->BounceOffGround(); //Reverses y velocity
		}

		//Intersphere collisions
		//complexity: O(n^2)
		for (vector<Model*>::iterator it2 = it; it2 < mModel.end(); ++it2)
		{
			if (it != it2 && (*it)->isSphere() && (*it2)->isSphere()) //Spheres can't collide with themselves, and both models should be spheres for this scene.
			{
				Model* s1 = *it;
				Model* s2 = *it2;

				float distance = glm::distance(s1->GetPosition(), s2->GetPosition());
				float r1 = s1->GetScaling().x;
				float r2 = s2->GetScaling().x;
				float totalRadii = r1 + r2;

				//TODO 2 - Collisions between spheres
				/*
				if (distance < totalRadii) //Collision
				{
					glm::vec3 collisionNormal = glm::normalize( s1->GetPosition() - s2->GetPosition() );
					glm::vec3 collisionPoint = s2->GetPosition() + r2 * collisionNormal;

					//decompose momentum
					//
					float m1 = s1->GetMass();
					float m2 = s2->GetMass();

					glm::vec3 normalVelocity1 = glm::dot(s1->GetVelocity(), collisionNormal) * collisionNormal;
					glm::vec3 normalVelocity2 = glm::dot(s2->GetVelocity(), collisionNormal) * collisionNormal;

					glm::vec3 tangentMomentum1 = s1->GetVelocity() - normalVelocity1;
					glm::vec3 tangentMomentum2 = s2->GetVelocity() - normalVelocity2;

					glm::vec3 newNormalVelocity1 = ((m1 - m2) / (m1 + m2)) * normalVelocity1 + ((2 * m2) / (m1 + m2) * normalVelocity2);
					glm::vec3 newNormalVelocity2 = ((2 * m1) / (m1 + m2)) * normalVelocity1 + ((m2 - m1) / (m1 + m2) * normalVelocity2);

					s1->SetVelocity(newNormalVelocity1 + tangentMomentum1);
					s2->SetVelocity(newNormalVelocity2 + tangentMomentum2);
				}
				*/

			}
		}

	}

	// Update models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		(*it)->Update(dt);
	}
 
}

void World::Draw()
{
	Renderer::BeginFrame();
	
	// Set shader to use
	glUseProgram(Renderer::GetShaderProgramID());

	// This looks for the MVP Uniform variable in the Vertex Program
	GLuint VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");

	// Send the view projection constants to the shader
	mat4 VP = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
	glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

	// Draw models
	for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
	{
		(*it)->Draw();
	}

	// Draw Path Lines
	
	// Set Shader for path lines
	unsigned int prevShader = Renderer::GetCurrentShader();
	Renderer::SetShader(SHADER_PATH_LINES);
	glUseProgram(Renderer::GetShaderProgramID());

	// Send the view projection constants to the shader
	VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
	glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

    Renderer::CheckForErrors();

	// Restore previous shader
	Renderer::SetShader((ShaderType) prevShader);

	Renderer::EndFrame();
}

void World::LoadScene(const char * scene_path)
{
	// Using case-insensitive strings and streams for easier parsing
	ci_ifstream input;
	input.open(scene_path, ios::in);

	// Invalid file
	if(input.fail() )
	{	 
		fprintf(stderr, "Error loading file: %s\n", scene_path);
		getchar();
		exit(-1);
	}

	ci_string item;
	while( std::getline( input, item, '[' ) )   
	{
        ci_istringstream iss( item );

		ci_string result;
		if( std::getline( iss, result, ']') )
		{
			if( result == "cube" )
			{
				// Box attributes
				CubeModel* cube = new CubeModel();
				cube->Load(iss);
				mModel.push_back(cube);
			}
            else if( result == "sphere" )
            {
                SphereModel* sphere = new SphereModel();
                sphere->Load(iss);
                mModel.push_back(sphere);
            }
			else if ( result.empty() == false && result[0] == '#')
			{
				// this is a comment line
			}
			else
			{
				fprintf(stderr, "Error loading scene file... !");
				getchar();
				exit(-1);
			}
	    }
	}
	input.close();
}

const Camera* World::GetCurrentCamera() const
{
     return mCamera[mCurrentCamera];
}
