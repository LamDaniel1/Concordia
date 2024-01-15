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
using namespace std;
using namespace glm;

// GLOBAL VARIABLES
#define INFO_LOG_SIZE 512
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define ASPECT_RATIO ((float)WINDOW_WIDTH/(float)WINDOW_HEIGHT)
#define GRID_SLICES_X 78
#define GRID_SLICES_Z 36
const float RIGHT_ANGLE = pi<float>()/2;
const float CORNER_ANGLE = pi<float>()/4; // desired corner angle assuming starting from horizontal
const float RACKET_ANGLE = RIGHT_ANGLE - CORNER_ANGLE;
const mat4 IDENTITY_MATRIX(1.0f);
const mat4 TRANSLATE_ABOVE_RACKET = translate(IDENTITY_MATRIX, vec3(0.0f, 10.0f, 0.0f));

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

// Initialize Group Racket Matrices

// STRUCT
typedef struct racketMatrices {
    mat4 racketTranslation;
    mat4 racketRotation;
    mat4 groupMatrix;

    vec3 returnRacketCenter() {
        return racketTranslation * vec4(0.0f, 7.0f, 0.0f, 1.0f);
    }

    void updateGroupMatrix() {
        groupMatrix = racketTranslation * racketRotation;
    }

    // Constructor with default values
    racketMatrices() : racketTranslation(mat4(1.0f)), racketRotation(mat4(1.0f)), groupMatrix(mat4(1.0f)) {}
} racketMatrices;

racketMatrices groupFirstRacketD;
racketMatrices groupSecondRacketA;
racketMatrices groupThirdRacketN;
racketMatrices groupFourthRacketI;
racketMatrices* currentlySelectedGroupRacket = NULL;


// SHADER SOURCES - VERTEX AND FRAGMENT
const char * vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aCoord;

    uniform mat4 model = mat4(1.0);
    uniform mat4 view = mat4(1.0);
    uniform mat4 proj = mat4(1.0);

    void main(){
        gl_Position = proj * view * model * vec4(aPos, 1.0);
    }
)";

const char * fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor; 

    uniform vec3 color = vec3(1.0, 1.0, 1.0);

    void main() {
        FragColor = vec4(color, 1.0);
    }
)";

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
        vec3(-0.5f, -0.5f, 0.5f),
        vec3(-0.5f, 0.5f, 0.5f),
        vec3(0.5f, 0.5f, 0.5f),
        vec3(0.5f, -0.5f, 0.5f),
        
        // back face
        vec3(-0.5f, -0.5f, -0.5f),
        vec3(-0.5f, 0.5f, -0.5f),
        vec3(0.5f, 0.5f, -0.5f),
        vec3(0.5f, -0.5f, -0.5f),
    }; 

    // INDICES OF 3D CUBE USING GL_TRIANGLES
    std::vector<GLuint> cubeIndices = {
        // front face - CCW
        2, 1, 0,
        0, 3, 2,

        // back face - CW
        5, 6, 7,
        7, 4, 5,

        //right side face
        2, 3, 7,
        7, 6, 2,

        //left side face - CW
        1, 5, 4,
        4, 0, 1,

        //top
        5, 1, 2,
        2, 6, 5,

        // bottom - CW
        4, 7, 3,
        3, 0, 4
    };

    // Set up VAOGrid of Cube
    glGenVertexArrays(1, &VAOCubeModel);
    glBindVertexArray(VAOCubeModel);
    
    // Set up VBOGrid of Cube
    glGenBuffers(1, &VBOCubeModel);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCubeModel);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size()*sizeof(vec3), &cubeVertices[0][0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Set up EBOGrid of 3D Cube Model - Triangles
    glGenBuffers(1, &EBOCubeModel);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOCubeModel);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size()*sizeof(GLuint), &cubeIndices[0], GL_STATIC_DRAW);
    
    EBOCubeModelLength = cubeIndices.size();
}

