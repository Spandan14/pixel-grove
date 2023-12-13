#version 330 core
in vec2 uv_coord;

uniform sampler2D myTexture;

uniform bool perPixel;
uniform bool kernelBased;

uniform float kernel[25];
uniform int screen_width;
uniform int screen_height;

out vec4 fragColor;

vec4 blur(){
    vec4 Color = vec4(0.f, 0.f, 0.f, 1.f);
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            float canvasU = uv_coord[0] - 2.f / float(screen_width) + (float(j) / float(screen_width));
            float canvasV = uv_coord[1] - 2.f / float(screen_height) + float(i) / float(screen_height);
            Color = Color + texture(myTexture, vec2(canvasU, canvasV));
        }
    }
    return Color * (1.f/25.f);
}

void main()
{
    fragColor = texture(myTexture, uv_coord);

    if(perPixel){
        fragColor = vec4(1.f - fragColor[0], 1.f - fragColor[1], 1.f - fragColor[2], 1.f - fragColor[3]);
    }

    if(kernelBased){
        fragColor = blur();
    }
}
