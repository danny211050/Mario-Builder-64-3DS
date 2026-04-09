#!/usr/bin/env python3
"""
Generate Citro3D shaders for N64 GBI rendering
"""
import os
import sys

# Vertex shader - mimics N64 RSP transformation
vertex_shader_src = """
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
"""

# Fragment shader - mimics N64 RDP rendering
fragment_shader_src = """
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
"""

# 2D UI shader for touch screen interface
ui_vertex_shader = """
uniform mat4 projection;

in vec2 aPosition;
in vec2 aTexCoord;
in vec4 aColor;

out vec2 vTexCoord;
out vec4 vColor;

void main() {
    gl_Position = projection * vec4(aPosition, 0.0, 1.0);
    vTexCoord = aTexCoord;
    vColor = aColor;
}
"""

ui_fragment_shader = """
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
"""

def write_shader(path, src):
    """Write shader source to file"""
    with open(path, 'w') as f:
        f.write(src)
    print(f"Generated: {path}")

def main():
    shader_dir = "src/port/3ds/shaders"
    
    # Create directory if needed
    os.makedirs(shader_dir, exist_ok=True)
    
    # Write shaders
    write_shader(f"{shader_dir}/n64_vertex.vsh", vertex_shader_src)
    write_shader(f"{shader_dir}/n64_fragment.fsh", fragment_shader_src)
    write_shader(f"{shader_dir}/ui_vertex.vsh", ui_vertex_shader)
    write_shader(f"{shader_dir}/ui_fragment.fsh", ui_fragment_shader)
    
    print("\nShaders generated successfully!")
    print("Use picasso and picasso-pica to compile these to .shbin files")
    return 0

if __name__ == "__main__":
    sys.exit(main())
