
// N64-style fragment shader for Citro3D
uniform sampler2D uTexture;
uniform int uUseTexture;
uniform int uUseFog;
uniform vec4 primColor;
uniform vec4 envColor;
uniform vec4 fogColor;
uniform int combineMode;

// Inputs
in vec2 vTexCoord;
in vec4 vColor;
in float vFog;
in float vDepth;

// Output
out vec4 fragColor;

// Combine modes (simplified)
#define COMBINE_SHADE 0
#define COMBINE_MODULATE 1
#define COMBINE_BLEND 2
#define COMBINE_DECAL 3

void main() {
    vec4 color = vColor;
    
    // Apply texture if enabled
    if (uUseTexture == 1) {
        vec4 texColor = texture(uTexture, vTexCoord);
        
        // Simple modulate combine mode
        if (combineMode == COMBINE_MODULATE) {
            color = color * texColor;
        } else if (combineMode == COMBINE_DECAL) {
            color = texColor;
        } else {
            color = texColor * color;
        }
    }
    
    // Apply fog if enabled
    if (uUseFog == 1) {
        color = mix(fogColor, color, vFog);
    }
    
    // Alpha test
    if (color.a < 0.01) {
        discard;
    }
    
    fragColor = color;
}
