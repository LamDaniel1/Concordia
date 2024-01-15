//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "../include/Model.h"
#include "../include/World.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

using namespace std;
using namespace glm;

Model::Model()
	: mName("UNNAMED"), mPosition(0.0f, 0.0f, 0.0f), mScaling(1.0f, 1.0f, 1.0f), mRotationAxis(0.0f, 1.0f, 0.0f),
	mRotationAngleInDegrees(0.0f), mVelocity(0.0f), mAngularAxis(mRotationAxis), mAngularVelocityInDegrees(0.0f), mMass(1.0f)
{
}

Model::~Model()
{
}

void Model::Update(float dt)
{
	mPosition += dt * mVelocity;
	mRotationAxis = glm::mix(mRotationAxis, mAngularAxis, dt);
	mRotationAngleInDegrees += dt * mAngularVelocityInDegrees;
}

void Model::Draw()
{
}


void Model::Load(ci_istringstream& iss)
{
	ci_string line;

	// Parse model line by line
	while(std::getline(iss, line))
	{
		// Splitting line into tokens
		ci_istringstream strstr(line);
		istream_iterator<ci_string, char, ci_char_traits> it(strstr);
		istream_iterator<ci_string, char, ci_char_traits> end;
		vector<ci_string> token(it, end);

		if (ParseLine(token) == false)
		{
			fprintf(stderr, "Error loading scene file... token:  %s!", token[0].c_str());
			getchar();
			exit(-1);
		}
	}
}

bool Model::ParseLine(const std::vector<ci_string> &token)
{
    if (token.empty() == false)
	{
		if (token[0].empty() == false && token[0][0] == '#')
		{
			return true;
		}
		else if (token[0] == "name")
		{
			assert(token.size() > 2);
			assert(token[1] == "=");

			mName = token[2];	
		}
		else if (token[0] == "position")
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			mPosition.x = static_cast<float>(atof(token[2].c_str()));
			mPosition.y = static_cast<float>(atof(token[3].c_str()));
			mPosition.z = static_cast<float>(atof(token[4].c_str()));
		}
		else if (token[0] == "rotation")
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			mRotationAxis.x = static_cast<float>(atof(token[2].c_str()));
			mRotationAxis.y = static_cast<float>(atof(token[3].c_str()));
			mRotationAxis.z = static_cast<float>(atof(token[4].c_str()));
			mRotationAngleInDegrees = static_cast<float>(atof(token[5].c_str()));
			mAngularAxis = mRotationAxis;

			glm::normalize(mRotationAxis);
		}
		else if (token[0] == "scaling")
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			mScaling.x = static_cast<float>(atof(token[2].c_str()));
			mScaling.y = static_cast<float>(atof(token[3].c_str()));
			mScaling.z = static_cast<float>(atof(token[4].c_str()));
		}
		else if (token[0] == "velocity")
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			mVelocity.x = static_cast<float>(atof(token[2].c_str()));
			mVelocity.y = static_cast<float>(atof(token[3].c_str()));
			mVelocity.z = static_cast<float>(atof(token[4].c_str()));
		}
		else if (token[0] == "mass")
		{
			assert(token.size() > 2);
			assert(token[1] == "=");

			mMass = static_cast<float>(atof(token[2].c_str()));
		}
		else
		{
			return false;
		}
	}

	return true;
}

glm::mat4 Model::GetWorldMatrix() const
{
	// @TODO 2 - You must build the world matrix from the position, scaling and rotation informations
    //           If the model has an animation, get the world transform from the animation.
	mat4 worldMatrix(1.0f);

    // Solution TRS

        mat4 t = glm::translate(mat4(1.0f), mPosition);
        mat4 r = glm::rotate(mat4(1.0f), glm::radians(mRotationAngleInDegrees), mRotationAxis);
        mat4 s = glm::scale(mat4(1.0f), mScaling);
        worldMatrix = t * r * s;
    
	return worldMatrix;
}

void Model::Accelerate(glm::vec3 acceleration, float delta)
{
	if (mMass != 0.0f) { //No acceleration for massless objects
		mVelocity += acceleration * delta;
	}
}

void Model::Angulate(glm::vec3 torque, float delta)
{
	if (mMass != 0.0f) {//No angular acceleration for massless objects
		mAngularAxis = torque;
		mAngularVelocityInDegrees += glm::dot(mAngularAxis, mRotationAxis);
	}
}

void Model::BounceOffGround()
{
	mVelocity.y = glm::abs(mVelocity.y);
}


void Model::SetPosition(glm::vec3 position)
{
	mPosition = position;
}

void Model::SetScaling(glm::vec3 scaling)
{
	mScaling = scaling;
}

void Model::SetRotation(glm::vec3 axis, float angleDegrees)
{
	mRotationAxis = axis;
	mRotationAngleInDegrees = angleDegrees;
}

void Model::SetVelocity(glm::vec3 velocity)
{
	mVelocity = velocity;
}
