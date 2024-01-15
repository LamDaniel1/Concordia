/* COMP 371 - QUIZ 1
   Daniel Lam - 40248073
*/

#include <iostream>
#include<vector>

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
#define GRID_SLICES_X 78
#define GRID_SLICES_Z 36

#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const float RIGHT_ANGLE = pi<float>()/2;
const float CORNER_ANGLE = pi<float>()/4; // desired corner angle assuming starting from horizontal
const float RACKET_ANGLE = RIGHT_ANGLE - CORNER_ANGLE;
const float ARM_ANGLE = RIGHT_ANGLE - CORNER_ANGLE;
const mat4 IDENTITY_MATRIX(1.0f);
const mat4 TRANSLATE_ABOVE_RACKET = translate(IDENTITY_MATRIX, vec3(0.0f, 15.0f, 0.0f));

// COLORS
const vec4 colorLightRed = vec4(220/255.0f, 20/255.0f, 60/255.0f,1.0f);
const vec4 colorRed = vec4(1.0f, 0.0f, 0.0f,1.0f);
const vec4 colorDarkRed = vec4(139/255.0f, 0.0f, 0.0f,1.0f);
const vec4 colorLightYellow = vec4(1.0f, 1.0f, 153/255.0f,1.0f);
const vec4 colorYellow = vec4(1.0f, 1.0f, 0.0f,1.0f);
const vec4 colorDarkYellow = vec4(204/255.0f, 204/255.0f, 0.0f,1.0f);
const vec4 colorLightBlue = vec4(30.0/255.0f, 144.0/255.0f, 255.0/255.0f,1.0f);
const vec4 colorBlue = vec4(0.0f, 0.0f, 1.0f,1.0f);
const vec4 colorDarkBlue = vec4(0.0f, 0.0f, 139.0/255.0f,1.0f);
const vec4 colorLightGreen = vec4(124/255.0f, 252/255.0f, 0.0f,1.0f);
const vec4 colorGreen = vec4(0.0f, 1.0f, 0.0f,1.0f);
const vec4 colorDarkGreen = vec4(0.0f, 100.0/255.0f, 0.0f,1.0f);
const vec4 colorBeige = vec4(238.0/255.0f, 217.0/255.0f, 196.0/255.0f,1.0f);
const vec4 colorWhite = vec4(1.0f, 1.0f, 1.0f,1.0f);
const vec4 colorGray = vec4(80.0/255.0f, 80.0/255.0f, 80.0/255.0f,1.0f);
const vec4 colorSkybox = vec4(173/255.0f, 216/255.0f, 230/255.0f,1.0f);

const vec4 colorRedTransparent = vec4(1.0f, 0.0f, 0.0f,0.0f);
const vec4 colorBlueTransparent = vec4(0.0f, 0.0f, 1.0f,0.0f);
const vec4 colorGreenTransparent = vec4(0.0f, 1.0f, 0.0f,0.0f);
const vec4 colorYellowTransparent = vec4(1.0f, 1.0f, 0.0f,0.0f);



// Initialize Group Racket Matrices

// STRUCT
typedef struct racketMatrices {
    mat4 racketTranslation;
    mat4 racketRotation;
    mat4 groupMatrix;

    vec3 returnRacketCenter() {
        return racketTranslation * vec4(0.0f, 7.0f, 3.5f, 1.0f);
    }

    void updateGroupMatrix() {
        groupMatrix = racketTranslation * racketRotation;
    }

    // Constructor with default values
    racketMatrices() : racketTranslation(mat4(1.0f)), racketRotation(mat4(1.0f)), groupMatrix(mat4(1.0f)) {}
} racketMatrices;

racketMatrices groupFirstRacketDA;
racketMatrices groupSecondRacketNI;
racketMatrices* currentlySelectedGroupRacket = NULL;
int currentlySelectedGroupRacketInt = 0;

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

