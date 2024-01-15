//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include "Model.h"

class CubeModel : public Model
{
public:
	CubeModel(glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f));
	virtual ~CubeModel();

	virtual void Update(float dt);
	virtual void Draw();

	virtual bool ContainsPoint(glm::vec3 position);
	virtual bool IntersectsPlane(glm::vec3 planePoint, glm::vec3 planeNormal);
	virtual float IntersectsRay(glm::vec3 rayOrigin, glm::vec3 rayDirection); //Returns a strictly positive value if an intersection occurs

	virtual bool isSphere() { return false; } //This is not at all object-oriented, but somewhat necessary due to need for a simple double-dispatch mechanism

protected:
	virtual bool ParseLine(const std::vector<ci_string> &token);

private:
	// The vertex format could be different for different types of models
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 color;
	};

	unsigned int mVAO;
	unsigned int mVBO;

	std::vector<glm::vec3> vertexPositions;
};
