/* COMP 371 - ASSIGNMENT 2
   Daniel Lam - 40248073
*/

#include <iostream>
#include <vector>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <random>
#include <fstream> // std::ifstream
#include <sstream> // std::stringstream, std::stringbuf
using namespace std;
using namespace glm;

// GLOBAL VARIABLES
#define INFO_LOG_SIZE 512
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define ASPECT_RATIO ((float)WINDOW_WIDTH/(float)WINDOW_HEIGHT)
#define GRID_SLICES 100
#define GLM_FORCE_RADIANS

#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

float FOV = radians(90.0f);
const float FOV_MODIFIER = radians(20.0f);
const float RIGHT_ANGLE = pi<float>()/2;
const float CORNER_ANGLE = pi<float>()/4; // desired corner angle assuming starting from horizontal
const float RACKET_ANGLE = RIGHT_ANGLE - CORNER_ANGLE;
const float ARM_ANGLE = RIGHT_ANGLE - CORNER_ANGLE;
const float CAMERA_ANGLE_MODIFIER = 5.f/360.f*pi<float>();
float CAMERA_ANGULAR_SPEED = 20.0f;
const mat4 IDENTITY_MATRIX(1.0f);
const double PI = 3.1415926535897932384626433832795028841971693993751058209;
unsigned int sphereIndexCount;


// COLORS
const vec3 colorLightRed = vec3(220/255.0f, 20/255.0f, 60/255.0f);
const vec3 colorRed = vec3(1.0f, 0.0f, 0.0f);
const vec3 colorDarkRed = vec3(139/255.0f, 0.0f, 0.0f);
const vec3 colorLightYellow = vec3(1.0f, 1.0f, 153/255.0f);
const vec3 colorYellow = vec3(1.0f, 1.0f, 0.0f);
const vec3 colorDarkYellow = vec3(204/255.0f, 204/255.0f, 0.0f);
const vec3 colorLightBlue = vec3(30.0/255.0f, 144.0/255.0f, 255.0/255.0f);
const vec3 colorBlue = vec3(0.0f, 0.0f, 1.0f);
const vec3 colorDarkBlue = vec3(0.0f, 0.0f, 139.0/255.0f);
const vec3 colorLightGreen = vec3(124/255.0f, 252/255.0f, 0.0f);
const vec3 colorGreen = vec3(0.0f, 1.0f, 0.0f);
const vec3 colorDarkGreen = vec3(0.0f, 100.0/255.0f, 0.0f);
const vec3 colorBeige = vec3(238.0/255.0f, 217.0/255.0f, 196.0/255.0f);
const vec3 colorWhite = vec3(1.0f, 1.0f, 1.0f);
const vec3 colorGray = vec3(80.0/255.0f, 80.0/255.0f, 80.0/255.0f);
const vec3 colorSkybox = vec3(173/255.0f, 216/255.0f, 230/255.0f);



// SHADER SOURCES - TEXTURE
void checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

int shader(const char *vertexPath, const char *fragmentPath) // doesn't change, just receives the sources
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure &e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    int ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return ID;
}

// This function loads and compile a shader code provided in a string format.
GLuint loadAndCompileShader(const char * shaderSource, GLenum shaderType) {
    int success;
    char infoLog[INFO_LOG_SIZE];

    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderSource, NULL);
    glCompileShader(shaderId);

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::COMPILATION_FAILED\n%s\n%s", infoLog, shaderSource);
        exit(EXIT_FAILURE);
    }
    return shaderId;
}

void computeGrid(GLuint & VAOGrid, GLuint & VBOGrid, GLuint & EBOGrid, GLuint & EBOGridLength) {
    
    vector<vec3> verticesGrid;
    vector<GLuint> indicesGrid;

    GLfloat gridMiddle = GRID_SLICES/2.0f;

    // Compute vertices on X axis
    for (int i = 0; i <= GRID_SLICES; i++) {
        // set a pair of vertices to form a line, placing one on positive x and other on negative x.
        // Building lines from the negative z axis to the positive axis.
        verticesGrid.push_back(vec3(-gridMiddle, 0.0f, -gridMiddle + i));
        verticesGrid.push_back(vec3(gridMiddle, 0.0f, -gridMiddle + i));
    }

    for (int i = 0; i <= GRID_SLICES; i++) {
        // set a pair of vertices to form a line, placing one on positive x and other on negative x.
        // Building lines from the negative z axis to the positive axis.
        verticesGrid.push_back(vec3(-gridMiddle + i, 0.0f, -gridMiddle));
        verticesGrid.push_back(vec3(-gridMiddle + i, 0.0f, gridMiddle));
    }

    // Compute correspond
    for (int i = 0; i < verticesGrid.size(); i++) {
        indicesGrid.push_back(i);
    }

    glGenVertexArrays(1, &VAOGrid);
    glBindVertexArray(VAOGrid);

    
    glGenBuffers(1, &VBOGrid);
    glBindBuffer(GL_ARRAY_BUFFER, VBOGrid);
    glBufferData(GL_ARRAY_BUFFER, verticesGrid.size()*sizeof(vec3), &verticesGrid[0][0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
    glEnableVertexAttribArray(0);
    
    glGenBuffers(1, &EBOGrid);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOGrid);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesGrid.size()*sizeof(GLuint), &indicesGrid[0], GL_STATIC_DRAW);

    EBOGridLength = indicesGrid.size();
}