void computeGrid(GLuint & VAOGrid, GLuint & VBOGrid, GLuint & EBOGrid, GLuint & EBOGridLength) {
    
    vector<vec3> verticesGrid;
    vector<GLuint> indicesGrid;

    GLfloat gridMiddleX = GRID_SLICES_X/2.0f;
    GLfloat gridMiddleZ = GRID_SLICES_Z/2.0f;

    // Compute vertices on X axis
    for (int i = 0; i <= GRID_SLICES_Z; i++) {
        // set a pair of vertices to form a line, placing one on positive x and other on negative x.
        // Building lines from the negative z axis to the positive axis.
        verticesGrid.push_back(vec3(-gridMiddleX, 0.0f, -gridMiddleZ + i));
        verticesGrid.push_back(vec3(gridMiddleX, 0.0f, -gridMiddleZ + i));
    }

    for (int i = 0; i <= GRID_SLICES_X; i++) {
        // set a pair of vertices to form a line, placing one on positive x and other on negative x.
        // Building lines from the negative z axis to the positive axis.
        verticesGrid.push_back(vec3(-gridMiddleX + i, 0.0f, -gridMiddleZ));
        verticesGrid.push_back(vec3(-gridMiddleX + i, 0.0f, gridMiddleZ));
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


// drawCubeModel() sets up vertices & indices of base cube and allows rendering in Triangles with respective EBOGrid
void drawCubeModel(GLuint &VAOCubeModel, GLuint &VBOCubeModel, GLuint &EBOCubeModel, GLuint &EBOCubeModelLength) {
    
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
    
    EBOCubeModelLength = cubeIndices.size();
}

void drawRacket(mat4 groupRacketMatrix, GLuint colorLocation, GLuint modelMatrixLocation, GLuint EBOCubeModelLength, GLuint metallicTextureId, GLuint tattooTextureId) {

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

    // ----------- INITIAL TRANSFORMATIONS TO ARM MODEL -----------
    // --> Upper Arm
    mat4 upperArmScale = scale(IDENTITY_MATRIX, armDimensions);
    mat4 upperArmTranslation = translate(IDENTITY_MATRIX, translationAboveGrid);
    upperArmMatrix = upperArmScale * upperArmTranslation;

    // --> Fore Arm
    mat4 foreArmScale = scale(IDENTITY_MATRIX, armDimensions);
    mat4 foreArmTranslate = translate(IDENTITY_MATRIX, translationAboveGrid);
    foreArmMatrix = foreArmScale * foreArmTranslate;

    mat4 groupArmMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix;


    // // ARM - Groups of Shoulder, Elbow & Hand
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tattooTextureId);
    glUniform4fv(colorLocation, 1, &colorBeige[0]);
    mat4 groupShoulder = groupRacketMatrix * groupShoulderMatrix * upperArmMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupShoulder)[0][0]); // Drawing upper arm
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);
    mat4 groupElbow = groupRacketMatrix * groupShoulderMatrix * groupElbowMatrix * foreArmMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupElbow)[0][0]); // Drawing fore arm
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);


    // RACKET MATRIX TRANSFORMATIONS
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

    // Every Net Matrix
    
    // RACKET - Render entire model of racket
    // Racket Handle
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, metallicTextureId);
    glUniform4fv(colorLocation, 1, &colorRed[0]);
    mat4 groupRacketHandleModel =  groupRacketMatrix * groupArmMatrix * racketHandleAndSideDimensions;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketHandleModel)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Bottom Left Corner
    glUniform4fv(colorLocation, 1, &colorWhite[0]);
    mat4 groupRacketBottomLeftCornerMatrix = groupRacketMatrix * groupArmMatrix * racketBottomLeftCornerMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketBottomLeftCornerMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Bottom Right Corner
    mat4 groupRacketBottomRightCornerMatrix = groupRacketMatrix * groupArmMatrix  * racketBottomRightCornerMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketBottomRightCornerMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Bottom Side
    glUniform4fv(colorLocation, 1, &colorRed[0]);
    mat4 groupRacketBottomSideMatrix = groupRacketMatrix * groupArmMatrix * racketBottomSideMatrix;
    // mat4 groupRacketBottomSideMatrix = racketBottomSideMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketBottomSideMatrix)[0][0]);
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Left Side 
    mat4 groupRacketLeftSideMatrix = groupRacketMatrix * groupArmMatrix *  racketLeftSideMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketLeftSideMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Right Side
    mat4 groupRacketRightSideMatrix =groupRacketMatrix *  groupArmMatrix *  racketRightSideMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketRightSideMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Top Side
    mat4 groupRacketTopSideMatrix = groupRacketMatrix * groupArmMatrix *  racketTopSideMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketTopSideMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // RACKET NET
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

    while (netStringPosition.y < netHeight - 0.25f) {
        netMatrices.push_back(racketBottomSideTranslation * translate(IDENTITY_MATRIX, netStringPosition) * netStringMatrix);
        netStringPosition.y += netSpacing + horizontalNetDimensions.y;
    }

    // Loop through all of net matrices and draw
    for (int i = 0; i < netMatrices.size(); i++) {
        mat4 groupNetMatrix =groupRacketMatrix * groupArmMatrix *  netMatrices[i];
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &groupNetMatrix[0][0]);
        glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);
    }
}

