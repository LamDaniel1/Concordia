//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include "ParsingHelper.h"

#include <vector>

#include <glm/glm.hpp>

class Animation;

class Model
{
public:
	Model();
	virtual ~Model();

	virtual void Update(float dt) = 0;
	void Accelerate(glm::vec3 force, float dt);
	void Angulate(glm::vec3 torque, float dt);
	virtual void Draw() = 0;

	void Load(ci_istringstream& iss);

	virtual glm::mat4 GetWorldMatrix() const;

	void SetPosition(glm::vec3 position);
	void SetScaling(glm::vec3 scaling);
	void SetRotation(glm::vec3 axis, float angleDegrees);
	void SetVelocity(glm::vec3 velocity);
	void SetAngularVelocity(glm::vec3 axis, float angleDegrees);

	glm::vec3 GetPosition() const		{ return mPosition; }
	glm::vec3 GetScaling() const		{ return mScaling; }
	glm::vec3 GetRotationAxis() const	{ return mRotationAxis; }
	float     GetRotationAngle() const	{ return mRotationAngleInDegrees; }
    ci_string GetName()                 { return mName; }

	glm::vec3 GetVelocity() const		{ return mVelocity; }
	float	  GetMass() const			{ return mMass; }

	virtual bool ContainsPoint(glm::vec3 position) = 0;//Whether or not the given point is withing the model. For collisions.
	virtual bool IntersectsPlane(glm::vec3 planePoint, glm::vec3 planeNormal) = 0;
	virtual float IntersectsRay(glm::vec3 rayOrigin, glm::vec3 rayDirection) = 0; //Returns a strictly positive value if an intersection occurs
	void BounceOffGround();

	virtual bool isSphere() = 0; //This is not at all object-oriented, but somewhat necessary due to need for a simple double-dispatch mechanism

protected:
	virtual bool ParseLine(const std::vector<ci_string> &token) = 0;

	ci_string mName; // The model name is mainly for debugging
	glm::vec3 mPosition;
	glm::vec3 mScaling;
	glm::vec3 mRotationAxis;
	float     mRotationAngleInDegrees;

	glm::vec3 mVelocity;
	glm::vec3 mAngularAxis;
	float     mAngularVelocityInDegrees;

	float mMass;
};