// drawCubeModel() sets up vertices & indices of base cube and allows rendering in either Points, Lines, Triangles (default) with respective EBO
void drawCubeModel(GLuint &VAOCubeModel, GLuint &VBOCubeModel, GLuint &EBOCubeModel, GLuint &EBOCubeModelLines, GLuint &EBOCubeModelPoints, GLuint &EBOCubeModelLength, GLuint &EBOCubeModelLinesLength, GLuint &EBOCubeModelPointsLength) {

    // VERTICES OF CUBE
    std::vector<vec3> cubeVertices = {
        // position vertices
        // front face
        vec3(-0.5f, -0.5f, 0.5f), // 0: front bottom left - normal: -x 
        vec3(-0.5f, -0.5f, 0.5f), // 1: normal: -y
        vec3(-0.5f, -0.5f, 0.5f), // 2: normal: z

        vec3(-0.5f, 0.5f, 0.5f), // 3: front top left - normal: -x 
        vec3(-0.5f, 0.5f, 0.5f), // 4: normal: y
        vec3(-0.5f, 0.5f, 0.5f), // 5: normal: z

        vec3(0.5f, 0.5f, 0.5f), // 6: front top right - normal: x
        vec3(0.5f, 0.5f, 0.5f), // 7: normal: y
        vec3(0.5f, 0.5f, 0.5f), // 8: normal: z

        vec3(0.5f, -0.5f, 0.5f), // 9: front bottom right - normal: x
        vec3(0.5f, -0.5f, 0.5f), // 10: normal: -y
        vec3(0.5f, -0.5f, 0.5f), // 11: normal: z
        
        // back face
        vec3(-0.5f, -0.5f, -0.5f), // 12: back bottom left - normal: -x
        vec3(-0.5f, -0.5f, -0.5f), // 13: normal: -y
        vec3(-0.5f, -0.5f, -0.5f), // 14: normal: -z

        vec3(-0.5f, 0.5f, -0.5f), // 15: back top left - normal: -x
        vec3(-0.5f, 0.5f, -0.5f), // 16: normal: y
        vec3(-0.5f, 0.5f, -0.5f), // 17: normal: -z

        vec3(0.5f, 0.5f, -0.5f), // 18: back top right - normal: x
        vec3(0.5f, 0.5f, -0.5f), // 19: normal: y
        vec3(0.5f, 0.5f, -0.5f), // 20: normal: -z

        vec3(0.5f, -0.5f, -0.5f), // 21: back bottom right - normal: x
        vec3(0.5f, -0.5f, -0.5f), // 22: normal: -y
        vec3(0.5f, -0.5f, -0.5f), // 23: normal: -z

    };

    std::vector<vec3> cubeNormals = {
        // normals (6 normals per face as every position vertex is being used 6 times)
        vec3(-1.0f, 0.0f, 0.0f), // front bottom left - normal: -x
        vec3(0.0f, -1.0f, 0.0f), // normal: -y
        vec3(0.0f, 0.0f, 1.0f), // normal: z

        vec3(-1.0f, 0.0f, 0.0f), // front top left - normal: -x 
        vec3(0.0f, 1.0f, 0.0f), // normal: y
        vec3(0.0f, 0.0f, 1.0f), // normal: z

        vec3(1.0f, 0.0f, 0.0f), // front top right - normal: x
        vec3(0.0f, 1.0f, 0.0f), // normal: y
        vec3(0.0f, 0.0f, 1.0f), // normal: z

        vec3(1.0f, 0.0f, 0.0f), // front bottom right - normal: x
        vec3(0.0f, -1.0f, 0.0f), // normal: -y
        vec3(0.0f, 0.0f, 1.0f), // normal: z

        vec3(-1.0f, 0.0f, 0.0f), // back bottom left - normal: -x
        vec3(0.0f, -1.0f, 0.0f), // normal: -y
        vec3(0.0f, 0.0f, -1.0f), // normal: -z

        vec3(-1.0f, 0.0f, 0.0f), // back top left - normal: -x
        vec3(0.0f, 1.0f, 0.0f), // normal: y
        vec3(0.0f, 0.0f, -1.0f), // normal: -z

        vec3(1.0f, 0.0f, 0.0f), // back top right - normal: x
        vec3(0.0f, 1.0f, 0.0f), // normal: y
        vec3(0.0f, 0.0f, -1.0f), // normal: -z

        vec3(1.0f, 0.0f, 0.0f), // back bottom right - normal: x
        vec3(0.0f, -1.0f, 0.0f), // normal: -y
        vec3(0.0f, 0.0f, -1.0f), // normal: -z
    }; 

    std::vector<vec2> cubeUV = {
        // front face
        vec2(0.0f, 0.0f),
        vec2(0.0f, 1.0f),
        vec2(1.0f, 1.0f),

        vec2(1.0f, 1.0f),
        vec2(1.0f, 0.0f),
        vec2(0.0f, 0.0f),

        // back face
        vec2(1.0f, 0.0f),
        vec2(0.0f, 0.0f),
        vec2(0.0f, 1.0f),

        vec2(0.0f, 1.0f),
        vec2(1.0f, 1.0f),
        vec2(1.0f, 0.0f),

        // right side face
        vec2(1.0f, 0.0f),
        vec2(0.0f, 0.0f),
        vec2(0.0f, 1.0f),

        vec2(0.0f, 1.0f),
        vec2(1.0f, 1.0f),
        vec2(1.0f, 0.0f),

        // left side face
        vec2(0.0f, 0.0f),
        vec2(1.0f, 0.0f),
        vec2(1.0f, 1.0f),

        vec2(1.0f, 1.0f),
        vec2(0.0f, 1.0f),
        vec2(0.0f, 0.0f),

        // top face
        vec2(0.0f, 0.0f),
        vec2(0.0f, 1.0f),
        vec2(1.0f, 1.0f),

        vec2(1.0f, 1.0f),
        vec2(1.0f, 0.0f),
        vec2(0.0f, 0.0f),

        // bottom face
        vec2(1.0f, 0.0f),
        vec2(0.0f, 0.0f),
        vec2(0.0f, 1.0f),

        vec2(0.0f, 1.0f),
        vec2(1.0f, 1.0f),
        vec2(1.0f, 0.0f),
    };


    // INDICES OF 3D CUBE USING GL_TRIANGLES 
    std::vector<GLuint> cubeIndices = {
        // front face - CCW - normal: z
        8, 5, 2,
        2, 11, 8,

        // back face - CW - normal: -z
        17, 20, 23,
        23, 14, 17,
        // 5, 6, 7,
        // 7, 4, 5,

        // right side face - CCW - normal: x
        6, 9, 21,
        21, 18, 6,
        // 2, 3, 7,
        // 7, 6, 2,

        //left side face - CW - normal: -x
        3, 15, 12,
        12, 0, 3,
        // 1, 5, 4,
        // 4, 0, 1,

        //top - CCW - normal: y
        16, 4, 7,
        7, 19, 16,
        // 5, 1, 2,
        // 2, 6, 5,

        // bottom - CW - normal: -y
        13, 22, 10,
        10, 1, 13,
        // 4, 7, 3,
        // 3, 0, 4
    };

    // INDICES OF LINES CUBE
    std::vector<GLuint> cubeIndicesLines = {
        //right face - normal: x
        6, 18,
        9, 21,  
        6, 9,
        18, 21,

        //left face - normal: -x
        3, 0,
        15, 12,
        3, 15,
        0, 12,
        // 1, 0, 
        // 5, 4,
        // 1, 5,
        // 0, 4,
        
        //side lines
        2, 11, // normal: z
        5, 8,
        17, 20, // normal: -z
        14, 23,
        // 0, 3, 
        // 1, 2, 
        // 5, 6, 
        // 4, 7,
    };

    // INDICES OF POINTS CUBE (just a loop from 0 to cubeIndices.size())
    std::vector<GLuint> cubeIndicesPoints;
    for (int i = 0; i < cubeIndices.size(); i++) {
        cubeIndicesPoints.push_back(i);
    }

    // Set up VAO of Cube
    glGenVertexArrays(1, &VAOCubeModel);
    glBindVertexArray(VAOCubeModel);
    
    // Set up VBO of Cube
    glGenBuffers(1, &VBOCubeModel);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCubeModel);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size()*sizeof(vec3), &cubeVertices[0][0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    // Set up normal vertex attrib pointer using same cube VBO
    glGenBuffers(1, &VBOCubeModel);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCubeModel);
    glBufferData(GL_ARRAY_BUFFER, cubeNormals.size()*sizeof(vec3), &cubeNormals[0][0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    // Set up texture vertex attrib pointer using same cube VBO
    glGenBuffers(1, &VBOCubeModel);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCubeModel);
    glBufferData(GL_ARRAY_BUFFER, cubeUV.size()*sizeof(vec2), &cubeUV[0][0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    
    // Set up EBO of 3D Cube Model - Triangles
    glGenBuffers(1, &EBOCubeModel);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOCubeModel);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size()*sizeof(GLuint), &cubeIndices[0], GL_STATIC_DRAW);

    // Set up EBO of Lines Cube
    glGenBuffers(1, &EBOCubeModelLines);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOCubeModelLines);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndicesLines.size()*sizeof(GLuint), &cubeIndicesLines[0], GL_STATIC_DRAW);

    // Set up EBO of Points Cube
    glGenBuffers(1, &EBOCubeModelPoints);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOCubeModelPoints);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndicesPoints.size()*sizeof(GLuint), &cubeIndicesPoints[0], GL_STATIC_DRAW);
    
    EBOCubeModelLength = cubeIndices.size();
    EBOCubeModelLinesLength = cubeIndicesLines.size();
    EBOCubeModelPointsLength = cubeIndicesPoints.size();
}