// drawTennisNet() 
void drawTennisNet(GLuint colorLocation, GLuint modelMatrixLocation, GLuint EBOCubeModelLength, GLuint brickTextureId, GLuint glossyTextureId) {
    vector<mat4> netMatrices;
    const float netThickness = 0.2f;
    const float spacing = 0.5f;
    float positionZ = 0.0f;
    float positionY = 0.0f;

    mat4 translationAboveGrid = translate(IDENTITY_MATRIX, vec3(0.0f, 0.5f, 0.0f));

    vec3 verticalNet(netThickness, 5, netThickness);
    vec3 horizontalNet(netThickness, netThickness, GRID_SLICES_Z);

    mat4 verticalNetMatrix = scale(IDENTITY_MATRIX, verticalNet) * translationAboveGrid;
    mat4 horizontalNetMatrix = scale(IDENTITY_MATRIX, horizontalNet) * translationAboveGrid;

    // Add all matrix transformations for horizontal nets located on Z axis
    while (positionZ <= GRID_SLICES_Z/2.0f) {
        netMatrices.push_back(translate(IDENTITY_MATRIX, vec3(0.0f, 0.0f, positionZ)) * verticalNetMatrix);
        netMatrices.push_back(translate(IDENTITY_MATRIX, vec3(0.0f, 0.0f, -positionZ)) * verticalNetMatrix);
        positionZ += netThickness + spacing;
    }

    // Add all matrix transformations for vertical nets located on Z axis
    while (positionY <= verticalNet.y) {
        netMatrices.push_back(translate(IDENTITY_MATRIX, vec3(0.0f, positionY, 0.0f)) * horizontalNetMatrix);
        positionY += netThickness + spacing;

    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glossyTextureId);
    // Loop through every matrix and draw corresponding net
    for (int i = 0; i < netMatrices.size(); i++) {
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(netMatrices[i])[0][0]);
        glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);
    }

    // Render Net Pillars
    vec3 netPillarDimensions(0.5f, verticalNet.y, 0.5f);
    mat4 netPillarMatrix = scale(IDENTITY_MATRIX, netPillarDimensions) * translationAboveGrid;

    vec3 topRowNetDimensions(0.5f, 1.0f, GRID_SLICES_Z + 1);
    mat4 topRowNetMatrix = scale(IDENTITY_MATRIX, topRowNetDimensions) * translationAboveGrid;

    mat4 whiteTopRowNetMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, verticalNet.y, 0.0f)) * topRowNetMatrix;
    mat4 redPillarMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 0.0f, positionZ)) * netPillarMatrix;
    mat4 bluePillarMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 0.0f, -positionZ)) * netPillarMatrix;
    mat4 yellowPillarMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 0.0f, 0.0f)) * netPillarMatrix;

    // Change texture to brick first for pillars
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brickTextureId);

    glUniform4fv(colorLocation, 1, &colorRed[0]);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(redPillarMatrix)[0][0]);
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    glUniform4fv(colorLocation, 1, &colorBlue[0]);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(bluePillarMatrix)[0][0]);
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    glUniform4fv(colorLocation, 1, &colorYellow[0]);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(yellowPillarMatrix)[0][0]);
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Change texture of top row to glossy
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glossyTextureId);
    glUniform4fv(colorLocation, 1, &colorWhite[0]);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(whiteTopRowNetMatrix)[0][0]);
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);
}

// DRAW ALPHABETS
void drawDModel(mat4 groupRacketMatrix, GLuint colorLocation, GLuint modelMatrixLocation, GLuint EBOCubeModelLength) {
    const float dWidth = 2.0f;
    const float dHeight = 0.3f;
    const float dThickness = 0.3f;


    mat4 dBottomSideScale = scale(IDENTITY_MATRIX, vec3(dWidth, dHeight, dThickness));
    mat4 dLeftSideScale = scale(IDENTITY_MATRIX, vec3(dHeight, dWidth, dThickness));
    mat4 dLeftSideTranslate = translate(IDENTITY_MATRIX, vec3(-dWidth/2, 2.0f/2, 0.0f));
    mat4 dTopSideTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, 2.0f, 0.0f));
    mat4 dRightSideScale = scale(IDENTITY_MATRIX, vec3(dHeight, dWidth * 2, dThickness));
    mat4 dRightSideTranslate = translate(IDENTITY_MATRIX, vec3(dWidth/2, dWidth, 0.0f));

    mat4 dBottomSide = (groupRacketMatrix * TRANSLATE_ABOVE_RACKET ) * dBottomSideScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(dBottomSide)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 dTopSide = (groupRacketMatrix * TRANSLATE_ABOVE_RACKET * dTopSideTranslate) * dBottomSideScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(dTopSide)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 dLeftSide = (groupRacketMatrix * TRANSLATE_ABOVE_RACKET * dLeftSideTranslate) * dLeftSideScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(dLeftSide)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 dRightSide = (groupRacketMatrix * TRANSLATE_ABOVE_RACKET * dRightSideTranslate) * dRightSideScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(dRightSide)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

}

