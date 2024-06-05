#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;
uniform vec3 color;
uniform bool useTexture;

void main() {
    if (useTexture) {
        FragColor = texture(texture1, TexCoords);
    } else {
        FragColor = vec4(color, 1.0f);
    }
}
