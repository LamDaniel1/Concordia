//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "../include/CubeModel.h"
#include "../include/Renderer.h"
#include <glm/gtx/transform.hpp>
#include <limits>

using namespace glm;

CubeModel::CubeModel(vec3 size) : Model()
{
	// Create Vertex Buffer for all the vertices of the Cube
	vec3 halfSize = size * 0.5f;
	
	Vertex vertexBuffer[] = {  // position,                normal,                  color
								{ vec3(-halfSize.x,-halfSize.y,-halfSize.z), vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) }, //left - red
								{ vec3(-halfSize.x,-halfSize.y, halfSize.z), vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) },
								{ vec3(-halfSize.x, halfSize.y, halfSize.z), vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) },

								{ vec3(-halfSize.x,-halfSize.y,-halfSize.z), vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) },
								{ vec3(-halfSize.x, halfSize.y, halfSize.z), vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) },
								{ vec3(-halfSize.x, halfSize.y,-halfSize.z), vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) },

								{ vec3( halfSize.x, halfSize.y,-halfSize.z), vec3( 0.0f, 0.0f,-1.0f), vec3(0.0f, 0.0f, 1.0f) }, // far - blue
								{ vec3(-halfSize.x,-halfSize.y,-halfSize.z), vec3( 0.0f, 0.0f,-1.0f), vec3(0.0f, 0.0f, 1.0f) },
								{ vec3(-halfSize.x, halfSize.y,-halfSize.z), vec3( 0.0f, 0.0f,-1.0f), vec3(0.0f, 0.0f, 1.0f) },

								{ vec3( halfSize.x, halfSize.y,-halfSize.z), vec3( 0.0f, 0.0f,-1.0f), vec3(0.0f, 0.0f, 1.0f) },
								{ vec3( halfSize.x,-halfSize.y,-halfSize.z), vec3( 0.0f, 0.0f,-1.0f), vec3(0.0f, 0.0f, 1.0f) },
								{ vec3(-halfSize.x,-halfSize.y,-halfSize.z), vec3( 0.0f, 0.0f,-1.0f), vec3(0.0f, 0.0f, 1.0f) },

								{ vec3( halfSize.x,-halfSize.y, halfSize.z), vec3( 0.0f,-1.0f, 0.0f), vec3(0.0f, 1.0f, 1.0f) }, // bottom - turquoise
								{ vec3(-halfSize.x,-halfSize.y,-halfSize.z), vec3( 0.0f,-1.0f, 0.0f), vec3(0.0f, 1.0f, 1.0f) },
								{ vec3( halfSize.x,-halfSize.y,-halfSize.z), vec3( 0.0f,-1.0f, 0.0f), vec3(0.0f, 1.0f, 1.0f) },
																
								{ vec3( halfSize.x,-halfSize.y, halfSize.z), vec3( 0.0f,-1.0f, 0.0f), vec3(0.0f, 1.0f, 1.0f) },
								{ vec3(-halfSize.x,-halfSize.y, halfSize.z), vec3( 0.0f,-1.0f, 0.0f), vec3(0.0f, 1.0f, 1.0f) },
								{ vec3(-halfSize.x,-halfSize.y,-halfSize.z), vec3( 0.0f,-1.0f, 0.0f), vec3(0.0f, 1.0f, 1.0f) },

								{ vec3(-halfSize.x, halfSize.y, halfSize.z), vec3( 0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f) }, // near - green
								{ vec3(-halfSize.x,-halfSize.y, halfSize.z), vec3( 0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f) },
								{ vec3( halfSize.x,-halfSize.y, halfSize.z), vec3( 0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f) },

								{ vec3( halfSize.x, halfSize.y, halfSize.z), vec3( 0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f) },
								{ vec3(-halfSize.x, halfSize.y, halfSize.z), vec3( 0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f) },
								{ vec3( halfSize.x,-halfSize.y, halfSize.z), vec3( 0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f) },

								{ vec3( halfSize.x, halfSize.y, halfSize.z), vec3( 1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 1.0f) }, // right - purple
								{ vec3( halfSize.x,-halfSize.y,-halfSize.z), vec3( 1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 1.0f) },
								{ vec3( halfSize.x, halfSize.y,-halfSize.z), vec3( 1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 1.0f) },

								{ vec3( halfSize.x,-halfSize.y,-halfSize.z), vec3( 1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 1.0f) },
								{ vec3( halfSize.x, halfSize.y, halfSize.z), vec3( 1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 1.0f) },
								{ vec3( halfSize.x,-halfSize.y, halfSize.z), vec3( 1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 1.0f) },

								{ vec3( halfSize.x, halfSize.y, halfSize.z), vec3( 0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f) }, // top - yellow
								{ vec3( halfSize.x, halfSize.y,-halfSize.z), vec3( 0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f) },
								{ vec3(-halfSize.x, halfSize.y,-halfSize.z), vec3( 0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f) },

								{ vec3( halfSize.x, halfSize.y, halfSize.z), vec3( 0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f) },
								{ vec3(-halfSize.x, halfSize.y,-halfSize.z), vec3( 0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f) },
								{ vec3(-halfSize.x, halfSize.y, halfSize.z), vec3( 0.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f) }
						};

	//Copy the vertex positions, for ray casting later on.
	for (int i = 0; i < sizeof(vertexBuffer) / sizeof(vertexBuffer[0]); ++i)
	{
		vertexPositions.push_back(vertexBuffer[i].position);
	}

	// Create a vertex array
	glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

	// Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBuffer), vertexBuffer, GL_STATIC_DRAW);
    
    
    
    // 1st attribute buffer : vertex Positions
    glVertexAttribPointer(    0,                // attribute. No particular reason for 0, but must match the layout in the shader.
                          3,                // size
                          GL_FLOAT,        // type
                          GL_FALSE,        // normalized?
                          sizeof(Vertex), // stride
                          (void*)0        // array buffer offset
                          );
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : vertex normal
    glVertexAttribPointer(    1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)sizeof(vec3)    // Normal is Offseted by vec3 (see class Vertex)
                          );
    glEnableVertexAttribArray(1);

    
    // 3rd attribute buffer : vertex color
    glVertexAttribPointer(    2,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*) (2* sizeof(vec3)) // Color is Offseted by 2 vec3 (see class Vertex)
                          );
    glEnableVertexAttribArray(2);

}

