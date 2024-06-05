#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;
uniform vec3 color;
uniform bool useTexture;
uniform float alpha;

void main() {
    if (useTexture) {
        vec4 texColor = texture(texture1, TexCoords);
        FragColor = vec4(texColor.rgb, texColor.a); 
    } else {
        FragColor = vec4(color, alpha);  
    }
}
