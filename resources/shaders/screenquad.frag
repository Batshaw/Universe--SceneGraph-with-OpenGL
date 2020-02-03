#version 150

// input form vert
in vec2 text_coords;

// input from uniform
uniform sampler2D colorTexture;
uniform int model_type;

// output
out vec4 out_color;

// 7: GrayScale
// 8: Horizontal Mirroring
// 9: Vertical Mirroring
// 0: Blur with 3x3 Gaussian Kernel

// Gaussian Blur Kernel
vec4 gaussian_blur(in vec2 texCoords) {
    // Pixel Size:
    // pass_TexCoord = pixel_size * gl_FragCoord
    // --> pixel_size = pass_TexCoord / gl_FragCoord
    vec2 pixel_size = texCoords / gl_FragCoord.xy;
    vec4 pixel_color[9];
    vec4 sum = vec4(0.0);

    // 3x3 matrix
    // |(-x, +y)/16| |(+0, +y)/8| |(+x, +y)/16|
    // |(-x, +0)/8 | |(+0, +0)/4| |(+x, +0)/8 |
    // |(-x, -y)/16| |(+0, -y)/8| |(+x, -y)/16|
    pixel_color[0] = texture(colorTexture, (vec2(texCoords.x - pixel_size.x, texCoords.y + pixel_size.y))) * 0.0625;
    pixel_color[1] = texture(colorTexture, (vec2(texCoords.x, texCoords.y + pixel_size.y))) * 0.125;
    pixel_color[2] = texture(colorTexture, (vec2(texCoords.x + pixel_size.x, texCoords.y + pixel_size.y))) * 0.0625;
    pixel_color[3] = texture(colorTexture, (vec2(texCoords.x - pixel_size.x, texCoords.y))) * 0.125;
    pixel_color[4] = texture(colorTexture, (vec2(texCoords.x, texCoords.y))) * 0.25;
    pixel_color[5] = texture(colorTexture, (vec2(texCoords.x + pixel_size.x, texCoords.y))) * 0.125;
    pixel_color[6] = texture(colorTexture, (vec2(texCoords.x - pixel_size.x, texCoords.y - pixel_size.y))) * 0.0625;
    pixel_color[7] = texture(colorTexture, (vec2(texCoords.x , texCoords.y - pixel_size.y))) * 0.125;
    pixel_color[8] = texture(colorTexture, (vec2(texCoords.x + pixel_size.x, texCoords.y - pixel_size.y))) * 0.0625;

    for(int i = 0; i <= 8; i++) {
        sum += pixel_color[i];
    }
    return sum;
}

void main() {

    vec2 used_text_coords = text_coords;
    float luminance;

    // Horizontal Mirroring
    if(model_type == 8) {
        used_text_coords.y = 1.0f - used_text_coords.y;   // invert the texture with respect to the y-axis
    }
    // Vertical Mirroring
    if(model_type == 9) {
        used_text_coords.x = 1.0f - used_text_coords.x;   // with respect to the x-axis
    }

    // out color after 2 types
    out_color = texture(colorTexture, used_text_coords);

    // Using of Gaussian Blur Kernel
    if(model_type == 0) {
        out_color = gaussian_blur(used_text_coords);
    }

    // GrayScale Mode
    if(model_type == 7) {
        float luminance = (0.229*out_color.r + 0.587*out_color.g + 0.114*out_color.b);
        out_color =vec4(vec3(luminance), 1.0);
    }
}