void drawAModel(mat4 groupRacketMatrix, GLuint colorLocation, GLuint modelMatrixLocation, GLuint EBOCubeModelLength) {
    const float aWidth = 2.0f;
    const float aHeight = 0.3f;
    const float aThickness = 0.3f;


    mat4 aBottomSideScale = scale(IDENTITY_MATRIX, vec3(aWidth, aHeight, aThickness));
    mat4 aLeftSideScale = scale(IDENTITY_MATRIX, vec3(aHeight, aWidth, aThickness));
    mat4 aLeftSideTranslate = translate(IDENTITY_MATRIX, vec3(-aWidth/2, 2.0f/2, 0.0f));
    mat4 aMiddleSideTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, 2.0f, 0.0f));
    mat4 aRightSideScale = scale(IDENTITY_MATRIX, vec3(aHeight, aWidth * 1.5, aThickness));
    mat4 aRightSideTranslate = translate(IDENTITY_MATRIX, vec3(aWidth/2, aWidth*0.67, 0.0f));
    mat4 aTopSideTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, aWidth * 1.5, 0.0f));

    mat4 aBottomSide = ( groupRacketMatrix * TRANSLATE_ABOVE_RACKET ) * aBottomSideScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(aBottomSide)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 aMiddleSide = (groupRacketMatrix * TRANSLATE_ABOVE_RACKET * aMiddleSideTranslate) * aBottomSideScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(aMiddleSide)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 aLeftSide = (groupRacketMatrix * TRANSLATE_ABOVE_RACKET * aLeftSideTranslate) * aLeftSideScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(aLeftSide)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 aRightSide = (groupRacketMatrix * TRANSLATE_ABOVE_RACKET * aRightSideTranslate) * aRightSideScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(aRightSide)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 aTopSide = (groupRacketMatrix * TRANSLATE_ABOVE_RACKET * aTopSideTranslate) * aBottomSideScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(aTopSide)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

}

void drawNModel(mat4 groupRacketMatrix, GLuint colorLocation, GLuint modelMatrixLocation, GLuint EBOCubeModelLength) {
    const float aWidth = 2.0f;
    const float aHeight = 0.3f;
    const float aThickness = 0.3f;

    mat4 NLeftSideScale = scale(IDENTITY_MATRIX, vec3(aHeight, aWidth, aThickness));
    mat4 NLeftSideTranslate = translate(IDENTITY_MATRIX, vec3(-aWidth/2, 0.0f, 0.0f));
    mat4 NTopSideScale = scale(IDENTITY_MATRIX, vec3(aWidth, aHeight, aThickness));
    mat4 NTopSideTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, aWidth/2, 0.0f));
    mat4 NRightSideTranslate = translate(IDENTITY_MATRIX, vec3(aWidth/2, 0.0f, 0.0f));

    mat4 nLeftSide = (groupRacketMatrix * TRANSLATE_ABOVE_RACKET * NLeftSideTranslate) * NLeftSideScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(nLeftSide)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 nTopSide = (groupRacketMatrix * TRANSLATE_ABOVE_RACKET  * NTopSideTranslate) * NTopSideScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(nTopSide)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 nRightSide = (groupRacketMatrix * TRANSLATE_ABOVE_RACKET  * NRightSideTranslate) * NLeftSideScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(nRightSide)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);


}

void drawIModel(mat4 groupRacketMatrix, GLuint colorLocation, GLuint modelMatrixLocation, GLuint EBOCubeModelLength) {
    const float iWidth = 2.0f;
    const float iHeight = 0.3f;
    const float iThickness = 0.3f;

    mat4 iBottomSideScale = scale(IDENTITY_MATRIX, vec3(iHeight, iWidth, iThickness));
    mat4 iDotScale = scale(IDENTITY_MATRIX, vec3(iHeight, iHeight, iHeight));
    mat4 iDotTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, iWidth + 1.0f, 0.0f));

    mat4 iBottomSide =  (groupRacketMatrix * TRANSLATE_ABOVE_RACKET) * iBottomSideScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(iBottomSide)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 iDot =  (groupRacketMatrix * TRANSLATE_ABOVE_RACKET * iDotTranslate) * iDotScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(iDot)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

}

// drawSkyBox() scales the skybox appropriately
void drawSkybox(GLuint colorLocation, GLuint modelMatrixLocation, GLuint EBOCubeModelLength) {
    const float modifier = 100.0f;

    mat4 skyboxScale = scale(IDENTITY_MATRIX, vec3(-GRID_SLICES_X, -modifier, -GRID_SLICES_Z * 2));

    mat4 skybox = skyboxScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(skybox)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

}