void drawRacket(mat4 groupRacketMatrix, GLuint colorLocation, GLuint modelMatrixLocation, GLuint EBOCubeModelLength) {


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
    vec3 translationAboveGrid = vec3(0.0f, 0.5f, 0.0f);

    // Net Components
    mat4 racketNetVerticalDimensions(1.0f);
    mat4 racketNet1VerticalMatrix(1.0f);
    mat4 racketNet2VerticalMatrix(1.0f);
    mat4 racketNet3VerticalMatrix(1.0f);
    mat4 racketNet4VerticalMatrix(1.0f);
    mat4 racketNet5VerticalMatrix(1.0f);
    mat4 racketNet6VerticalMatrix(1.0f);
    mat4 racketNet7VerticalMatrix(1.0f);
    mat4 racketNet8VerticalMatrix(1.0f);
    mat4 racketNet9VerticalMatrix(1.0f);

    mat4 racketNetHorizontalDimensions(1.0f);
    mat4 racketNet1HorizontalMatrix(1.0f);
    mat4 racketNet2HorizontalMatrix(1.0f);
    mat4 racketNet3HorizontalMatrix(1.0f);
    mat4 racketNet4HorizontalMatrix(1.0f);
    mat4 racketNet5HorizontalMatrix(1.0f);
    mat4 racketNet6HorizontalMatrix(1.0f);
    mat4 racketNet7HorizontalMatrix(1.0f);
    mat4 racketNet8HorizontalMatrix(1.0f);
    mat4 racketNet9HorizontalMatrix(1.0f);
    mat4 racketNet10HorizontalMatrix(1.0f);
    mat4 racketNet11HorizontalMatrix(1.0f);
    mat4 racketNet12HorizontalMatrix(1.0f);
    mat4 racketNet13HorizontalMatrix(1.0f);
    mat4 racketNet14HorizontalMatrix(1.0f);

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

    // Racket Net Transformations
    mat4 racketNetVerticalScale = scale(IDENTITY_MATRIX, vec3(racketNetThickness, racketLength, racketNetThickness));
    mat4 racketNet1VerticalTranslate = translate(IDENTITY_MATRIX, vec3(-racketNetDeltaX * 4, racketLength + racketCornerUnitY - offsetY, 0.0f));
    mat4 racketNet2VerticalTranslate = translate(IDENTITY_MATRIX, vec3(-racketNetDeltaX * 3, racketLength + racketCornerUnitY - offsetY, 0.0f));
    mat4 racketNet3VerticalTranslate = translate(IDENTITY_MATRIX, vec3(-racketNetDeltaX * 2, racketLength + racketCornerUnitY - offsetY, 0.0f));
    mat4 racketNet4VerticalTranslate = translate(IDENTITY_MATRIX, vec3(-racketNetDeltaX * 1, racketLength + racketCornerUnitY - offsetY, 0.0f));
    mat4 racketNet5VerticalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY - offsetY, 0.0f));
    mat4 racketNet6VerticalTranslate = translate(IDENTITY_MATRIX, vec3(racketNetDeltaX * 1, racketLength + racketCornerUnitY - offsetY, 0.0f));
    mat4 racketNet7VerticalTranslate = translate(IDENTITY_MATRIX, vec3(racketNetDeltaX * 2, racketLength + racketCornerUnitY - offsetY, 0.0f));
    mat4 racketNet8VerticalTranslate = translate(IDENTITY_MATRIX, vec3(racketNetDeltaX * 3, racketLength + racketCornerUnitY - offsetY, 0.0f));
    mat4 racketNet9VerticalTranslate = translate(IDENTITY_MATRIX, vec3(racketNetDeltaX * 4, racketLength + racketCornerUnitY - offsetY, 0.0f));

    mat4 racketNetHorizontalScale = scale(IDENTITY_MATRIX, vec3(racketCornerUnitX * 2, racketNetThickness, racketNetThickness));
    mat4 racketNet1HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 + racketNetDeltaY * 1, 0.0f)); // starting from middle of net
    mat4 racketNet2HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 + racketNetDeltaY * 2, 0.0f));
    mat4 racketNet3HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 + racketNetDeltaY * 3, 0.0f));
    mat4 racketNet4HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 + racketNetDeltaY * 4, 0.0f));
    mat4 racketNet5HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 + racketNetDeltaY * 5, 0.0f));
    mat4 racketNet6HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 + racketNetDeltaY * 6 , 0.0f));
    mat4 racketNet7HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 + racketNetDeltaY * 7, 0.0f));
    mat4 racketNet8HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 + 0.0f , 0.0f));
    mat4 racketNet9HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 - racketNetDeltaY * 1, 0.0f));
    mat4 racketNet10HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 - racketNetDeltaY * 2, 0.0f));
    mat4 racketNet11HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 - racketNetDeltaY * 3, 0.0f));
    mat4 racketNet12HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 - racketNetDeltaY * 4, 0.0f));
    mat4 racketNet13HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 - racketNetDeltaY * 5, 0.0f));
    mat4 racketNet14HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 - racketNetDeltaY * 6, 0.0f));

    // Calculate matrices transformations for each component of the racket
    racketHandleAndSideDimensions = racketHandleAndSideScale * racketTranslateAboveGrid;
    
    racketCornerMatrix = racketCornerScale * racketTranslateAboveGrid;
    racketBottomLeftCornerMatrix =  racketCornerTranslation * racketRotation * racketCornerMatrix;
    racketBottomRightCornerMatrix = racketCornerTranslation * inverse(racketRotation) * racketCornerMatrix;
    racketLeftSideMatrix = racketLeftSideTranslate * racketHandleAndSideDimensions;
    racketRightSideMatrix = racketRightSideTranslate * racketHandleAndSideDimensions;
    racketBottomSideMatrix = racketBottomSideTranslation * racketTopAndBottomSideScale;
    racketTopSideMatrix = racketTopSideTranslation * racketTopAndBottomSideScale;

    // Dimensions of Net
    racketNetVerticalDimensions = racketNetVerticalScale * racketTranslateAboveGrid;
    racketNetHorizontalDimensions = racketNetHorizontalScale * racketTranslateAboveGrid;
    
    // Every Net Matrix
    racketNet1VerticalMatrix = racketNet1VerticalTranslate * racketNetVerticalDimensions;
    racketNet2VerticalMatrix = racketNet2VerticalTranslate * racketNetVerticalDimensions;
    racketNet3VerticalMatrix = racketNet3VerticalTranslate * racketNetVerticalDimensions;
    racketNet4VerticalMatrix = racketNet4VerticalTranslate * racketNetVerticalDimensions;
    racketNet5VerticalMatrix = racketNet5VerticalTranslate * racketNetVerticalDimensions;
    racketNet6VerticalMatrix = racketNet6VerticalTranslate * racketNetVerticalDimensions;
    racketNet7VerticalMatrix = racketNet7VerticalTranslate * racketNetVerticalDimensions;
    racketNet8VerticalMatrix = racketNet8VerticalTranslate * racketNetVerticalDimensions;
    racketNet9VerticalMatrix = racketNet9VerticalTranslate * racketNetVerticalDimensions;


    racketNet1HorizontalMatrix = racketNet1HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet2HorizontalMatrix = racketNet2HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet3HorizontalMatrix = racketNet3HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet4HorizontalMatrix = racketNet4HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet5HorizontalMatrix = racketNet5HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet6HorizontalMatrix = racketNet6HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet7HorizontalMatrix = racketNet7HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet8HorizontalMatrix = racketNet8HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet9HorizontalMatrix = racketNet9HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet10HorizontalMatrix = racketNet10HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet11HorizontalMatrix = racketNet11HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet12HorizontalMatrix = racketNet12HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet13HorizontalMatrix = racketNet13HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet14HorizontalMatrix = racketNet14HorizontalTranslate * racketNetHorizontalDimensions;

    // RACKET - Render entire model of racket
    // Racket Handle
    glUniform4fv(colorLocation, 1, &colorRed[0]);
    mat4 groupRacketHandleModel = groupRacketMatrix * racketHandleAndSideDimensions;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketHandleModel)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Bottom Left Corner
    glUniform4fv(colorLocation, 1, &colorWhite[0]);
    mat4 groupRacketBottomLeftCornerMatrix =  groupRacketMatrix * racketBottomLeftCornerMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketBottomLeftCornerMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Bottom Right Corner
    mat4 groupRacketBottomRightCornerMatrix =  groupRacketMatrix * racketBottomRightCornerMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketBottomRightCornerMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Bottom Side
    glUniform4fv(colorLocation, 1, &colorRed[0]);
    mat4 groupRacketBottomSideMatrix =  groupRacketMatrix * racketBottomSideMatrix;
    // mat4 groupRacketBottomSideMatrix = racketBottomSideMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketBottomSideMatrix)[0][0]);
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Left Side 
    mat4 groupRacketLeftSideMatrix =  groupRacketMatrix * racketLeftSideMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketLeftSideMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Right Side
    mat4 groupRacketRightSideMatrix =  groupRacketMatrix * racketRightSideMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketRightSideMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Top Side
    mat4 groupRacketTopSideMatrix =  groupRacketMatrix * racketTopSideMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketTopSideMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Racket Net - PLEASE REWRITE CODE USING FOR LOOP
    glUniform4fv(colorLocation, 1, &colorGreen[0]);

    // Vertical nets
    mat4 groupRacketNet1VerticalMatrix =  groupRacketMatrix * racketNet1VerticalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet1VerticalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet2VerticalMatrix =  groupRacketMatrix * racketNet2VerticalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet2VerticalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet3VerticalMatrix =  groupRacketMatrix * racketNet3VerticalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet3VerticalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);
    
    mat4 groupRacketNet4VerticalMatrix =  groupRacketMatrix * racketNet4VerticalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet4VerticalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet5VerticalMatrix =  groupRacketMatrix * racketNet5VerticalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet5VerticalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet6VerticalMatrix = groupRacketMatrix *  racketNet6VerticalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet6VerticalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet7VerticalMatrix = groupRacketMatrix *  racketNet7VerticalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet7VerticalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet8VerticalMatrix = groupRacketMatrix *  racketNet8VerticalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet8VerticalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet9VerticalMatrix =  groupRacketMatrix * racketNet9VerticalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet9VerticalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    // Horizontal Nets
    mat4 groupRacketNet1HorizontalMatrix =  groupRacketMatrix * racketNet1HorizontalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet1HorizontalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet2HorizontalMatrix =  groupRacketMatrix * racketNet2HorizontalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet2HorizontalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet3HorizontalMatrix =  groupRacketMatrix * racketNet3HorizontalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet3HorizontalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet4HorizontalMatrix =  groupRacketMatrix * racketNet4HorizontalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet4HorizontalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet5HorizontalMatrix =  groupRacketMatrix * racketNet5HorizontalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet5HorizontalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet6HorizontalMatrix =  groupRacketMatrix * racketNet6HorizontalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet6HorizontalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet7HorizontalMatrix =  groupRacketMatrix * racketNet7HorizontalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet7HorizontalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet8HorizontalMatrix =  groupRacketMatrix * racketNet8HorizontalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet8HorizontalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet9HorizontalMatrix =  groupRacketMatrix * racketNet9HorizontalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet9HorizontalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet10HorizontalMatrix =  groupRacketMatrix * racketNet10HorizontalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet10HorizontalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet11HorizontalMatrix =  groupRacketMatrix * racketNet11HorizontalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet11HorizontalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet12HorizontalMatrix = groupRacketMatrix *  racketNet12HorizontalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet12HorizontalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet13HorizontalMatrix =  groupRacketMatrix * racketNet13HorizontalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet13HorizontalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    mat4 groupRacketNet14HorizontalMatrix =  groupRacketMatrix * racketNet14HorizontalMatrix;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet14HorizontalMatrix)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);


}

