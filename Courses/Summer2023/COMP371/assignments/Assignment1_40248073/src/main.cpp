/* COMP 371 - ASSIGNMENT 1
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
#define GRID_SCALING_FACTOR_X 100
#define GRID_SCALING_FACTOR_Z 133.33
#define GRID_SLICES 100
#define OFFSET_GRID_X 50.0f
#define OFFSET_GRID_Z 50.66f
#define GLM_FORCE_RADIANS
float CAMERA_ANGULAR_SPEED = 10.0f;
float FOV = radians(100.0f);
const float FOV_MODIFIER = radians(20.0f);
const float RIGHT_ANGLE = pi<float>()/2;
const float CORNER_ANGLE = pi<float>()/3; // desired corner angle assuming starting from horizontal
const float ARM_ANGLE = RIGHT_ANGLE - CORNER_ANGLE;
const float CAMERA_ANGLE_MODIFIER = 1.f/360.f*pi<float>();

// COLORS
const vec3 colorRed = vec3(1.0f, 0.0f, 0.0f);
const vec3 colorYellow = vec3(1.0f, 1.0f, 0.0f);
const vec3 colorBlue = vec3(0.0f, 0.0f, 1.0f);
const vec3 colorGreen = vec3(0.0f, 1.0f, 0.0f);
const vec3 colorBeige = vec3(238.0/255.0f, 217.0/255.0f, 196.0/255.0f);
const vec3 colorWhite = vec3(1.0f, 1.0f, 1.0f);



// VERTICES AND INDICES
// Vertices and Indices already contain model coordinates
std::vector<vec3> vertices;
int offsetVertices = vertices.size(); // needed when rendering grid to offset the indices count

std::vector<uvec4> indices;

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
GLuint loadAndCompileShader(const char* shaderSource, GLenum shaderType) {
    int success;
    char infoLog[INFO_LOG_SIZE];

    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderSource, NULL);
    glCompileShader(shaderId);

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHAER::COMPILATION_FAILED\n%s\n%s", infoLog, shaderSource);
        exit(EXIT_FAILURE);
    }
    return shaderId;
}

// GENERATE GRID ALGORITHM
// REFERENCE: https://stackoverflow.com/questions/58494179/how-to-create-a-grid-in-opengl-and-drawing-it-with-lines
void drawGrid() {

    // Calculate and add grid vertices (vertices ranging from x and z = {-50.0f to 50.0f} )
    for (int i=0; i<=GRID_SLICES; ++i) {
        for (int j=0; j <= GRID_SLICES; ++j) {
            float x = (float)j / (float)GRID_SLICES * GRID_SCALING_FACTOR_X;
            float y = 0;
            float z = (float)i / (float)GRID_SLICES * GRID_SCALING_FACTOR_Z;
            vertices.push_back(vec3(x-OFFSET_GRID_X, y, z-OFFSET_GRID_Z));
        }
    }

    // Calculate and add grid indices
    for (int i = 0; i < GRID_SLICES; ++i) {
        for (int j = 0; j < GRID_SLICES; ++j) {

            int row1 =  i    * (GRID_SLICES+1);
            int row2 = (i+1) * (GRID_SLICES+1);

            indices.push_back(uvec4(row1+j, row1+j+1, row1+j+1, row2+j+1));
            indices.push_back(uvec4(row2+j+1, row2+j, row2+j, row1+j));

        }
    }
}

void drawCoordinates(GLuint &VAOCoordinates, GLuint &VBOCoordinates, GLuint &EBOCoordinates, GLuint &EBOCoordinatesLength) {

    std::vector<vec3> coordinatesVertices = {
        vec3( 0.0f, 0.25f, 0.0f ), // origin
        vec3( 5.0f, 0.25f, 0.0f ), // x axis
        vec3( 0.0f, 5.25f, 0.0f ), // y axis
        vec3( 0.0f, 0.25f, 6.5f ), // z axis
    };

    std::vector<int> coordinatesIndices = {
        0, 1, // x axis
        0, 2, // y axis
        0, 3, // z axis
    };

    // Set up VAO of coordinates
    glGenVertexArrays(1, &VAOCoordinates);
    glBindVertexArray(VAOCoordinates);
    
    // Set up VBO of coordinates
    glGenBuffers(1, &VBOCoordinates);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCoordinates);
    glBufferData(GL_ARRAY_BUFFER, coordinatesVertices.size()*sizeof(vec3), &coordinatesVertices[0][0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Set up EBO of coordinates
    glGenBuffers(1, &EBOCoordinates);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOCoordinates);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, coordinatesIndices.size()*sizeof(GLuint), &coordinatesIndices[0], GL_STATIC_DRAW);
    
    EBOCoordinatesLength = indices.size();
}

// drawCubeModel() sets up vertices & indices of base cube and allows rendering in either Points, Lines, Triangles (default) with respective EBO
void drawCubeModel(GLuint &VAOCubeModel, GLuint &VBOCubeModel, GLuint &EBOCubeModel, GLuint &EBOCubeModelLines, GLuint &EBOCubeModelPoints, GLuint &EBOCubeModelLength, GLuint &EBOCubeModelLinesLength, GLuint &EBOCubeModelPointsLength) {
    
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

    // INDICES OF LINES CUBE
    std::vector<GLuint> cubeIndicesLines = {
        //right face
        2, 6,
        3, 7, 
        2, 3,
        6, 7,

        //left face
        1, 0, 
        5, 4,
        1, 5,
        0, 4,
        
        //side lines
        0, 3, 
        1, 2, 
        5, 6, 
        4, 7,
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
    glEnableVertexAttribArray(0);
    
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

float returnRandomNumber() {
    GLfloat min = -GRID_SLICES/2.0f;
    GLfloat max = GRID_SLICES/2.0f;
    double randomNum = min + (((double)rand())/RAND_MAX * (max - min));
    return randomNum;

}


int main(int argc,  char * argv[]) {
    
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
    GLFWwindow * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "COMP 371 - Assignment 1", NULL, NULL);
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


    // -------------IMPORTANT FOR DRAWING GRID------------------------
    // Update Vertices and Indices by drawing Grid 
    int cubeIndicesCount = indices.size() * 4;
    drawGrid();
    GLuint indicesLength = (GLuint)indices.size()*4;

    // ------------- VAO for Grid -----------
    GLuint VAOGrid;
    // create a vertex array and set id in VAOGrid variable
    glGenVertexArrays(1, &VAOGrid);
    glBindVertexArray(VAOGrid);

    // CREATE A VBOGrid FOR VERTICE COORDINATES AND LINK TO VAOGrid
    GLuint VBOGrid;
    glGenBuffers(1, &VBOGrid);
    glBindBuffer(GL_ARRAY_BUFFER, VBOGrid);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vec3), value_ptr(vertices[0]), GL_STATIC_DRAW);
    // link VBOGrid to VAOGrid
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    GLuint EBOGrid;
    glGenBuffers(1, &EBOGrid);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOGrid);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(vec3), value_ptr(indices[0]), GL_STATIC_DRAW);


    // ---------------- VAO for Coordinates --------------
    GLuint VAOCoordinates, VBOCoordinates, EBOCoordinates, EBOCoordinatesLength;
    drawCoordinates(VAOCoordinates, VBOCoordinates, EBOCoordinates, EBOCoordinatesLength);

    // ------------------------- VAO for base Cube Model -----------------
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

    // Set the viewport to avoid the jittering in the beginning.
    GLint frameBufferWidth, frameBufferHeight;
    glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
    glViewport(0, 0, frameBufferWidth, frameBufferHeight);

    // Initialize MVP matrices of the entire world to identity matrices
    mat4 modelMatrix(1.0f);
    mat4 viewMatrix(1.0f);
    mat4 projectionMatrix(1.0f);
    mat4 IDENTITY_MATRIX(1.0f);

    // Same initial dimensions and translation for both upper arm and fore arm
    vec3 armDimensions = vec3(0.4f, 3.5f, 0.4f);
    vec3 translationAboveGrid = vec3(0.0f, 0.5f, 0.0f);
    vec3 handDimensions = vec3(1.5f, 1.5f, 0.5f);
    vec3 fingerDimensions = vec3(0.75f, 0.2f, 0.2f);
    vec3 fingerVerticalDimensions = vec3(0.2f, 1.25f, 0.2f);

    // Initialize matrices of arm and racket objects
    mat4 upperArmMatrix(1.0f);
    mat4 foreArmMatrix(1.0f);
    mat4 handMatrix(1.0f);
    mat4 finger1Matrix(1.0f);
    mat4 finger2Matrix(1.0f);
    mat4 finger3Matrix(1.0f);
    mat4 finger4Matrix(1.0f);
    mat4 finger5Matrix(1.0f);

    
    // All racket components
    mat4 racketHandleAndSideDimensions(1.0f);
    mat4 racketCornerMatrix(1.0f);
    mat4 racketBottomLeftCornerMatrix(1.0f);
    mat4 racketBottomRightCornerMatrix(1.0f);
    mat4 racketLeftSideMatrix(1.0f);
    mat4 racketRightSideMatrix(1.0f);
    mat4 racketBottomSideMatrix(1.0f);
    mat4 racketTopSideMatrix(1.0f);

    // Net Components
    mat4 racketNetVerticalDimensions(1.0f);
    mat4 racketNet1VerticalMatrix(1.0f);
    mat4 racketNet2VerticalMatrix(1.0f);
    mat4 racketNet3VerticalMatrix(1.0f);
    mat4 racketNet4VerticalMatrix(1.0f);
    mat4 racketNet5VerticalMatrix(1.0f);

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

    // --> Hand
    mat4 handScale = scale(IDENTITY_MATRIX, handDimensions);
    mat4 handTranslation = translate(IDENTITY_MATRIX, vec3(0.0f, 0.5f, -1.5f));
    handMatrix = handScale * handTranslation;


    // --> Finger
    mat4 fingerHorizontalScale = scale(IDENTITY_MATRIX, fingerDimensions);
    mat4 fingerVerticalScale = scale(IDENTITY_MATRIX, fingerVerticalDimensions);
    mat4 finger1Translation = translate(IDENTITY_MATRIX, vec3(-handDimensions.x/2, handDimensions.y/2, 0.0f));
    mat4 finger2Translation = translate(IDENTITY_MATRIX, vec3(-handDimensions.x, handDimensions.y/2, 0.0f));
    mat4 finger3Translation = translate(IDENTITY_MATRIX, vec3(0.0f, handDimensions.y/2, 0.0f));
    mat4 finger4Translation = translate(IDENTITY_MATRIX, vec3(handDimensions.x, handDimensions.y/2, 0.0f));
    mat4 finger5Translation = translate(IDENTITY_MATRIX, vec3(handDimensions.x/2, handDimensions.y/2, 0.0f));
    finger1Matrix = fingerHorizontalScale * finger1Translation;
    finger2Matrix = fingerVerticalScale * finger2Translation;
    finger3Matrix = fingerVerticalScale * finger3Translation;
    finger4Matrix = fingerVerticalScale * finger4Translation;
    finger5Matrix = fingerHorizontalScale * finger5Translation;

    

    // --> Racket
    // RACKET MATRIX TRANSFORMATIONS
    const float racketThickness = 0.2f;
    const float racketNetThickness = 0.05f;
    const float racketLength = 2.0f;
    const float racketCornerLength = 1.0f;
    const float racketNetDeltaX = 0.2f;
    const float racketNetDeltaY = 0.2f;

    // remember: angle is from the vertical
    const float racketCornerUnitX = sin(ARM_ANGLE) * racketCornerLength; // corner unit x located at top left corner of racketCorner
    const float racketCornerUnitY = cos(ARM_ANGLE) * racketCornerLength;

    // Set up matrices transformations
    const float offsetY = racketThickness/2;

    mat4 racketHandleAndSideScale = scale(IDENTITY_MATRIX, vec3(racketThickness, racketLength, racketThickness));
    mat4 racketCornerScale = scale(IDENTITY_MATRIX, vec3(racketThickness, racketCornerLength, racketThickness));
    mat4 racketTranslateAboveGrid = translate(IDENTITY_MATRIX, translationAboveGrid);
    mat4 racketCornerTranslation = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength, 0.0f));
    mat4 racketRotation = rotate(IDENTITY_MATRIX, ARM_ANGLE, vec3(0.0f, 0.0f, 1.0f));

    mat4 racketLeftSideTranslate = translate(IDENTITY_MATRIX, vec3(-racketCornerUnitX, racketLength + racketCornerUnitY - offsetY, 0.0f));
    mat4 racketRightSideTranslate = translate(IDENTITY_MATRIX, vec3(racketCornerUnitX, racketLength + racketCornerUnitY - offsetY, 0.0f));

    mat4 racketTopAndBottomSideScale = scale(IDENTITY_MATRIX, vec3(racketCornerUnitX * 2, racketThickness, racketThickness));
    mat4 racketBottomSideTranslation = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY, 0.0f));
    mat4 racketTopSideTranslation = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY + racketLength - racketThickness, 0.0f ));

    // Racket Net Transformations
    mat4 racketNetVerticalScale = scale(IDENTITY_MATRIX, vec3(racketNetThickness, racketLength, racketNetThickness));
    mat4 racketNet1VerticalTranslate = translate(IDENTITY_MATRIX, vec3(-racketNetDeltaX * 2, racketLength + racketCornerUnitY - offsetY, 0.0f));
    mat4 racketNet2VerticalTranslate = translate(IDENTITY_MATRIX, vec3(-racketNetDeltaX * 1, racketLength + racketCornerUnitY - offsetY, 0.0f));
    mat4 racketNet3VerticalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY - offsetY, 0.0f));
    mat4 racketNet4VerticalTranslate = translate(IDENTITY_MATRIX, vec3(racketNetDeltaX * 1, racketLength + racketCornerUnitY - offsetY, 0.0f));
    mat4 racketNet5VerticalTranslate = translate(IDENTITY_MATRIX, vec3(racketNetDeltaX * 2, racketLength + racketCornerUnitY - offsetY, 0.0f));
    
    mat4 racketNetHorizontalScale = scale(IDENTITY_MATRIX, vec3(racketCornerUnitX * 2, racketNetThickness, racketNetThickness));
    mat4 racketNet1HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 + racketNetDeltaY * 1, 0.0f)); // starting from middle of net
    mat4 racketNet2HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 + racketNetDeltaY * 2, 0.0f));
    mat4 racketNet3HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 + racketNetDeltaY * 3, 0.0f));
    mat4 racketNet4HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 + racketNetDeltaY * 4, 0.0f));
    mat4 racketNet5HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 + racketNetDeltaY * 5, 0.0f));
    mat4 racketNet6HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 + 0.0f , 0.0f));
    mat4 racketNet7HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 - racketNetDeltaY * 1, 0.0f));
    mat4 racketNet8HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 - racketNetDeltaY * 2, 0.0f));
    mat4 racketNet9HorizontalTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, racketLength + racketCornerUnitY * 2 - racketNetDeltaY * 3, 0.0f));

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

    racketNet1HorizontalMatrix = racketNet1HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet2HorizontalMatrix = racketNet2HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet3HorizontalMatrix = racketNet3HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet4HorizontalMatrix = racketNet4HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet5HorizontalMatrix = racketNet5HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet6HorizontalMatrix = racketNet6HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet7HorizontalMatrix = racketNet7HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet8HorizontalMatrix = racketNet8HorizontalTranslate * racketNetHorizontalDimensions;
    racketNet9HorizontalMatrix = racketNet9HorizontalTranslate * racketNetHorizontalDimensions;

    // HIERARCHICAL MODELING
    // ----------- SET UP SHOULDER GROUP MATRIX -------------
    mat4 groupShoulderMatrix(1.0f);
    mat4 groupShoulderMatrixRotation = rotate(IDENTITY_MATRIX, -ARM_ANGLE, vec3(0.0f, 0.0f, 1.0f));
    groupShoulderMatrix = groupShoulderMatrixRotation;


    // ----------- SET UP ELBOW GROUP MATRIX ------------
    mat4 groupElbowMatrix(1.0f);
    mat4 groupElbowMatrixTranslate = translate(IDENTITY_MATRIX, vec3(armDimensions.x/2, armDimensions.y, 0.0f));
    mat4 groupElbowMatrixRotation = rotate(IDENTITY_MATRIX, 0.0f, vec3(0.0f, 0.0f, 1.0f));
    mat4 groupElbowMatrixTranslateUndo = translate(IDENTITY_MATRIX, vec3(-armDimensions.x/2, 0.0f, 0.0f)); // undo previous translation so rotation is done properly
    groupElbowMatrix = groupElbowMatrixTranslate * groupElbowMatrixRotation * groupElbowMatrixTranslateUndo;

    // ----------- SET UP WRIST GROUP MATRIX -----------
    mat4 groupWristMatrix(1.0f);
    mat4 groupWristMatrixTranslate = translate(IDENTITY_MATRIX, vec3(0.0f, armDimensions.y, 0.75f));
    groupWristMatrix = groupWristMatrixTranslate;

    // ---------- CAMERA -----------
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;
    float theta = radians(cameraHorizontalAngle);
    float phi = radians(cameraVerticalAngle);
    mat4 viewRotation(1.0f);

    vec3 cameraPosition = vec3(1.5f, 7.0f, 10.0f);
    vec3 cameraLookAt = vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));
    // vec3 cameraLookAt = vec3(0.0f, 0.0f, 0.0f);
    vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);


    // Initial lookAt camera angle
    mat4 initialWorldLookAt = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
    mat4 currentWorldLookAt = viewRotation * initialWorldLookAt;
    viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

    // Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Background color
    glClearColor(57/255.0f, 62/255.0f, 67/255.0f, 1.0f);

    // Time between current frame and last frame
    GLfloat lastFrameTime = glfwGetTime();
    GLuint spacebarLastState = glfwGetKey(window, GLFW_KEY_SPACE);

    // -------- MOUSE ---------
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

    // Sets cursor in the middle
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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


        // clear current buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GLuint modelMatrixLocation = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        // Set LookAt at ViewMatrix to see corner of cube (default camera position)
        GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewMatrixLocation, 1 , GL_FALSE, &viewMatrix[0][0]);

        // Set Projection Matrix to perspective view as default
        projectionMatrix = perspective(FOV, ASPECT_RATIO, 0.01f, 1000.0f);
        GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "proj");
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

        // CUBE - Render the base 3D Cube
        glUniform3fv(colorLocation, 1, &colorBeige[0]);
        glBindVertexArray(VAOCubeModel);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentCubeEBO);

        // ARM - Groups of Shoulder, Elbow & Hand
        mat4 groupShoulder = groupShoulderMatrix * upperArmMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupShoulder)[0][0]); // Drawing upper arm
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);
        mat4 groupElbow = groupShoulderMatrix * groupElbowMatrix * foreArmMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupElbow)[0][0]); // Drawing fore arm
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        // HAND - Render entire model of hand + fingers
        mat4 groupWrist = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * handMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupWrist)[0][0]);
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        mat4 groupFinger1 = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * handMatrix * finger1Matrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupFinger1)[0][0]);
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        mat4 groupFinger2 = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * handMatrix * finger2Matrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupFinger2)[0][0]);
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        mat4 groupFinger3 = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * handMatrix * finger3Matrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupFinger3)[0][0]);
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        mat4 groupFinger4 = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * handMatrix * finger4Matrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupFinger4)[0][0]);
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        mat4 groupFinger5 = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * handMatrix * finger5Matrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupFinger5)[0][0]);
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        
        // RACKET - Render entire model of racket
        // Racket Handle
        glUniform3fv(colorLocation, 1, &colorRed[0]);
        mat4 groupRacketHandleModel = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketHandleAndSideDimensions;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketHandleModel)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        // Racket Bottom Left Corner
        glUniform3fv(colorLocation, 1, &colorWhite[0]);
        mat4 groupRacketBottomLeftCornerMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketBottomLeftCornerMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketBottomLeftCornerMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        // Racket Bottom Right Corner
        mat4 groupRacketBottomRightCornerMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketBottomRightCornerMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketBottomRightCornerMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        // Racket Bottom Side
        glUniform3fv(colorLocation, 1, &colorRed[0]);
        mat4 groupRacketBottomSideMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketBottomSideMatrix;
        // mat4 groupRacketBottomSideMatrix = racketBottomSideMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketBottomSideMatrix)[0][0]);
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        // Racket Left Side 
        mat4 groupRacketLeftSideMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketLeftSideMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketLeftSideMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        // Racket Right Side
        mat4 groupRacketRightSideMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketRightSideMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketRightSideMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        // Racket Top Side
        mat4 groupRacketTopSideMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketTopSideMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketTopSideMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        // Racket Net - PLEASE REWRITE CODE USING FOR LOOP
        glUniform3fv(colorLocation, 1, &colorGreen[0]);

        // Vertical nets
        mat4 groupRacketNet1VerticalMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketNet1VerticalMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet1VerticalMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        mat4 groupRacketNet2VerticalMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketNet2VerticalMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet2VerticalMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        mat4 groupRacketNet3VerticalMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketNet3VerticalMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet3VerticalMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);
        
        mat4 groupRacketNet4VerticalMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketNet4VerticalMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet4VerticalMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        mat4 groupRacketNet5VerticalMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketNet5VerticalMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet5VerticalMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        // Horizontal Nets
        mat4 groupRacketNet1HorizontalMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketNet1HorizontalMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet1HorizontalMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        mat4 groupRacketNet2HorizontalMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketNet2HorizontalMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet2HorizontalMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        mat4 groupRacketNet3HorizontalMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketNet3HorizontalMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet3HorizontalMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        mat4 groupRacketNet4HorizontalMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketNet4HorizontalMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet4HorizontalMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        mat4 groupRacketNet5HorizontalMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketNet5HorizontalMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet5HorizontalMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        mat4 groupRacketNet6HorizontalMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketNet6HorizontalMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet6HorizontalMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        mat4 groupRacketNet7HorizontalMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketNet7HorizontalMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet7HorizontalMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        mat4 groupRacketNet8HorizontalMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketNet8HorizontalMatrix;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet8HorizontalMatrix)[0][0]); 
        glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        // looks better without last one
        // mat4 groupRacketNet9HorizontalMatrix = groupShoulderMatrix * groupElbowMatrix * groupWristMatrix * racketNet9HorizontalMatrix;
        // glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &(groupRacketNet9HorizontalMatrix)[0][0]); 
        // glDrawElements(currentRenderingMode, currentCubeEBOLength, GL_UNSIGNED_INT, 0);

        // GRID - Render the X,Z plane grid
        glUniform3fv(colorLocation, 1, &colorYellow[0]);
        glBindVertexArray(VAOGrid);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOGrid);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &mat4(1.0)[0][0]);
        glDrawElements(GL_LINES, indicesLength - cubeIndicesCount, GL_UNSIGNED_INT, (void *)cubeIndicesCount);
        
        // COORDINATES - Render the X, Y, Z Axis Coordinates onto the Screen
        glBindVertexArray(VAOCoordinates);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOCoordinates);
        // glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &mat4(1.0)[0][0]);
        glUniform3fv(colorLocation, 1, &colorWhite[0]);
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0); // draw x axis - red
        
        glUniform3fv(colorLocation, 1, &colorGreen[0]);
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(sizeof(GLuint)*2) ); // draw y axis - green

        glUniform3fv(colorLocation, 1, &colorBlue[0]);
        glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(sizeof(GLuint)*4) ); // draw z axis - blue

        // apply translation matrix to modelmatrix of shader


        // swap the buffer to the one already written, and place the current one as a canvas
        glfwSwapBuffers(window);
        glfwPollEvents();

        bool isShiftKeyPressed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

        // ------- INPUT EVENTS ----------
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            // viewMatrix = mat4(1.0f);
            viewMatrix = lookAt(vec3(0.0f, 10.0f, 10.0f), vec3(0.0f, 5.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            viewMatrix = lookAt(vec3(1.5f, 10.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            viewMatrix = lookAt(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            viewMatrix = lookAt(vec3(0.0f, 4.0f, 7.0f), vec3(0.0f, 3.0f, -4.0f), vec3(0.0f, 0.0f, -1.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            viewMatrix = lookAt(vec3(0.0f, 10.0f, 25.0f), vec3(0.0f, 9.0f, -4.0f), vec3(0.0f, 0.0f, -1.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            viewMatrix = lookAt(vec3(0.0f, 2.0f, 4.0f), vec3(0.0f, 2.0f, -4.0f), vec3(0.0f, 1.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
            viewMatrix = lookAt(vec3(25.0f, 15.0f, 50.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        }
        
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
    glDeleteProgram(shaderProgram);
    glfwTerminate();

}