// drawRacket() sets up all vertices and matrix transformations to render racket model
void drawRacket(mat4 groupRacketMatrix, GLuint colorLocation, GLuint modelMatrixLocation, GLuint EBOCubeModelLength, GLuint currentRenderingMode) {

    // RACKET

    // Dimensions
    const float racketThickness = 0.2f;
    const float racketNetThickness = 0.05f;
    const float racketLength = 3.0f;
    const float racketCornerLength = 1.5f;
    const float racketNetDeltaX = 0.2f;
    const float racketNetDeltaY = 0.2f;

    // remember: angle is from the vertical
    const float racketCornerUnitX = sin(RACKET_ANGLE) * racketCornerLength; // corner unit x located at top left corner of racketCorner
    const float racketCornerUnitY = cos(RACKET_ANGLE) * racketCornerLength;

    // All racket components
    mat4 racketHandleAndSideDimensions(1.0f);
    mat4 racketCornerMatrix(1.0f);
    mat4 racketBottomLeftCornerMatrix(1.0f);
    mat4 racketBottomRightCornerMatrix(1.0f);
    mat4 racketLeftSideMatrix(1.0f);
    mat4 racketRightSideMatrix(1.0f);
    mat4 racketBottomSideMatrix(1.0f);
    mat4 racketTopSideMatrix(1.0f);
    vec3 translationAboveGrid = vec3(0.0f, 0.5f, 0.0f);

    // Set up matrices transformations
    const float offsetY = racketThickness/2;

    mat4 racketHandleAndSideScale = scale(IDENTITY_MATRIX, vec3(racketThickness, racketLength, racketThickness));
    mat4 racketCornerScale = scale(IDENTITY_MATRIX, vec3(racketThickness, racketCornerLength, racketThickness));
    mat4 racketTranslateAboveGrid = translate(IDENTITY_MATRIX, translationAboveGrid);
    mat4 racketCornerTranslation = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength, 0.0f));
    mat4 racketRotation = rotate(IDENTITY_MATRIX, RACKET_ANGLE, vec3(0.0f, 0.0f, 1.0f));

    mat4 racketLeftSideTranslate = translate(IDENTITY_MATRIX, vec3(-racketCornerUnitX, racketLength + racketCornerUnitY - offsetY, 0.0f));
    mat4 racketRightSideTranslate = translate(IDENTITY_MATRIX, vec3(racketCornerUnitX, racketLength + racketCornerUnitY - offsetY, 0.0f));

    mat4 racketTopAndBottomSideScale = scale(IDENTITY_MATRIX, vec3(racketCornerUnitX * 2, racketThickness, racketThickness));
    mat4 racketBottomSideTranslation = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY, 0.0f));
    mat4 racketTopSideTranslation = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY + racketLength - racketThickness, 0.0f ));

    // Calculate matrices transformations for each component of the racket
    racketHandleAndSideDimensions = racketHandleAndSideScale * racketTranslateAboveGrid;
    
    racketCornerMatrix = racketCornerScale * racketTranslateAboveGrid;
    racketBottomLeftCornerMatrix =  racketCornerTranslation * racketRotation * racketCornerMatrix;
    racketBottomRightCornerMatrix = racketCornerTranslation * inverse(racketRotation) * racketCornerMatrix;
    racketLeftSideMatrix = racketLeftSideTranslate * racketHandleAndSideDimensions;
    racketRightSideMatrix = racketRightSideTranslate * racketHandleAndSideDimensions;
    racketBottomSideMatrix = racketBottomSideTranslation * racketTopAndBottomSideScale;
    racketTopSideMatrix = racketTopSideTranslation * racketTopAndBottomSideScale;

    
    // RACKET - Render entire model of racket
    // Racket Handle
    glUniform4fv(colorLocation, 1, &colorRed[0]);
    mat4 groupRacketHandleModel = groupRacketMatrix * racketHandleAndSideDimensions;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketHandleModel)[0][0]); 
    glDrawElements(currentRenderingMode, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Bottom Left Corner
    glUniform4fv(colorLocation, 1, &colorWhite[0]);
    mat4 groupRacketBottomLeftCornerMatrix =  groupRacketMatrix * racketBottomLeftCornerMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketBottomLeftCornerMatrix)[0][0]); 
    glDrawElements(currentRenderingMode, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Bottom Right Corner
    mat4 groupRacketBottomRightCornerMatrix =  groupRacketMatrix * racketBottomRightCornerMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketBottomRightCornerMatrix)[0][0]); 
    glDrawElements(currentRenderingMode, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Bottom Side
    glUniform4fv(colorLocation, 1, &colorRed[0]);
    mat4 groupRacketBottomSideMatrix =  groupRacketMatrix * racketBottomSideMatrix;
    // mat4 groupRacketBottomSideMatrix = racketBottomSideMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketBottomSideMatrix)[0][0]);
    glDrawElements(currentRenderingMode, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Left Side 
    mat4 groupRacketLeftSideMatrix =  groupRacketMatrix * racketLeftSideMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketLeftSideMatrix)[0][0]); 
    glDrawElements(currentRenderingMode, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Right Side
    mat4 groupRacketRightSideMatrix =  groupRacketMatrix * racketRightSideMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketRightSideMatrix)[0][0]); 
    glDrawElements(currentRenderingMode, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Top Side
    mat4 groupRacketTopSideMatrix =  groupRacketMatrix * racketTopSideMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketTopSideMatrix)[0][0]); 
    glDrawElements(currentRenderingMode, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // RACKET NET

    // Racket Net - PLEASE REWRITE CODE USING FOR LOOP
    glUniform4fv(colorLocation, 1, &colorGreen[0]);

    vector<mat4> netMatrices;

    GLfloat netHeight = racketLength;
    GLfloat netWidth = racketCornerUnitX * 2;

    GLfloat netSpacing = 0.15 * netWidth;
    GLfloat netStringThickness = 0.15 * netSpacing;

    vec3 horizontalNetDimensions(netWidth, netStringThickness, netStringThickness);
    vec3 verticalNetDimensions(netStringThickness, netHeight - 0.25f, netStringThickness);
    mat4 netTranslationAboveGrid = translate(IDENTITY_MATRIX, translationAboveGrid);

    // Set up horizontal  nets on y-axis
    vec3 netStringPosition(-netWidth/2, racketThickness/2, 0.0f);
    mat4 netStringMatrix = scale(IDENTITY_MATRIX, verticalNetDimensions) * netTranslationAboveGrid;

    while (netStringPosition.x < netWidth/2) {
        netMatrices.push_back(racketBottomSideTranslation * translate(IDENTITY_MATRIX, netStringPosition) * netStringMatrix);
        netStringPosition.x += netSpacing + verticalNetDimensions.x;
    }

    //Set up Vertical Nets on x-axis
    netStringPosition.y = netSpacing;
    netStringPosition.x = 0.0f;
    netStringMatrix = scale(IDENTITY_MATRIX, horizontalNetDimensions) * netTranslationAboveGrid;

    while (netStringPosition.y < netHeight) {
        netMatrices.push_back(racketBottomSideTranslation * translate(IDENTITY_MATRIX, netStringPosition) * netStringMatrix);
        netStringPosition.y += netSpacing + horizontalNetDimensions.y;
    }

    // Loop through all of net matrices and draw
    for (int i = 0; i < netMatrices.size(); i++) {
        mat4 groupNetMatrix = groupRacketMatrix * netMatrices[i];
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &groupNetMatrix[0][0]);
        glDrawElements(currentRenderingMode, EBOCubeModelLength, GL_UNSIGNED_INT, 0);
    }

}