CubeModel::~CubeModel()
{
	// Free the GPU from the Vertex Buffer
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
}

void CubeModel::Update(float dt)
{
	// If you are curious, un-comment this line to have spinning cubes!
	// That will only work if your world transform is correct...
	// mRotationAngleInDegrees += 90 * dt; // spins by 90 degrees per second

	Model::Update(dt);
}

void CubeModel::Draw()
{
	// Draw the Vertex Buffer
	// Note this draws a unit Cube
	// The Model View Projection transforms are computed in the Vertex Shader
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform"); 
	glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &GetWorldMatrix()[0][0]);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices: 3 * 2 * 6 (3 per triangle, 2 triangles per face, 6 faces)
}

//Using an oriented bounding box (OBB)
bool CubeModel::ContainsPoint(glm::vec3 position)
{
	glm::vec4 worldSpacePoint(position, 1.0f);

	//We will first transform the position into Cube Model Space
	glm::mat4 mTranslate = glm::translate(GetPosition());
	glm::mat4 mRotation = glm::rotate(GetRotationAngle(), GetRotationAxis());
	glm::mat4 mScale = glm::scale(GetScaling());

	glm::vec3 cubeModelSpacePoint = glm::inverse(mTranslate * mRotation * mScale) * worldSpacePoint;

	//Then we simply check if the transformed point lies inside a unit cube at the origin
	return
		cubeModelSpacePoint.x >= -0.5f && cubeModelSpacePoint.x <= 0.5f &&
		cubeModelSpacePoint.y >= -0.5f && cubeModelSpacePoint.y <= 0.5f &&
		cubeModelSpacePoint.z >= -0.5f && cubeModelSpacePoint.z <= 0.5f;
}