void drawTennisNet(GLuint colorLocation, GLuint modelMatrixLocation, GLuint EBOCubeModelLength) {
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

    // Loop through every matrix and draw corresponding net
    for (int i = 0; i < netMatrices.size(); i++) {
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(netMatrices[i])[0][0]);
        glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);
    }

    // Render Net Pillars
    vec3 netPillarDimensions(1.0f, verticalNet.y + 1, 1.0f);
    mat4 netPillarMatrix = scale(IDENTITY_MATRIX, netPillarDimensions) * translationAboveGrid;

    mat4 redPillarMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 0.0f, positionZ)) * netPillarMatrix;
    mat4 bluePillarMatrix = translate(IDENTITY_MATRIX, vec3(0.0f, 0.0f, -positionZ)) * netPillarMatrix;

    glUniform4fv(colorLocation, 1, &colorRed[0]);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(redPillarMatrix)[0][0]);
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

    glUniform4fv(colorLocation, 1, &colorBlue[0]);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(bluePillarMatrix)[0][0]);
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);
}

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

void drawSkybox(GLuint colorLocation, GLuint modelMatrixLocation, GLuint EBOCubeModelLength) {
    const float modifier = 100.0f;

    mat4 skyboxScale = scale(IDENTITY_MATRIX, vec3(modifier, modifier, modifier));

    mat4 skybox = skyboxScale;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(skybox)[0][0]); 
    glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

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

    // Create a glfw window with specified size
    // glfwCreateWindow(width, size, title, monitor, share)
    GLFWwindow * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "COMP 371 - Quiz 1", NULL, NULL);
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

    // compile the shader strings and create shader ids for them
    GLuint vertexShaderId = loadAndCompileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShaderId = loadAndCompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    // create shader program and attach shaders to the program.
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShaderId);
    glAttachShader(shaderProgram, fragmentShaderId);
    glLinkProgram(shaderProgram);

    // check if link was properly done
    int success;
    char infoLog[INFO_LOG_SIZE];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, INFO_LOG_SIZE, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Assign OpenGL to use shaderProgram that we've set
    glUseProgram(shaderProgram);

    // Initialize uniform for color
    GLuint colorLocation = glGetUniformLocation(shaderProgram, "color");

    // General Clean Up
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    
    // Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

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

    // --------------- 3. INITIALIZE MATRICES AND RENDER -----------

    // Initialize MVP matrices of the entire world to identity matrices
    mat4 modelMatrix(1.0f);
    mat4 viewMatrix(1.0f);
    mat4 projectionMatrix(1.0f);
    mat4 IDENTITY_MATRIX(1.0f);

    // --> RACKET MATRIX TRANSFORMATIONS
    GLfloat movementSpeed = 1.0f;
    mat4 movementTranslationMatrix(1.0f);
    mat4 movementScaleMatrix(1.0f);

    // Set up group rackets
    mat4 groupFirstRacketDTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, 0.0f, 0.0f)); // located upper left of grid
    mat4 groupSecondRacketATranslate = translate(IDENTITY_MATRIX, vec3(-GRID_SLICES_X/4.0f, 0.0f, GRID_SLICES_Z/4.0f)); // located bottom left of grid
    mat4 groupThirdRacketNTranslate = translate(IDENTITY_MATRIX, vec3(GRID_SLICES_X/4.0f, 0.0f, -GRID_SLICES_Z/4.0f)); // located  right of grid
    mat4 groupFourthRacketITranslate = translate(IDENTITY_MATRIX, vec3(GRID_SLICES_X/4.0f, 0.0f, GRID_SLICES_Z/4.0f)); // located upper right of grid
    
    // Initial Translation
    groupFirstRacketD.racketTranslation = groupFirstRacketDTranslate;
    groupSecondRacketA.racketTranslation = groupSecondRacketATranslate;
    groupThirdRacketN.racketTranslation = groupThirdRacketNTranslate;
    groupFourthRacketI.racketTranslation = groupFourthRacketITranslate;

    groupFirstRacketD.updateGroupMatrix();
    groupSecondRacketA.updateGroupMatrix();
    groupThirdRacketN.updateGroupMatrix();
    groupFourthRacketI.updateGroupMatrix();

    // MATRIX TRANSLATIONS FOR ALPHABET
    mat4 frontLetterTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, 0.0f, 0.75f));
    mat4 backLetterTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, 0.0f, -0.75f));

    // Initial lookAt camera angle
    viewMatrix = lookAt(vec3(7.0f, 10.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    GLfloat lastFrameTime = glfwGetTime();

    while(!glfwWindowShouldClose(window)) {

        // TIMER CODE
        // dt = delta time
        GLfloat dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

        // clear current buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GLuint modelMatrixLocation = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        // Set LookAt at ViewMatrix to see corner of cube (default camera position)
        GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewMatrixLocation, 1 , GL_FALSE, &viewMatrix[0][0]);

        // Set Projection Matrix to perspective view as default
        projectionMatrix = perspective(radians(100.0f), ASPECT_RATIO, 0.01f, 1000.0f);
        GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "proj");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
        
        // CUBE - Render the base 3D Cube
        glUniform4fv(colorLocation, 1, &colorBeige[0]);
        glBindVertexArray(VAOCubeModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOCubeModel);
        // glDrawElements(GL_TRIANGLES, EBOCubeModelLength, GL_UNSIGNED_INT, 0);

        glUniform4fv(colorLocation, 1, &colorSkybox[0]);
        glBindVertexArray(VAOSkybox);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOSkybox);
        drawSkybox( colorLocation,  modelMatrixLocation,  EBOCubeModelLength);


         

        // RACKETS - Render the 4 rackets
        glUniform4fv(colorLocation, 1, &colorRed[0]);
        drawRacket(groupFirstRacketD.groupMatrix, colorLocation, modelMatrixLocation, EBOCubeModelLength); // racket D
        drawRacket(groupSecondRacketA.groupMatrix, colorLocation, modelMatrixLocation, EBOCubeModelLength); // racket A 
        drawRacket(groupThirdRacketN.groupMatrix, colorLocation, modelMatrixLocation, EBOCubeModelLength); // racket N
        drawRacket(groupFourthRacketI.groupMatrix, colorLocation, modelMatrixLocation, EBOCubeModelLength); // racket I

        // ALPHABET - render d a n i
        // Letter D 
        glUniform4fv(colorLocation, 1, &colorBlue[0]);
        glBindVertexArray(VAODModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBODModel);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupFirstRacketD.groupMatrix)[0][0]);
        drawDModel(groupFirstRacketD.groupMatrix, colorLocation, modelMatrixLocation, EBOCubeModelLength);

        glUniform4fv(colorLocation, 1, &colorLightBlue[0]);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupFirstRacketD.groupMatrix)[0][0]);
        drawDModel((groupFirstRacketD.groupMatrix * frontLetterTranslate), colorLocation, modelMatrixLocation, EBOCubeModelLength);

        glUniform4fv(colorLocation, 1, &colorDarkBlue[0]);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupFirstRacketD.groupMatrix)[0][0]);
        drawDModel((groupFirstRacketD.groupMatrix * backLetterTranslate), colorLocation, modelMatrixLocation, EBOCubeModelLength);

        // Letter A
        glUniform4fv(colorLocation, 1, &colorRed[0]);
        glBindVertexArray(VAOAModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOAModel);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupSecondRacketA.groupMatrix)[0][0]);
        drawAModel(groupSecondRacketA.groupMatrix, colorLocation, modelMatrixLocation, EBOCubeModelLength);

        glUniform4fv(colorLocation, 1, &colorLightRed[0]);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupSecondRacketA.groupMatrix)[0][0]);
        drawAModel((groupSecondRacketA.groupMatrix * frontLetterTranslate), colorLocation, modelMatrixLocation, EBOCubeModelLength);

        glUniform4fv(colorLocation, 1, &colorDarkRed[0]);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupSecondRacketA.groupMatrix)[0][0]);
        drawAModel((groupSecondRacketA.groupMatrix * backLetterTranslate), colorLocation, modelMatrixLocation, EBOCubeModelLength);

        // Letter N
        glUniform4fv(colorLocation, 1, &colorGreen[0]);
        glBindVertexArray(VAONModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOAModel);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupThirdRacketN.groupMatrix)[0][0]);
        drawNModel(groupThirdRacketN.groupMatrix, colorLocation, modelMatrixLocation, EBOCubeModelLength);

        glUniform4fv(colorLocation, 1, &colorLightGreen[0]);
        glBindVertexArray(VAONModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOAModel);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupThirdRacketN.groupMatrix)[0][0]);
        drawNModel((groupThirdRacketN.groupMatrix * frontLetterTranslate), colorLocation, modelMatrixLocation, EBOCubeModelLength);

        glUniform4fv(colorLocation, 1, &colorDarkGreen[0]);
        glBindVertexArray(VAONModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOAModel);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupThirdRacketN.groupMatrix)[0][0]);
        drawNModel((groupThirdRacketN.groupMatrix * backLetterTranslate), colorLocation, modelMatrixLocation, EBOCubeModelLength);

        // Letter I
        glUniform4fv(colorLocation, 1, &colorYellow[0]);
        glBindVertexArray(VAOIModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOIModel);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupFourthRacketI.groupMatrix)[0][0]);
        drawIModel(groupFourthRacketI.groupMatrix, colorLocation, modelMatrixLocation, EBOCubeModelLength);

        glUniform4fv(colorLocation, 1, &colorLightYellow[0]);
        glBindVertexArray(VAOIModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOIModel);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupFourthRacketI.groupMatrix)[0][0]);
        drawIModel((groupFourthRacketI.groupMatrix * frontLetterTranslate), colorLocation, modelMatrixLocation, EBOCubeModelLength);

        glUniform4fv(colorLocation, 1, &colorDarkYellow[0]);
        glBindVertexArray(VAOIModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOIModel);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupFourthRacketI.groupMatrix)[0][0]);
        drawIModel((groupFourthRacketI.groupMatrix * backLetterTranslate), colorLocation, modelMatrixLocation, EBOCubeModelLength);

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
        drawTennisNet(colorLocation, modelMatrixLocation, EBOCubeModelLength);


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

        // Setting Camera to selected racket
        if (currentlySelectedGroupRacket != NULL) {
            viewMatrix = lookAt(currentlySelectedGroupRacket->returnRacketCenter() + vec3(0.0f, 0.0f, GRID_SLICES_Z/4), currentlySelectedGroupRacket->returnRacketCenter(), vec3(0.0f,  1.0f, 0.0f));
        }

        // SELECT RACKET
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            currentlySelectedGroupRacket = &groupFirstRacketD;
            // viewMatrix = lookAt(currentlySelectedGroupRacket->racketTranslation * vec4(0.0f, 7.0f, 0.0f, 1.0f), vec3(-GRID_SLICES_X/4.0f, 7.0f, -50.0f), vec3(0.0f,  1.0f, 0.0f));

        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            currentlySelectedGroupRacket = &groupSecondRacketA;
            // viewMatrix = lookAt(vec3(-GRID_SLICES_X/4.0f, 7.0f, GRID_SLICES_Z/2), vec3(-GRID_SLICES_X/4.0f, 7.0f, -50.0f), vec3(0.0f,  1.0f, 0.0f));
            
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            currentlySelectedGroupRacket = &groupThirdRacketN;
            // viewMatrix = lookAt(vec3(GRID_SLICES_X/4.0f, 7.0f, 0), vec3(GRID_SLICES_X/4.0f, 7.0f, -50.0f), vec3(0.0f,  1.0f, 0.0f));
            
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            currentlySelectedGroupRacket = &groupFourthRacketI;
            // viewMatrix = lookAt(vec3(GRID_SLICES_X/4.0f, 7.0f, GRID_SLICES_Z/2), vec3(GRID_SLICES_X/4.0f, 7.0f, -50.0f), vec3(0.0f,  1.0f, 0.0f));
            
        }

        // CONTROL MODEL POSITION AND ORIENTATION - WASD keys
        if (currentlySelectedGroupRacket != NULL && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && isShiftKeyPressed) {
            float travelDistance = movementSpeed * dt;
            mat4 distanceTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, travelDistance, 0.0f) );
            currentlySelectedGroupRacket->racketTranslation = distanceTranslate * currentlySelectedGroupRacket->racketTranslation;
            currentlySelectedGroupRacket->updateGroupMatrix();
        }
        // not mandatory but added movement for z axis too
        // else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        //     mat4 rotationTransformation = rotate(mat4(1.0f), -1.f/360.f*pi<float>(), vec3(1.f, 0.f, 0.f));
        //     currentlySelectedGroupRacket = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * currentlySelectedGroupRacket;
        // }
        
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
        // not mandatory but added movement for -z axis too 
        // else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        //     mat4 rotationTransformation = rotate(IDENTITY_MATRIX, 1.f/360.f*pi<float>(), vec3(1.0f, 0.0f, 0.0f));
        //     currentlySelectedGroupRacket = movementTranslationMatrix * rotationTransformation * inverse(movementTranslationMatrix) * currentlySelectedGroupRacket;
        // }

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


        // Upon pressing escape, stop window, and terminate program later.
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

    }

    // Terminate the program.
    glDeleteProgram(shaderProgram);
    glfwTerminate();
}