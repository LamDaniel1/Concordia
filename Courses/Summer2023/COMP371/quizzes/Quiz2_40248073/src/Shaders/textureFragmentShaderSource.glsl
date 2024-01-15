#version 330 core

uniform vec3 lightPos;
uniform sampler2D textureSampler;
uniform int enableTexture;

// spot light calculations
uniform vec3 spotLPos;
uniform vec3 spotLightLookAt;
uniform float spotLIntensity;

in vec4 vertexColor;
in vec3 pos;
in vec3 normal;
in vec3 eyeDir;
in vec3 lightDir;
in vec2 vertexUV;

out vec4 FragColor;

void main() {
    // Spotlight variables
    vec3 spotDir = spotLightLookAt - spotLPos;
    vec3 fragDir = pos - spotLPos;
    float fragCos = dot(normalize(spotDir), normalize(fragDir));
    float cutoffCos = cos(radians(30.0f));

    vec4 textureColor = texture( textureSampler, vertexUV ); 

    vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 0.5f);
    float lightPower = 10.0f;

    vec4 diffuseFactor = vec4(1.0f, 1.0f, 1.0f, 0.5f); 
    vec4 ambientFactor = vec4(0.3f, 0.3f, 0.3f, 0.5f);
    vec4 specularFactor = vec4(1.0f, 1.0f, 1.0f, 0.5f);

    float dist = length(lightPos - pos);
    vec3 n = normalize(normal);
    vec3 l = normalize(lightDir);

    float cosTheta = clamp(dot(n,l), 0, 1);

    vec3 E = normalize(eyeDir);
    vec3 R = reflect(-l, n);
    float cosAlpha = clamp(dot(E, R), 0, 1);
    


    vec4 ambientColor;
    vec4 diffuseColor;
    vec4 specularColor;

    // Toggle Texture On and Off
    if (enableTexture == 1) {
        ambientColor = textureColor * vertexColor * ambientFactor;
        diffuseColor = textureColor * vertexColor * diffuseFactor * lightColor * lightPower * cosTheta / (1 + dist * 0.5 + dist * dist * 0.05);
    }
    else if (enableTexture == 0) {
        ambientColor = vertexColor * ambientFactor;
        diffuseColor = vertexColor * diffuseFactor * lightColor * lightPower * cosTheta / (1 + dist * 0.5 + dist * dist * 0.05);
    }

    specularColor = (specularFactor * lightColor * lightPower * pow(cosAlpha,5) / (1 + dist * 0.5 + dist * dist * 0.05));


    // Calculate Spot Light toggle & conditions
    if (spotLIntensity > 0.01f && (fragCos > cutoffCos))
    {
        if (enableTexture == 1) {
            diffuseColor += textureColor * vertexColor * diffuseFactor * (spotLIntensity * pow(fragCos, 80));
        } else if (enableTexture == 0) {
            diffuseColor += vertexColor * diffuseFactor * (spotLIntensity * pow(fragCos, 80));
        }
        specularColor += specularFactor * vertexColor * (spotLIntensity * pow(fragCos, 80));
    }

    FragColor = (ambientColor + diffuseColor) + specularColor;
}