// drawFloor() sets up scale and translation for the floor cube
void drawFloor(GLuint colorLocation, GLuint modelMatrixLocation, GLuint EBOCubeModelLength) {
    mat4 floorScale = scale(IDENTITY_MATRIX, vec3(GRID_SLICES, 0.5f, GRID_SLICES));
    mat4 floorTranslation = translate(IDENTITY_MATRIX, vec3(0.0f, -0.55f, 0.0f));

    mat4 floorMatrix = floorTranslation * floorScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &floorMatrix[0][0]);
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

}

// loadTexture() takes a filename and renders a certain texture for use
GLuint loadTexture(const char *filename)
{
  // Step1 Create and bind textures
  GLuint textureId = 0;
  glGenTextures(1, &textureId);
  assert(textureId != 0);


  glBindTexture(GL_TEXTURE_2D, textureId);

  // Step2 Set filter parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Step3 Load Textures with dimension data
  int width, height, nrChannels;
  unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);

  if(stbi_failure_reason())
    std::cout << stbi_failure_reason();

  if (!data)
  {
    std::cerr << "\nError::Texture could not load texture file:" << filename << std::endl;
    return 0;
  }

  // Step4 Upload the texture to the PU
  GLenum format = 0;
  if (nrChannels == 1)
      format = GL_RED;
  else if (nrChannels == 3)
      format = GL_RGB;
  else if (nrChannels == 4)
      format = GL_RGBA;
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
               0, format, GL_UNSIGNED_BYTE, data);

  // Step5 Free resources
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0);
  return textureId;
}

float returnRandomNumber() {
    GLfloat min = -GRID_SLICES/2.0f;
    GLfloat max = GRID_SLICES/2.0f;
    double randomNum = min + (((double)rand())/RAND_MAX * (max - min));
    return randomNum;
}

void setUseTexture(int shaderProgram, int status){
    glUseProgram(shaderProgram);
    GLuint useTextureLocation = glGetUniformLocation(shaderProgram, "enableTexture");
    glUniform1i(useTextureLocation, status);
}

// SPHERE
static GLuint index(GLuint i, GLuint j, GLuint verticesPerSlice) {
    return i * verticesPerSlice + j;    
}

void computeIndices(GLuint subDivisions, GLuint verticesSize, vector<GLuint> & indices) {
    GLuint numVerticesPerSidePerFace = subDivisions + 1;
    GLuint numVerticesPerFace = numVerticesPerSidePerFace * numVerticesPerSidePerFace;
    GLuint numFaces = verticesSize/(numVerticesPerFace);

    for (GLuint i = 0; i < numFaces; i++) {
        for (int y = i *numVerticesPerSidePerFace; y < (i+1)*numVerticesPerSidePerFace - 1; y++) {
            for (int x = 0; x < numVerticesPerSidePerFace-1; x++) {
                // bottom left
                indices.push_back(index(y,x, numVerticesPerSidePerFace));
                // bottom right
                indices.push_back(index(y,x+1, numVerticesPerSidePerFace));
                // top right
                indices.push_back(index(y+1,x+1, numVerticesPerSidePerFace));
                // top right
                indices.push_back(index(y+1,x+1, numVerticesPerSidePerFace));
                // top left
                indices.push_back(index(y+1,x, numVerticesPerSidePerFace));
                // bottom left
                indices.push_back(index(y,x, numVerticesPerSidePerFace));
            }
        }
    }
}

void computeVertices(GLuint subdivisionsPerFace, vector<vec3> &vertices) {
    GLfloat increments = 90.f/subdivisionsPerFace;

    // Compute x+ axis
     // latitude
    for (GLfloat theta = -45.f; theta <= 45.f; theta += increments) {
        GLfloat thetaRads = radians(theta);
        vec3 n1(-sin(thetaRads), cosf(thetaRads), 0);
        // longitude
        for (GLfloat phi = -45.f; phi <= 45.f; phi += increments) {
            float phiRads = radians(phi);
            vec3 n2(sinf(phiRads), 0, cos(phiRads));
            vertices.push_back(normalize(cross(n1, n2)));
        }
    }

    // Compute x-
    GLuint numOfVerticesPerFace = vertices.size();
    for (GLuint i = 0; i<numOfVerticesPerFace; i++) {
        vertices.push_back(vec3(-vertices[i].x, vertices[i].y, -vertices[i].z));
    }

    // Compute y+, y-
    for (GLuint i = 0; i < numOfVerticesPerFace*2; i++) {
        vertices.push_back(vec3(vertices[i].y, -vertices[i].z, -vertices[i].x));
    }

    // Compute z+, z-
    for (GLuint i = 0; i < numOfVerticesPerFace*2; i++) {
        vertices.push_back(vec3(-vertices[i].y, vertices[i].x, vertices[i].z));
    }
    
}

void computeUV(GLuint verticesSize, vector<vec2> &sphereUV) {
    int verticesCountPerSquare = 6;
    for (int i = 0; i < verticesSize; i += verticesCountPerSquare) {
        // bottom left
        sphereUV.push_back(vec2(0,0));
        // bottom right
        sphereUV.push_back(vec2(0,1));
        // top right
        sphereUV.push_back(vec2(1,1));
        // top right
        sphereUV.push_back(vec2(1,1));
        // top left
        sphereUV.push_back(vec2(0,1));
        // bottom left
        sphereUV.push_back(vec2(0,0));
    }
}

