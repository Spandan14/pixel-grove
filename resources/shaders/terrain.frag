#version 330 core
in vec4 vert;
in float worldHeight;
in vec4 norm;
in vec3 color;
in vec2 uv_coord;

uniform bool wireShadeOn;
uniform vec3 lightDir;

uniform vec4 cameraPos;

uniform sampler2D rocks_texture;
uniform sampler2D sandgrass_texture;

float k_a = 0.1;
vec4 c_ambient = vec4(0.2, 0.2, 0.2, 1.0);
float k_d = 0.7;
vec4 c_diffuse = vec4(0.2, 0.2, 0.8, 1.0);
float k_s = 0.3;
vec4 c_specular = vec4(1.0, 1.0, 1.0, 1.0);
float shininess = 0.0;

float blend = 0.85;

out vec4 fragColor;

void main(void)
{
    // initialize 0 color
    fragColor = vec4(0);

    // ambient color
    fragColor += k_a * c_ambient;

    float f_att = 1.0; // directional light only

    vec4 intensity = vec4(1.0, 1.0, 1.0, 1.0); // light is colored white for now

    // diffuse
    vec4 diffuseColor = k_d * c_diffuse;
    float diffuseFactor = min(1.0, max(0.0, dot(normalize(vec3(norm)), -lightDir)));

    vec4 textureColor = vec4(1);
    if (worldHeight < -0.025) {
        textureColor = texture(rocks_texture, uv_coord);
    } else if (worldHeight > -0.025 && worldHeight < 0.025) {
        // lerp between textures
        textureColor = mix(texture(rocks_texture, uv_coord), texture(sandgrass_texture, uv_coord), (worldHeight + 0.025) / 0.05);
    } else {
        textureColor = texture(sandgrass_texture, uv_coord);
    }

    diffuseColor = textureColor * blend + diffuseColor * (1.0 - blend);

    vec4 diffuseAddition = diffuseColor * intensity; // SHOULD EVALUATE TO ELEMENT WISE VECTOR

    fragColor += f_att * diffuseFactor * diffuseAddition;

    // specular
    vec3 lightReflection = normalize(reflect(lightDir, normalize(vec3(norm))));
    vec3 directionToCamera = normalize(vec3(cameraPos) - vec3(vert));
    float specularFactor = pow(max(dot(-lightReflection, directionToCamera), 0.0), shininess);
    if (shininess == 0.0) {
        specularFactor = 0.0;
    }

    vec4 specularAddition = intensity * c_specular;
    fragColor += f_att * k_s * specularFactor * specularAddition;


    if (wireShadeOn) {
        fragColor = vec4(color, 1.0f);
    }

}
// old method
// fragColor = vec4((clamp(dot(norm.xyz, lightDir), 0, 1) * 0.7 +  0.3) * textureColor);
