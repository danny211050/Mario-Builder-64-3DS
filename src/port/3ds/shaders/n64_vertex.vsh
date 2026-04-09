
// N64-style vertex shader for Citro3D
uniform mat4 projection;
uniform mat4 modelview;
uniform vec4 primColor;
uniform vec4 envColor;
uniform float fogStart;
uniform float fogEnd;
uniform vec4 fogColor;

// Attributes
in vec3 aPosition;
in vec2 aTexCoord;
in vec4 aColor;

// Outputs
out vec2 vTexCoord;
out vec4 vColor;
out float vFog;
out float vDepth;

void main() {
    // Transform position
    vec4 pos = modelview * vec4(aPosition, 1.0);
    vec4 clipPos = projection * pos;
    
    gl_Position = clipPos;
    
    // Output texture coordinates
    vTexCoord = aTexCoord;
    
    // Calculate fog
    vDepth = -pos.z;
    vFog = clamp((fogEnd - vDepth) / (fogEnd - fogStart), 0.0, 1.0);
    
    // Vertex color with lighting
    vColor = aColor;
}