GLuint createSphere() {

    GLuint sphereVAO, sphereVBOVertices, sphereVBONormals, sphereVBOUV;

    GLuint subDivisions = 7;
    vector<vec3> tmpVertices;
    vector<vec3> vertices;

    computeVertices(subDivisions, tmpVertices);
    
    vector<GLuint> tmindices;
    computeIndices(subDivisions, tmpVertices.size(), tmindices);

    // we need to repeat some values for textures, therefore we recompute vertices based on indices values
    for (GLuint i = 0; i<tmindices.size(); i++) {
        vertices.push_back(tmpVertices[tmindices[i]]);
    }
    // remove indices
    vector<vec2> UV;
    computeUV(vertices.size(), UV);

    // Set up VAO of Sphere
    glGenVertexArrays(1, &sphereVAO);
    glBindVertexArray(sphereVAO);
    
    // Set up VBO of Sphere
    glGenBuffers(1, &sphereVBOVertices);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBOVertices);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vec3), &vertices[0][0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    // Set up normal vertex attrib pointer using same cube VBO
    glGenBuffers(1, &sphereVBONormals);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBONormals);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vec3), &vertices[0][0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    // Set up texture vertex attrib pointer using same cube VBO
    glGenBuffers(1, &sphereVBOUV);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBOUV);
    glBufferData(GL_ARRAY_BUFFER, UV.size()*sizeof(vec2), &UV[0][0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    sphereIndexCount = vertices.size();
 
    return sphereVAO;
}

int main(int argc, char* argv[]) {

    // Init the glfw library and set various configurations.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a glfw window with specified size
    // glfwCreateWindow(width, size, title, monitor, share)
    GLFWwindow * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "COMP 371 - Assignment 2", NULL, NULL);
    // If cannot create window we abort the program.
    if (!window) {
        std::cerr << "Failed to open GLFW Window." << std::endl;
        exit(EXIT_FAILURE);
    }
    // Set context current context to this window.
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    // Set experimental features needed for core profile
    glewExperimental = true;
    // init glew and check if it was properly init.
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to init GLEW" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    int success;
    char infoLog[INFO_LOG_SIZE];

    // -------------------- COMPILE SHADERS ---------------
    
    GLuint textureShaderProgram = shader("../Shaders/textureVertexShaderSource.glsl", "../Shaders/textureFragmentShaderSource.glsl");
    GLuint depthShaderProgram = shader("../Shaders/depthVertexShaderSource.glsl", "../Shaders/depthFragmentShaderSource.glsl");

    // ------------ END --------------

    // Assign OpenGL to use shaderProgram that we've set
    glUseProgram(textureShaderProgram);

    // Initialize uniform for color
    GLuint colorLocation = glGetUniformLocation(textureShaderProgram, "color");

    // Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Background color
    glClearColor(57/255.0f, 62/255.0f, 67/255.0f, 1.0f);

    // -------------- 2. INITIALIZE VAOs ---------------

    // VAOGrid for grid
    GLuint VAOGrid, VBOGrid, EBOGrid, EBOGridLength;
    computeGrid(VAOGrid, VBOGrid, EBOGrid, EBOGridLength);

    // VAOSphere for sphere
    GLuint VAOSphere = createSphere();

    // VAO for base Cube Model
    GLuint VAOCubeModel, VBOCubeModel, EBOCubeModel, EBOCubeModelLength;
    GLuint EBOCubeModelPoints, EBOCubeModelPointsLength, EBOCubeModelLines, EBOCubeModelLinesLength;
    drawCubeModel(VAOCubeModel, VBOCubeModel, EBOCubeModel, EBOCubeModelLines, EBOCubeModelPoints, EBOCubeModelLength, EBOCubeModelLinesLength, EBOCubeModelPointsLength);

    GLuint currentCubeEBO = EBOCubeModel;
    GLuint currentRenderingMode = GL_TRIANGLES;
    GLuint currentCubeEBOLength = EBOCubeModelLength;


    // unbind VAOGrid, VBOGrid and EBOGrid for clean up
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // --------------- 2.5 TEXTURES ------------------------------

    GLuint clayTextureId = loadTexture("/COMP371/src/textures/clay.jpg");
    GLuint glossyTextureId = loadTexture("/COMP371/src/textures/glossy.jpg");

    // --------------- 3. INITIALIZE MATRICES AND RENDER -----------

    // Initialize MVP matrices of the entire world to identity matrices
    mat4 modelMatrix(1.0f);
    mat4 viewMatrix(1.0f);
    mat4 projectionMatrix(1.0f);

    // Initialize matrices of arm and racket objects
    mat4 upperArmMatrix(1.0f);
    mat4 foreArmMatrix(1.0f);

    // Same initial dimensions and translation for both upper arm and fore arm
    vec3 armDimensions = vec3(0.4f, 3.5f, 0.4f);
    vec3 translationAboveGrid = vec3(0.0f, 0.5f, 0.0f);

    // Movement Matrices & Variables
    GLfloat movementSpeed = 1.0f;
    GLfloat scaleSpeed = 0.01f;
    mat4 movementTranslationMatrix(1.0f);
    mat4 movementScaleMatrix(1.0f);

    // ----------- INITIAL TRANSFORMATIONS TO ARM MODEL -----------
    // --> Upper Arm
    mat4 upperArmScale = scale(IDENTITY_MATRIX, armDimensions);
    mat4 upperArmTranslation = translate(IDENTITY_MATRIX, translationAboveGrid);
    upperArmMatrix = upperArmScale * upperArmTranslation;

    // --> Fore Arm
    mat4 foreArmScale = scale(IDENTITY_MATRIX, armDimensions);
    mat4 foreArmTranslate = translate(IDENTITY_MATRIX, translationAboveGrid);
    foreArmMatrix = foreArmScale * foreArmTranslate;


    // ---------------- 4. HIERARCHICAL MODELING ------------------

    // HIERARCHICAL MODELING
    // ----------- SET UP SHOULDER GROUP MATRIX -------------
    mat4 groupShoulderMatrix(1.0f);
    mat4 groupShoulderMatrixRotation = rotate(IDENTITY_MATRIX, -ARM_ANGLE, vec3(0.0f, 0.0f, 1.0f));
    groupShoulderMatrix = groupShoulderMatrixRotation;


    // ----------- SET UP ELBOW GROUP MATRIX ------------
    mat4 groupElbowMatrix(1.0f);
    mat4 groupElbowMatrixTranslate = translate(IDENTITY_MATRIX, vec3(armDimensions.x/2, armDimensions.y, 0.0f));
    mat4 groupElbowMatrixRotation = rotate(IDENTITY_MATRIX, ARM_ANGLE, vec3(0.0f, 0.0f, 1.0f));
    mat4 groupElbowMatrixTranslateUndo = translate(IDENTITY_MATRIX, vec3(-armDimensions.x/2, 0.0f, 0.0f)); // undo previous translation so rotation is done properly
    groupElbowMatrix = groupElbowMatrixTranslate * groupElbowMatrixRotation * groupElbowMatrixTranslateUndo;

    // ----------- SET UP WRIST GROUP MATRIX -----------
    mat4 groupWristMatrix(1.0f);
    mat4 groupWristMatrixTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, armDimensions.y, 0.0f));
    groupWristMatrix = groupWristMatrixTranslate;
    
    // ---------------- 5. CAMERA ------------------

    // CAMERA
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;
    float theta = radians(cameraHorizontalAngle);
    float phi = radians(cameraVerticalAngle);
    mat4 viewRotation(1.0f);

    // vec3 cameraPosition = vec3(0.0f, 10.0f, 15.0f);
    vec3 cameraPosition = vec3(0.0f, 8.0f, 15.0f);
    vec3 cameraLookAt = vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));
    // vec3 cameraLookAt = vec3(0.0f, 0.0f, 0.0f);
    vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

    // Initial lookAt camera angle
    mat4 initialWorldLookAt = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
    mat4 currentWorldLookAt = viewRotation * initialWorldLookAt;
    viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

    // MOUSE
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

    // Sets cursor in the middle
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ----------------- 5. LIGHT -------------------
    vec3 lightPos(-2.0f, 20.0f, 10.0f);
    GLuint lightLocation = glGetUniformLocation(textureShaderProgram, "lightPos");
    glUniform3fv(lightLocation, 1, &lightPos[0]);

    // ----------------- 6. SHADOW ------------------
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    int shadowWidth = 4096;
    int shadowHeight = 4096;

    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0); //depth component of our buffer
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    glDrawBuffer(GL_NONE);

    // Set LookAt at ViewMatrix to see corner of cube (default camera position)
    GLuint viewMatrixLocation = glGetUniformLocation(textureShaderProgram, "view");
    glUniformMatrix4fv(viewMatrixLocation, 1 , GL_FALSE, &viewMatrix[0][0]);

    // Set Projection Matrix to perspective view as default
    projectionMatrix = perspective(FOV, ASPECT_RATIO, 0.001f, 1000.0f);
    GLuint projectionMatrixLocation = glGetUniformLocation(textureShaderProgram, "proj");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    GLuint modelMatrixLocation = glGetUniformLocation(textureShaderProgram, "model");
    GLuint modelMatrixDepthLocation = glGetUniformLocation(depthShaderProgram, "model");

    // Light Space Matrices for shadow
    GLuint lightSpaceMatrixLocation = glGetUniformLocation(textureShaderProgram, "lightSpaceMatrix");
    GLuint lightSpaceMatrixDepthLocation = glGetUniformLocation(depthShaderProgram, "lightSpaceMatrix");

    GLint frameBufferWidth, frameBufferHeight;
    glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);


    // Inputs
    GLfloat lastFrameTime = glfwGetTime();
    GLuint spacebarLastState = glfwGetKey(window, GLFW_KEY_SPACE);

    int enableTexture = 0;
    int lastTstate = GLFW_RELEASE;

    while(!glfwWindowShouldClose(window)) {

        // TIMER CODE
        // dt = delta time
        GLfloat dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

         // Calculate Mouse Movement every frame
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);
        
        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;
        
        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;

        // FIRST SHADOW PASS RUN ALGORITHM - Set up
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glViewport(0, 0, shadowWidth, shadowHeight);

        glClear(GL_DEPTH_BUFFER_BIT);

        glUseProgram(depthShaderProgram);

        // GENERATE DEPTH DATA FOR SHADOW
        mat4 lightViewMatrix, lightProjectionMatrix;
        lightViewMatrix = lookAt(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
        lightProjectionMatrix = ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 50.0f);

        mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

        glUniformMatrix4fv(lightSpaceMatrixDepthLocation, 1, GL_FALSE, &lightSpaceMatrix[0][0]);

        // FIRST SHADOW PASS RUN ALGORITHM - DrawScene()
        #pragma region

        glUniform4fv(colorLocation, 1, &colorYellow[0]);
        drawFloor(colorLocation, modelMatrixDepthLocation, EBOCubeModelLength); // draw floor as clay

        // GRID - Render the X,Z plane grid
        glUniform4fv(colorLocation, 1, &colorDarkYellow[0]);
        glBindVertexArray(VAOGrid);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOGrid);
        glUniformMatrix4fv(modelMatrixDepthLocation, 1, GL_FALSE, &modelMatrix[0][0]);
        glDrawElements(GL_LINES, EBOGridLength, GL_UNSIGNED_INT, 0);

        // Sphere
        mat4 sphereTranslationAboveGrid = translate(IDENTITY_MATRIX, vec3(0.0f, 2.5f, 0.0f));   

        glUniform4fv(colorLocation, 1, &colorGreen[0]);
        glBindVertexArray(VAOSphere);
        glUniformMatrix4fv(modelMatrixDepthLocation , 1, GL_FALSE, &(sphereTranslationAboveGrid)[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, sphereIndexCount);

        // CUBE - Render the base 3D Cube
        mat4 cubeScale = scale(IDENTITY_MATRIX, vec3(10.0f, 10.0f, 10.0f));
        mat4 cubeTranslationAboveGrid = translate(IDENTITY_MATRIX, vec3(0.0f, 2.5f, -2.0f));

        glUniform4fv(colorLocation, 1, &colorBeige[0]);
        glBindVertexArray(VAOCubeModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentCubeEBO);
        // glUniformMatrix4fv(modelMatrixDepthLocation, 1, GL_FALSE, &(cubeTranslationAboveGrid * cubeScale)[0][0]);
        // glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        // // ARM - Groups of Shoulder, Elbow & Hand
        mat4 groupShoulder = groupShoulderMatrix * upperArmMatrix;
        glUniformMatrix4fv(modelMatrixDepthLocation, 1, GL_FALSE, &(groupShoulder)[0][0]); // Drawing upper arm
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);
        mat4 groupElbow = groupShoulderMatrix * groupElbowMatrix * foreArmMatrix;
        glUniformMatrix4fv(modelMatrixDepthLocation, 1, GL_FALSE, &(groupElbow)[0][0]); // Drawing fore arm
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        // // RACKETS - Render the racket
        glUniform4fv(colorLocation, 1, &colorRed[0]);
        mat4 groupRacket = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix;
        drawRacket(groupRacket, colorLocation, modelMatrixDepthLocation, EBOCubeModelLength, currentRenderingMode); // racket D

        #pragma endregion




        // Reset back to original textureShader
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(textureShaderProgram);

        glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, &lightSpaceMatrix[0][0]);

        glViewport(0, 0, frameBufferWidth, frameBufferHeight);

        // clear current buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        GLuint modelMatrixLocation = glGetUniformLocation(textureShaderProgram, "model");
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        // Set LookAt at ViewMatrix to see corner of cube (default camera position)
        GLuint viewMatrixLocation = glGetUniformLocation(textureShaderProgram, "view");
        glUniformMatrix4fv(viewMatrixLocation, 1 , GL_FALSE, &viewMatrix[0][0]);

        // Set Projection Matrix to perspective view as default
        projectionMatrix = perspective(FOV, ASPECT_RATIO, 0.001f, 1000.0f);
        GLuint projectionMatrixLocation = glGetUniformLocation(textureShaderProgram, "proj");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);


        


        // SECOND SHADOW ALGO PASS - Set up
        GLuint lightPosLocation = glGetUniformLocation(textureShaderProgram, "lightPos");
        glUniform3fv(lightPosLocation, 1, &lightPos[0]);


        // ACTIVATE TEXTURES
        GLuint shadowMapLocation = glGetUniformLocation(textureShaderProgram, "shadowMap");
        glUniform1i(shadowMapLocation, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthTexture);

        GLuint textureLocation = glGetUniformLocation(textureShaderProgram, "textureSampler");
        glUniform1i(textureLocation, 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, clayTextureId);
        glUniformMatrix4fv(lightSpaceMatrixDepthLocation, 1, GL_FALSE, &lightSpaceMatrix[0][0]);

        // SECOND SHADOW ALGO PASS - DrawScene()
        #pragma region
        glUniform4fv(colorLocation, 1, &colorYellow[0]);
        drawFloor(colorLocation, modelMatrixLocation, EBOCubeModelLength); // draw floor as clay

        // Set Texture back to glossy
        glBindTexture(GL_TEXTURE_2D, glossyTextureId);

        // GRID - Render the X,Z plane grid
        glUniform4fv(colorLocation, 1, &colorDarkYellow[0]);
        glBindVertexArray(VAOGrid);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOGrid);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
        glDrawElements(GL_LINES, EBOGridLength, GL_UNSIGNED_INT, 0);

        // Sphere
        glUniform4fv(colorLocation, 1, &colorGreen[0]);
        glBindVertexArray(VAOSphere);
        glUniformMatrix4fv(modelMatrixLocation , 1, GL_FALSE, &(sphereTranslationAboveGrid)[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, sphereIndexCount);
        // glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);

        // CUBE - Render the base 3D Cube
        cubeScale = scale(IDENTITY_MATRIX, vec3(10.0f, 10.0f, 10.0f));
        cubeTranslationAboveGrid = translate(IDENTITY_MATRIX, vec3(0.0f, 2.5f, -2.0f));

        glUniform4fv(colorLocation, 1, &colorBeige[0]);
        glBindVertexArray(VAOCubeModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentCubeEBO);
        // glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(cubeTranslationAboveGrid * cubeScale)[0][0]);
        // glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        // // ARM - Groups of Shoulder, Elbow & Hand
        groupShoulder = groupShoulderMatrix * upperArmMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupShoulder)[0][0]); // Drawing upper arm
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);
        groupElbow = groupShoulderMatrix * groupElbowMatrix * foreArmMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupElbow)[0][0]); // Drawing fore arm
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        // // RACKETS - Render the racket
        glUniform4fv(colorLocation, 1, &colorRed[0]);
        groupRacket = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix;
        drawRacket(groupRacket, colorLocation, modelMatrixLocation, EBOCubeModelLength, currentRenderingMode); // racket D
       
        #pragma endregion

        // glUseProgram(textureShaderProgram);
        // glBindVertexArray(VAOCubeModel);

        



        // // GLuint texturedLightLocation = glGetUniformLocation(textureShaderProgram, "lightPos");
        // // glUniform3fv(texturedLightLocation, 1, &lightPos[0]);

        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, clayTextureId);

        
        // swap the buffer to the one already written, and place the current one as a canvas
        glfwSwapBuffers(window);
        glfwPollEvents();

        bool isShiftKeyPressed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

        // CHANGING RENDERING MODE - P, L, T keys
        // KEY "P" PRESS - Uses EBO Points Cube and Sets Rendering Mode of Model to GL_POINTS
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            currentCubeEBO = EBOCubeModelPoints;
            currentRenderingMode = GL_POINTS;
            currentCubeEBOLength = EBOCubeModelPointsLength;
        }

        // KEY "L" PRESS - Uses EBO Lines Cube and Sets Rendering Mode of Model to GL_LINES
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            currentCubeEBO = EBOCubeModelLines;
            currentRenderingMode = GL_LINES;
            currentCubeEBOLength = EBOCubeModelLinesLength;
        }

        // KEY "T" PRESS - Uses EBO Triangle Cube and sets Rendering Mode of Model to GL_TRIANGLES (3D)
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            currentCubeEBO = EBOCubeModel;
            currentRenderingMode = GL_TRIANGLES;
            currentCubeEBOLength = EBOCubeModelLength;
        }

        // SCALING UP AND DOWN - U, J keys
        // Scale Up
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
            mat4 distanceScale = scale(IDENTITY_MATRIX, vec3(1.00f + scaleSpeed, 1.00f + scaleSpeed, 1.00f + scaleSpeed));
            groupShoulderMatrix = inverse(movementScaleMatrix) * groupShoulderMatrix;
            movementScaleMatrix = distanceScale * movementScaleMatrix;
            groupShoulderMatrix = movementScaleMatrix * groupShoulderMatrix;
        }

        // Scale Down
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
            mat4 distanceScale = scale(IDENTITY_MATRIX, vec3(1.00f - scaleSpeed, 1.00f - scaleSpeed, 1.00f - scaleSpeed));
            groupShoulderMatrix = inverse(movementScaleMatrix) * groupShoulderMatrix;
            movementScaleMatrix = distanceScale * movementScaleMatrix;
            groupShoulderMatrix = movementScaleMatrix * groupShoulderMatrix;
        }

        // CONTROL MODEL POSITION AND ORIENTATION - WASD keys
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && isShiftKeyPressed) {
            float travelDistance = movementSpeed * dt;
            mat4 distanceTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, travelDistance, 0.0f) );
            groupShoulderMatrix = inverse(movementTranslationMatrix) * groupShoulderMatrix;
            movementTranslationMatrix = distanceTranslate * movementTranslationMatrix;
            groupShoulderMatrix = movementTranslationMatrix * groupShoulderMatrix;
        }
        // not mandatory but added movement for z axis too
        else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            float travelDistance = movementSpeed * dt;
            mat4 rotationTransformation = rotate(mat4(1.0f), -1.f/360.f*pi<float>(), vec3(1.f, 0.f, 0.f));
            groupShoulderMatrix = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupShoulderMatrix;
        }
        
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && isShiftKeyPressed) {

            float travelDistance = movementSpeed * dt;
            mat4 distanceTranslate = translate(IDENTITY_MATRIX, vec3(-travelDistance, 0.0f, 0.0f) );
            groupShoulderMatrix = inverse(movementTranslationMatrix) * groupShoulderMatrix;
            movementTranslationMatrix = distanceTranslate * movementTranslationMatrix;
            groupShoulderMatrix = movementTranslationMatrix * groupShoulderMatrix;
            

        } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            float travelDistance = movementSpeed * dt;
            mat4 rotationTransformation = rotate(mat4(1.0f), 5.f/360.f*pi<float>(), vec3(0.f, 1.f, 0.f));
            groupShoulderMatrix = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupShoulderMatrix;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && isShiftKeyPressed) {
            float travelDistance = movementSpeed * dt;
            mat4 distanceTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, -travelDistance, 0.0f) );
            groupShoulderMatrix = inverse(movementTranslationMatrix) * groupShoulderMatrix;
            movementTranslationMatrix = distanceTranslate * movementTranslationMatrix;
            groupShoulderMatrix = movementTranslationMatrix * groupShoulderMatrix;

        }
        // not mandatory but added movement for -z axis too 
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            float travelDistance = movementSpeed * dt;
            mat4 rotationTransformation = rotate(IDENTITY_MATRIX, 1.f/360.f*pi<float>(), vec3(1.0f, 0.0f, 0.0f));
            groupShoulderMatrix = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupShoulderMatrix;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && isShiftKeyPressed) {
            float travelDistance = movementSpeed * dt;
            mat4 distanceTranslate = translate(IDENTITY_MATRIX, vec3(travelDistance, 0.0f, 0.0f) );
            groupShoulderMatrix = inverse(movementTranslationMatrix) * groupShoulderMatrix;
            movementTranslationMatrix = distanceTranslate * movementTranslationMatrix;
            groupShoulderMatrix = movementTranslationMatrix * groupShoulderMatrix;
            
        } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            float travelDistance = movementSpeed * dt;
            mat4 rotationTransformation = rotate(IDENTITY_MATRIX, -5.0f/360.f*pi<float>(), vec3(0.0f, 1.0f, 0.0f));
            groupShoulderMatrix = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupShoulderMatrix;
        }

        // ROTATION AROUND ELBOW AND WRIST
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            float travelDistance = movementSpeed * dt;
            mat4 rotationTransformation = rotate(IDENTITY_MATRIX, -5.0f/360.f*pi<float>(), vec3(0.0f, 1.0f, 0.0f));
            groupElbowMatrix = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupElbowMatrix;
            
        } else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            float travelDistance = movementSpeed * dt;
            mat4 rotationTransformation = rotate(IDENTITY_MATRIX, 5.0f/360.f*pi<float>(), vec3(0.0f, 1.0f, 0.0f));
            groupElbowMatrix = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupElbowMatrix;
        }

        // ROTATION AROUND ELBOW AND WRIST
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            float travelDistance = movementSpeed * dt;
            mat4 rotationTransformation = rotate(IDENTITY_MATRIX, -5.0f/360.f*pi<float>(), vec3(0.0f, 1.0f, 0.0f));
            groupElbowMatrix = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupElbowMatrix;
            
        } else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            float travelDistance = movementSpeed * dt;
            mat4 rotationTransformation = rotate(IDENTITY_MATRIX, 5.0f/360.f*pi<float>(), vec3(0.0f, 1.0f, 0.0f));
            groupElbowMatrix = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupElbowMatrix;
        }

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            float travelDistance = movementSpeed * dt;
            mat4 rotationTransformation = rotate(IDENTITY_MATRIX, -5.0f/360.f*pi<float>(), vec3(0.0f, 1.0f, 0.0f));
            groupElbowMatrix = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupElbowMatrix;
            
        } else if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            float travelDistance = movementSpeed * dt;
            mat4 rotationTransformation = rotate(IDENTITY_MATRIX, 5.0f/360.f*pi<float>(), vec3(0.0f, 1.0f, 0.0f));
            groupElbowMatrix = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupElbowMatrix;
        }
        
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            float travelDistance = movementSpeed * dt;
            mat4 rotationTransformation = rotate(IDENTITY_MATRIX, 5.0f/360.f*pi<float>(), vec3(0.0f, 0.0f, 1.0f));
            groupShoulderMatrix = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupShoulderMatrix;
            
        } else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            float travelDistance = movementSpeed * dt;
            mat4 rotationTransformation = rotate(IDENTITY_MATRIX, -5.0f/360.f*pi<float>(), vec3(0.0f, 0.0f, 1.0f));
            groupShoulderMatrix =  movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupShoulderMatrix;
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            float travelDistance = movementSpeed * dt;
            mat4 rotationTransformation = rotate(IDENTITY_MATRIX, -5.0f/360.f*pi<float>(), vec3(0.0f, 1.0f, 0.0f));
            groupWristMatrix = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupWristMatrix;
            
        } else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
            float travelDistance = movementSpeed * dt;
            mat4 rotationTransformation = rotate(IDENTITY_MATRIX, 5.0f/360.f*pi<float>(), vec3(0.0f, 1.0f, 0.0f));
            groupWristMatrix = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupWristMatrix;
        }

        // Enable and Disable Texture
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && lastTstate == GLFW_RELEASE) 
        {
            if (enableTexture == 1) {
                setUseTexture(textureShaderProgram, 0);
                enableTexture = 0;
            }
            else if (enableTexture == 0) {
                setUseTexture(textureShaderProgram, 1);
                enableTexture = 1;
            }        
        }   
        lastTstate = glfwGetKey(window, GLFW_KEY_T);

        // if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        //     float travelDistance = movementSpeed * dt;
        //     mat4 rotationTransformation = rotate(IDENTITY_MATRIX, 5.0f/360.f*pi<float>(), vec3(0.0f, 0.0f, 1.0f));
        //     groupWristMatrix = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupWristMatrix;
            
        // } else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        //     float travelDistance = movementSpeed * dt;
        //     mat4 rotationTransformation = rotate(IDENTITY_MATRIX, -5.0f/360.f*pi<float>(), vec3(0.0f, 0.0f, 1.0f));
        //     groupWristMatrix =  movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * groupWristMatrix;
        // }


        // SET MODEL TO A RANDOM POSITION - spacebar
        if (spacebarLastState == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            double randomLocationX = returnRandomNumber();
            double randomLocationZ = returnRandomNumber();

            vec3 randomLocation(randomLocationX, 0.0f, randomLocationZ);
            groupShoulderMatrix = inverse(movementTranslationMatrix) * groupShoulderMatrix;
            movementTranslationMatrix = translate(IDENTITY_MATRIX, randomLocation);
            groupShoulderMatrix = movementTranslationMatrix * groupShoulderMatrix;
        }

        // ROTATE WORLD ORIENTATION - arrow keys & Home
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            viewRotation = rotate(IDENTITY_MATRIX, CAMERA_ANGLE_MODIFIER, vec3(1.f, 0.f, 0.f));
            viewMatrix = viewMatrix * viewRotation;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            viewRotation = rotate(IDENTITY_MATRIX, CAMERA_ANGLE_MODIFIER, vec3(0.f, 1.f, 0.f));
            viewMatrix = viewMatrix * viewRotation;
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            viewRotation = rotate(IDENTITY_MATRIX, -CAMERA_ANGLE_MODIFIER, vec3(1.f, 0.f, 0.f));
            viewMatrix = viewMatrix * viewRotation;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            viewRotation = rotate(IDENTITY_MATRIX, -CAMERA_ANGLE_MODIFIER, vec3(0.f, 1.f, 0.f));
            viewMatrix = viewMatrix * viewRotation;
        }

        // Set Model Back to Origin
        if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) {
            groupShoulderMatrix = inverse(movementTranslationMatrix) * groupShoulderMatrix;
            viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
            movementTranslationMatrix = mat4(1.0f);
        }

        // MOUSE MOVEMENTS
        // Right Click - mouse movement in x direction
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            cameraHorizontalAngle -= dx * CAMERA_ANGULAR_SPEED * dt;

            if (cameraHorizontalAngle > 360) {
                cameraHorizontalAngle -= 360;
            }
            else if (cameraHorizontalAngle < -360) {
                cameraHorizontalAngle += 360;
            }

            float newTheta = radians(cameraHorizontalAngle);
            float newPhi = radians(cameraVerticalAngle);

            cameraLookAt = vec3(cosf(newPhi)*cosf(newTheta), sinf(newPhi), -cosf(newPhi)*sinf(newTheta));
            viewMatrix = inverse(viewRotation) * inverse(currentWorldLookAt) * viewMatrix;
            currentWorldLookAt = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
            viewMatrix = currentWorldLookAt * viewRotation * viewMatrix;
        }

        // Middle mouse - mouse movement in y direction
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            cameraVerticalAngle -= dy * CAMERA_ANGULAR_SPEED * dt;

            if (cameraVerticalAngle  > 90) {
                cameraVerticalAngle  = 89;
            }
            else if (cameraVerticalAngle  < -90) {
                cameraVerticalAngle  = -89;
            }

            float newTheta = radians(cameraHorizontalAngle);
            float newPhi = radians(cameraVerticalAngle);

            cameraLookAt = vec3(cosf(newPhi)*cosf(newTheta), sinf(newPhi), -cosf(newPhi)*sinf(newTheta));
            viewMatrix = inverse(viewRotation) * inverse(currentWorldLookAt) * viewMatrix;
            currentWorldLookAt = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
            viewMatrix = currentWorldLookAt * viewRotation * viewMatrix;
        }

        // Left Click - zoom in and out
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            FOV += radians(dy * FOV_MODIFIER * dt);

            projectionMatrix = perspective(FOV, ASPECT_RATIO, 0.01f, 100.0f);
        }


        // Upon pressing escape, stop window, and terminate program later.
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        spacebarLastState = glfwGetKey(window, GLFW_KEY_SPACE);
    }

    // Terminate the program.
    glDeleteProgram(textureShaderProgram);
    glfwTerminate();
}