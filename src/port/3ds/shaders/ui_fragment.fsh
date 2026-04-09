
uniform sampler2D uTexture;
uniform int uUseTexture;

in vec2 vTexCoord;
in vec4 vColor;

out vec4 fragColor;

void main() {
    vec4 color = vColor;
    
    if (uUseTexture == 1) {
        color = color * texture(uTexture, vTexCoord);
    }
    
    fragColor = color;
}
