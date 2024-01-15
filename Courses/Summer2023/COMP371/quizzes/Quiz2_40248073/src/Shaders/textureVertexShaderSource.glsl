#version 330 core
layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aUV;

uniform vec4 color;
uniform vec3 lightPos;
uniform mat4 model = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 proj = mat4(1.0);

out vec4 vertexColor;
out vec3 pos;
out vec3 normal;
out vec3 eyeDir;
out vec3 lightDir;
out vec2 vertexUV;

void main() {
    vertexColor = color;
    mat4 MVP = proj * view * model;
    gl_Position = MVP * vec4(aPos, 1.0);

    pos = (model * vec4(aPos, 1)).xyz;
    vec3 vertexPosition = ( view * model * vec4(aPos, 1)).xyz;
    eyeDir = vec3(0,0,0) - vertexPosition;

    vec3 lightPosCamera = ( view * vec4(lightPos, 1)).xyz;
    lightDir = lightPosCamera + eyeDir;
    normal = (transpose(inverse(view * model)) * vec4(aNormal, 1)).xyz;

    vertexUV = aUV;
}