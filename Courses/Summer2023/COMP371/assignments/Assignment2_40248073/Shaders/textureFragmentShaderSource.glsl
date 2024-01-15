#version 330 core

uniform vec3 lightPos;
uniform sampler2D textureSampler;
uniform sampler2D shadowMap;
uniform int enableTexture;


in vec3 vertexColor;
in vec3 pos;
in vec3 normal;
in vec3 eyeDir;
in vec3 lightDir;
in vec2 vertexUV;
in vec4 fragPosLightSpace;

out vec3 FragColor;



void main() {
    vec4 textureColor = texture( textureSampler, vertexUV ); 


    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    float lightPower = 20.0f;

    vec3 diffuseFactor = vec3(1.0f, 1.0f, 1.0f); 
    vec3 ambientFactor = vec3(0.3f, 0.3f, 0.3f);
    vec3 specularFactor = vec3(1.0f, 1.0f, 1.0f);

    float dist = length(lightPos - pos);
    vec3 n = normalize(normal);
    vec3 l = normalize(lightDir);

    float cosTheta = clamp(dot(n,l), 0, 1);

    vec3 E = normalize(eyeDir);
    vec3 R = reflect(-l, n);

    float cosAlpha = clamp(dot(E, R), 0, 1);

    vec3 shadowTextureCoord = (fragPosLightSpace.xyz / fragPosLightSpace.w) * 0.5 + 0.5;
    float shadow = 1.0f;
    float bias = 0.05f;
    if (shadowTextureCoord.z > texture(shadowMap, shadowTextureCoord.xy).r) {
        shadow = 0.5f;
    }

    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;

    if (enableTexture == 1) {
        ambientColor = vec3(textureColor) * vertexColor * ambientFactor;
        diffuseColor = vec3(textureColor) * (vertexColor * diffuseFactor * lightColor * lightPower * cosTheta / (1 + dist * 0.5 + dist * dist * 0.05));
    }
    else if (enableTexture == 0) {
        ambientColor = vertexColor * ambientFactor;
        diffuseColor = (vertexColor * diffuseFactor * lightColor * lightPower * cosTheta / (1 + dist * 0.5 + dist * dist * 0.05));
    }

    specularColor = (specularFactor * lightColor * lightPower * pow(cosAlpha,5) / (1 + dist * 0.5 + dist * dist * 0.05));

    // FragColor = texture(shadowMap, shadowTextureCoord.xy).zzz;
    // FragColor = shadowTextureCoord.xyz;
    // FragColor = shadowTextureCoord.zzz;
    FragColor = (ambientColor + shadow * (diffuseColor + specularColor));
    // FragColor = vec3(textureColor) * (ambientColor + shadow * (diffuseColor + specularColor));
}