// drawFloor() sets up scale and translation for the floor cube
void drawFloor(GLuint colorLocation, GLuint modelMatrixLocation, GLuint EBOCubeModelLength) {
    mat4 floorScale = scale(IDENTITY_MATRIX, vec3(GRID_SLICES_X, 0.5f, GRID_SLICES_Z));
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

// setEnableTexture() updates enableTexture uniform to toggle on and off
void setEnableTexture(int shaderProgram, int status){
    glUseProgram(shaderProgram);
    GLuint enableTextureLocation = glGetUniformLocation(shaderProgram, "enableTexture");
    glUniform1i(enableTextureLocation, status);
}

// setSpotLight() updates the uniform variables of spotLight
void setSpotLight(int shaderProgram, vec3 position, float intensity, vec3 LookAtVector)
{
    glUseProgram(shaderProgram);

    GLuint spotLightPositionLocation = glGetUniformLocation(shaderProgram, "spotLPos");
    glUniform3fv(spotLightPositionLocation, 1, &position[0]);

    GLuint spotLightIntensityLocation = glGetUniformLocation(shaderProgram, "spotLIntensity");
    glUniform1f(spotLightIntensityLocation, intensity);

    GLuint spotLightLookAtLocation = glGetUniformLocation(shaderProgram, "spotLightLookAt");
    glUniform3fv(spotLightLookAtLocation, 1, &LookAtVector[0]);
}


int main(int argc, char * argv[]) {

    // -------------- 1. INITIALIZE GLFW AND SHADERS ---------------

    // Init the glfw library and set various configurations.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
 
    // Enables Transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create a glfw window with specified size
    // glfwCreateWindow(width, size, title, monitor, share)
    GLFWwindow * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "COMP 371 - Quiz 2", NULL, NULL);
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

     // -------------------- TEXTURE SHADER - CHANGE SO THAT IT ONLY USES THIS ---------------

    GLuint textureShaderProgram = shader("./Shaders/textureVertexShaderSource.glsl", "./Shaders/textureFragmentShaderSource.glsl");

    // ---------------------- END --------------------------

    // Assign OpenGL to use shaderProgram that we've set
    glUseProgram(textureShaderProgram);

    // Initialize uniform for color
    GLuint colorLocation = glGetUniformLocation(textureShaderProgram, "color");

    // Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Background color
    glClearColor(57/255.0f, 62/255.0f, 67/255.0f, 1.0f);

    // Set the viewport to avoid the jittering in the beginning.
    GLint frameBufferWidth, frameBufferHeight;
    glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
    glViewport(0, 0, frameBufferWidth, frameBufferHeight);

    // -------------- 2. INITIALIZE VAOs ---------------

    // VAOGrid for grid
    GLuint VAOGrid, VBOGrid, EBOGrid, EBOGridLength;
    computeGrid(VAOGrid, VBOGrid, EBOGrid, EBOGridLength);

    // VAO for cube
    GLuint VAOCubeModel, VBOCubeModel, EBOCubeModel, EBOCubeModelLength;
    drawCubeModel(VAOCubeModel, VBOCubeModel, EBOCubeModel, EBOCubeModelLength);

    // VAO for net
    GLuint VAONetModel, VBONetModel, EBONetModel, EBONetModelLength;
    drawCubeModel(VAONetModel, VBONetModel, EBONetModel, EBONetModelLength);

    // VAO for d
    GLuint VAODModel, VBODModel, EBODModel, EBODModelLength;
    drawCubeModel(VAODModel, VBODModel, EBODModel, EBODModelLength);

    // VAO for a
    GLuint VAOAModel, VBOAModel, EBOAModel, EBOAModelLength;
    drawCubeModel(VAOAModel, VBOAModel, EBOAModel, EBOAModelLength);

    // VAO for n
    GLuint VAONModel, VBONModel, EBONModel, EBONModelLength;
    drawCubeModel(VAONModel, VBONModel, EBONModel, EBONModelLength);

    // VAO for I
    GLuint VAOIModel, VBOIModel, EBOIModel, EBOIModelLength;
    drawCubeModel(VAOIModel, VBOIModel, EBOIModel, EBOIModelLength);

    GLuint VAOSkybox, VBOSkybox, EBOSkybox, EBOSkyboxLength;
    drawCubeModel(VAOSkybox, VBOSkybox, EBOSkybox, EBOSkyboxLength);

    // unbind VAOGrid, VBOGrid and EBOGrid for clean up
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // ------------------------- 2.5 TEXTURES -----------------------

    GLuint metallicTextureId = loadTexture("./textures/metal.jpg");
    GLuint clayTextureId = loadTexture("./textures/clay.jpg");
    GLuint glossyTextureId = loadTexture("./textures/glossy.jpg");
    GLuint brickTextureId = loadTexture("./textures/brick.jpg");
    GLuint woodTextureId = loadTexture("./textures/wood.jpg");
    GLuint skyTextureId = loadTexture("./textures/sky.jpg");
    GLuint stoneTextureId = loadTexture("./textures/stone.jpg");
    GLuint sesameTextureId = loadTexture("./textures/sesame.jpg");
    GLuint paperTextureId = loadTexture("./textures/paper.jpg");
    GLuint floorTextureId = loadTexture("./textures/floor.jpg");
    GLuint tattooTextureId = loadTexture("./textures/tattoo.jpg");

    // --------------- 3. INITIALIZE MATRICES AND RENDER -----------

    // Initialize MVP matrices of the entire world to identity matrices
    mat4 modelMatrix(1.0f);
    mat4 viewMatrix(1.0f);
    mat4 projectionMatrix(1.0f);

    // --> RACKET MATRIX TRANSFORMATIONS
    GLfloat movementSpeed = 1.0f;
    mat4 movementTranslationMatrix(1.0f);
    mat4 movementScaleMatrix(1.0f);

    // Set up group rackets
    mat4 groupFirstRacketDATranslate = translate(IDENTITY_MATRIX, vec3(-GRID_SLICES_X/4.0f, 0.0f, 0.0f)); // located upper left of grid
    mat4 groupSecondRacketNITranslate = translate(IDENTITY_MATRIX, vec3(GRID_SLICES_X/4.0f, 0.0f, 0.0f)); // located bottom left of grid
    
    // Initial Translation
    groupFirstRacketDA.racketTranslation = groupFirstRacketDATranslate;
    groupSecondRacketNI.racketTranslation = groupSecondRacketNITranslate;
    
    groupFirstRacketDA.updateGroupMatrix();
    groupSecondRacketNI.updateGroupMatrix();

    // MATRIX TRANSLATIONS FOR ALPHABET
    mat4 leftLetterTranslate = translate(IDENTITY_MATRIX, vec3(-1.3f, 0.0f, 0.0f));
    mat4 rightLetterTranslate = translate(IDENTITY_MATRIX, vec3(1.3f, 0.0f, 0.0f));

    // ---------------- 4. CAMERA ------------------

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
    currentlySelectedGroupRacket = NULL;
    mat4 initialWorldLookAt = lookAt(vec3(0.0f, 10.0f, GRID_SLICES_Z/2), vec3(0.0f, 5.0f, -30.0f), vec3(0.0f, 1.0f, 0.0f));
    // mat4 currentWorldLookAt = viewRotation * initialWorldLookAt;
    // viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

    // ----------------- 5. SPOTLIGHT -------------------
    vec3 lightPos(5.0f, 40.0f, 0.0f);
    vec3 spotLightPos(0.0f, 20.0f, 30.0f);
    float spotLightIntensity = 1.0f;

    GLuint lightLocation = glGetUniformLocation(textureShaderProgram, "lightPos");
    glUniform3fv(lightLocation, 1, &spotLightPos[0]);

    // Inputs
    GLfloat lastFrameTime = glfwGetTime();
    GLuint spacebarLastState = glfwGetKey(window, GLFW_KEY_SPACE);

    int enableTexture = 0;
    int lastTstate = GLFW_RELEASE;
    int lastMstate = GLFW_RELEASE;
    int lastRstate = GLFW_RELEASE;
    int lastLstate = GLFW_RELEASE;


    while(!glfwWindowShouldClose(window)) {

        // TIMER CODE
        // dt = delta time
        GLfloat dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;



        // clear current buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // UPDATE SPOT LIGHT ROTATION
        vec4 spotLightPos4 = vec4(spotLightPos, 1.0f);

        vec4 spotLightRotation = rotate(IDENTITY_MATRIX, radians(1.0f), vec3(0.0f, 1.0f, 0.0f)) * spotLightPos4;
        spotLightPos = vec3(spotLightRotation);
        
        GLuint lightLocation = glGetUniformLocation(textureShaderProgram, "spotLPos");
        glUniform3fv(lightLocation, 1, &spotLightPos[0]);

        
        // MVP
        GLuint modelMatrixLocation = glGetUniformLocation(textureShaderProgram, "model");
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        // Set LookAt at ViewMatrix to see corner of cube (default camera position)
        GLuint viewMatrixLocation = glGetUniformLocation(textureShaderProgram, "view");
        glUniformMatrix4fv(viewMatrixLocation, 1 , GL_FALSE, &viewMatrix[0][0]);

        // Set Projection Matrix to perspective view as default
        projectionMatrix = perspective(radians(100.0f), ASPECT_RATIO, 0.01f, 1000.0f);
        GLuint projectionMatrixLocation = glGetUniformLocation(textureShaderProgram, "proj");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
        

        // Spotlight
        setSpotLight(textureShaderProgram, spotLightPos, spotLightIntensity, vec3(0.0f, 0.0f, 0.0f));

        // ACTIVATE TEXTURES
        glActiveTexture(GL_TEXTURE0);
        GLuint textureLocation = glGetUniformLocation(textureShaderProgram, "textureSampler");
        glUniform1i(textureLocation, 0);

        // draw Skybox
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skyTextureId);
        glUniform4fv(colorLocation, 1, &colorSkybox[0]);
        glBindVertexArray(VAOSkybox);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOSkybox);
        drawSkybox( colorLocation,  modelMatrixLocation,  EBOCubeModelLength);

        // CUBE - Render the base 3D Cube
        glBindVertexArray(VAOCubeModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOCubeModel);

        // FLOOR - Render floor below grid
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTextureId);
        glUniform4fv(colorLocation, 1, &colorGreen[0]);
        drawFloor(colorLocation, modelMatrixLocation, EBOCubeModelLength); // draw floor as tennis

        
        // RACKETS - Render the 4 rackets
        // Change texture to metal for rackets
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, metallicTextureId);

        glUniform4fv(colorLocation, 1, &colorRed[0]);
        drawRacket(groupFirstRacketDA.groupMatrix, colorLocation, modelMatrixLocation, EBOCubeModelLength, metallicTextureId, tattooTextureId); // racket DA
        drawRacket(groupSecondRacketNI.groupMatrix, colorLocation, modelMatrixLocation, EBOCubeModelLength, metallicTextureId, tattooTextureId); // racket NI 
        
        // GRID - Render the X,Z plane grid
        glUniform4fv(colorLocation, 1, &colorYellow[0]);
        glBindVertexArray(VAOGrid);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOGrid);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
        glDrawElements(GL_LINES, EBOGridLength, GL_UNSIGNED_INT, 0);

        // NET - Render the Tennis Net
        glUniform4fv(colorLocation, 1, &colorGray[0]);
        glBindVertexArray(VAONetModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBONetModel);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
        drawTennisNet(colorLocation, modelMatrixLocation, EBOCubeModelLength, brickTextureId, glossyTextureId);


        // ALPHABET - render d a n i
        // Letter D 
        glUniform4fv(colorLocation, 1, &colorBlueTransparent[0]);
        glBindVertexArray(VAODModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBODModel);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupFirstRacketDA.groupMatrix)[0][0]);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, brickTextureId);
        drawDModel((groupFirstRacketDA.groupMatrix * leftLetterTranslate), colorLocation, modelMatrixLocation, EBOCubeModelLength);

        
        // Letter A
        glUniform4fv(colorLocation, 1, &colorRedTransparent[0]);
        glBindVertexArray(VAOAModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOAModel);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupFirstRacketDA.groupMatrix)[0][0]);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTextureId);
        drawAModel((groupFirstRacketDA.groupMatrix * rightLetterTranslate), colorLocation, modelMatrixLocation, EBOCubeModelLength);

        
        // Letter N
        glUniform4fv(colorLocation, 1, &colorGreenTransparent[0]);
        glBindVertexArray(VAONModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOAModel);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupSecondRacketNI.groupMatrix)[0][0]);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sesameTextureId);
        drawNModel((groupSecondRacketNI.groupMatrix * leftLetterTranslate), colorLocation, modelMatrixLocation, EBOCubeModelLength);

        
        // Letter I
        glUniform4fv(colorLocation, 1, &colorYellowTransparent[0]);
        glBindVertexArray(VAOIModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOIModel);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupSecondRacketNI.groupMatrix)[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, paperTextureId);
        drawIModel((groupSecondRacketNI.groupMatrix * rightLetterTranslate), colorLocation, modelMatrixLocation, EBOCubeModelLength);

        

        // swap the buffer to the one already written, and place the current one as a canvas
        glfwSwapBuffers(window);
        glfwPollEvents();

        bool isShiftKeyPressed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

         // ------- INPUT EVENTS ----------
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            // viewMatrix = mat4(1.0f);
            viewMatrix = lookAt(vec3(0.0f, 10.0f, 10.0f), vec3(0.0f, 5.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            viewMatrix = lookAt(vec3(1.5f, 10.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            viewMatrix = lookAt(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
            viewMatrix = lookAt(vec3(0.0f, 4.0f, 7.0f), vec3(0.0f, 3.0f, -4.0f), vec3(0.0f, 0.0f, -1.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
            viewMatrix = lookAt(vec3(0.0f, 10.0f, 25.0f), vec3(0.0f, 9.0f, -4.0f), vec3(0.0f, 0.0f, -1.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
            viewMatrix = lookAt(vec3(0.0f, 2.0f, 4.0f), vec3(0.0f, 2.0f, -4.0f), vec3(0.0f, 1.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
            viewMatrix = lookAt(vec3(25.0f, 15.0f, 50.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) {
            viewMatrix = lookAt(vec3(0.0f, 50.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
            viewMatrix = lookAt(vec3(15.0f, 0.5f, 0.0f), vec3(0.0f, 0.5f, 0.0f), vec3(0.0f,  1.0f, 0.0f));
        }
        if (glfwGetKey(window, GLFW_KEY_SLASH) == GLFW_PRESS) {
            viewMatrix = lookAt(vec3(110.0f, 110.0f, 110.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  1.0f, 0.0f));
            
        }

        // SELECT RACKET
        // if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        //     currentlySelectedGroupRacket = &groupFirstRacketDA;
        //     // viewMatrix = lookAt(currentlySelectedGroupRacket->racketTranslation * vec4(0.0f, 7.0f, 0.0f, 1.0f), vec3(-GRID_SLICES_X/4.0f, 7.0f, -50.0f), vec3(0.0f,  1.0f, 0.0f));

        // }
        // if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        //     currentlySelectedGroupRacket = &groupSecondRacketNI;
        //     // viewMatrix = lookAt(vec3(-GRID_SLICES_X/4.0f, 7.0f, GRID_SLICES_Z/2), vec3(-GRID_SLICES_X/4.0f, 7.0f, -50.0f), vec3(0.0f,  1.0f, 0.0f));
            
        // }

        // Enable and Disable Texture
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && lastTstate == GLFW_RELEASE) 
        {
            if (enableTexture == 1) {
                setEnableTexture(textureShaderProgram, 0);
                enableTexture = 0;
            }
            else if (enableTexture == 0) {
                setEnableTexture(textureShaderProgram, 1);
                enableTexture = 1;
            }        
        }   
        lastTstate = glfwGetKey(window, GLFW_KEY_T);

        // Enable and Disable Spotlight
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && lastLstate == GLFW_RELEASE) 
        {
            if (spotLightIntensity > 0) {
                spotLightIntensity = 0;
            } else if (spotLightIntensity == 0) {
                spotLightIntensity = 1.0f;
            }
        }   
        lastLstate = glfwGetKey(window, GLFW_KEY_L);

        // CONTROL MODEL POSITION AND ORIENTATION - WASD keys
        if (currentlySelectedGroupRacket != NULL && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && isShiftKeyPressed) {
            float travelDistance = movementSpeed * dt;
            mat4 distanceTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, travelDistance, 0.0f) );
            currentlySelectedGroupRacket->racketTranslation = distanceTranslate * currentlySelectedGroupRacket->racketTranslation;
            currentlySelectedGroupRacket->updateGroupMatrix();
        }
        
        if (currentlySelectedGroupRacket != NULL && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && isShiftKeyPressed) {

            float travelDistance = movementSpeed * dt;
            mat4 distanceTranslate = translate(IDENTITY_MATRIX, vec3(-travelDistance, 0.0f, 0.0f) );
            currentlySelectedGroupRacket->racketTranslation = distanceTranslate * currentlySelectedGroupRacket->racketTranslation;
            currentlySelectedGroupRacket->updateGroupMatrix();
            

        } else if (currentlySelectedGroupRacket != NULL && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            mat4 rotationTransformation = rotate(mat4(1.0f), 5.f/360.f*pi<float>(), vec3(0.f, 1.f, 0.f));
            currentlySelectedGroupRacket->racketRotation = rotationTransformation * currentlySelectedGroupRacket->racketRotation;
            currentlySelectedGroupRacket->updateGroupMatrix();
        }

        if (currentlySelectedGroupRacket != NULL && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && isShiftKeyPressed) {
            float travelDistance = movementSpeed * dt;
            mat4 distanceTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, -travelDistance, 0.0f) );
            currentlySelectedGroupRacket->racketTranslation = distanceTranslate * currentlySelectedGroupRacket->racketTranslation;
            currentlySelectedGroupRacket->updateGroupMatrix();

        }

        if (currentlySelectedGroupRacket != NULL && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && isShiftKeyPressed) {
            float travelDistance = movementSpeed * dt;
            mat4 distanceTranslate = translate(IDENTITY_MATRIX, vec3(travelDistance, 0.0f, 0.0f) );
            currentlySelectedGroupRacket->racketTranslation = distanceTranslate * currentlySelectedGroupRacket->racketTranslation;
            currentlySelectedGroupRacket->updateGroupMatrix();
            
        } else if (currentlySelectedGroupRacket != NULL && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            mat4 rotationTransformation = rotate(IDENTITY_MATRIX, -5.0f/360.f*pi<float>(), vec3(0.0f, 1.0f, 0.0f));
            currentlySelectedGroupRacket->racketRotation = rotationTransformation * currentlySelectedGroupRacket->racketRotation;
            currentlySelectedGroupRacket->updateGroupMatrix();
        }

        // Keybind M - Cycle through currentlySelectedRacket
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && lastMstate == GLFW_RELEASE) {
            if (currentlySelectedGroupRacketInt == 1) {
                currentlySelectedGroupRacket = &groupFirstRacketDA;
            } else if (currentlySelectedGroupRacketInt == 2) {
                currentlySelectedGroupRacket = &groupSecondRacketNI;
            } else {
                currentlySelectedGroupRacket = NULL;
            }
            currentlySelectedGroupRacketInt = (currentlySelectedGroupRacketInt + 1) % 3;

        }
        lastMstate = glfwGetKey(window, GLFW_KEY_M);

        // Keybind R - Reset camera back to main camera
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && lastRstate == GLFW_RELEASE) {
            currentlySelectedGroupRacket = NULL;
            currentlySelectedGroupRacketInt = 0;
        }
        lastRstate = glfwGetKey(window, GLFW_KEY_R);

        // Setting Camera to selected racket
        if (currentlySelectedGroupRacket != NULL) {
            viewMatrix = lookAt(currentlySelectedGroupRacket->returnRacketCenter() + vec3(0.0f, 0.0f, GRID_SLICES_Z/4), currentlySelectedGroupRacket->returnRacketCenter(), vec3(0.0f,  1.0f, 0.0f));
        } else {
            viewMatrix = initialWorldLookAt;
        }

        // Upon pressing escape, stop window, and terminate program later.
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

    }

    // Terminate the program.
    glDeleteProgram(textureShaderProgram);
    glfwTerminate();
}