//UNIMPLEMENTED
bool CubeModel::IntersectsPlane(glm::vec3 planePoint, glm::vec3 planeNormal)
{
	return false;
}

float CubeModel::IntersectsRay(glm::vec3 source, glm::vec3 direction)
{
	float intersection = std::numeric_limits<float>::infinity() * -1.0f;
	for (int i = 0; i != vertexPositions.size(); i += 3)
	{
		//TODO 3 - Ray casting on a cube model

		
		//Three corners of a triangle
		
		glm::vec3 p0 = GetWorldMatrix() * glm::vec4(vertexPositions.at(i), 1.0f);
		glm::vec3 p1 = GetWorldMatrix() * glm::vec4(vertexPositions.at(i + 1), 1.0f);
		glm::vec3 p2 = GetWorldMatrix() * glm::vec4(vertexPositions.at(i + 2), 1.0f);

		//define a plane based on triangle vertices
		glm::vec3 normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));


		//Check plane is facing us
		if (glm::dot(normal, direction) < 0) {
			float t = (glm::dot(normal, p0) - glm::dot(normal, source)) / glm::dot(normal, direction);
			//check collision happens forward in time
			if (t >= 0) {
				//Plane contains point, now what about the triangle?
				glm::vec3 intersectionInPlane = source + t * direction;

				//Projection onto a plane, we must pick a plane not perpendicular to triangle, to avoid collapsing the triangle into a single line
				//At least one of these is guaranteed to be non-perpendicular. All are centered at the origin
				glm::vec3 xNormal = glm::vec3(1, 0, 0);
				glm::vec3 yNormal = glm::vec3(0, 1, 0);
				glm::vec3 zNormal = glm::vec3(0, 0, 1);

				glm::vec2 a;
				glm::vec2 b;
				glm::vec2 c;
				glm::vec2 r; //Ray intersection point				

				if (glm::dot(xNormal, normal) != 0.0f)
				{ //Project on yz-plane
					a = glm::vec2(p0.y, p0.z);
					b = glm::vec2(p1.y, p1.z);
					c = glm::vec2(p2.y, p2.z);
					r = glm::vec2(intersectionInPlane.y, intersectionInPlane.z);
				}
				else if (glm::dot(yNormal, normal) != 0.0f)
				{ //Project on xz-plane
					a = glm::vec2(p0.z, p0.x);
					b = glm::vec2(p1.z, p1.x);
					c = glm::vec2(p2.z, p2.x);
					r = glm::vec2(intersectionInPlane.z, intersectionInPlane.x);
				}
				else
				{ //Project on xy-plane
					a = glm::vec2(p0.x, p0.y);
					b = glm::vec2(p1.x, p1.y);
					c = glm::vec2(p2.x, p2.y);
					r = glm::vec2(intersectionInPlane.x, intersectionInPlane.y);
				}
				float areaABC = glm::length( glm::cross(glm::vec3(b - a, 0.0f), glm::vec3(c - a, 0.0f)) ) / 2.0f;

				float areaABP = glm::length(glm::cross(glm::vec3(b - a, 0.0f), glm::vec3(r - a, 0.0f))) / 2.0f;
				float areaACP = glm::length(glm::cross(glm::vec3(r - a, 0.0f), glm::vec3(c - a, 0.0f))) / 2.0f;
				float areaBCP = glm::length(glm::cross(glm::vec3(b - r, 0.0f), glm::vec3(c - r, 0.0f))) / 2.0f;

				float alpha = areaBCP / areaABC;
				float beta =  areaACP / areaABC;
				float gamma = areaABP / areaABC; //Recalculate this!!!!

				if ((alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f) && (alpha + beta + gamma == 1.0f) )
				{
					intersection = t;
				}
			}
		}
		
	}

	return intersection;
}

bool CubeModel::ParseLine(const std::vector<ci_string> &token)
{
	if (token.empty())
	{
		return true;
	}
	else
	{
		return Model::ParseLine(token);
	}
}
