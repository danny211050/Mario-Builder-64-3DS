#!/usr/bin/env python3
"""
3D Model to 2D Banner Renderer for Mario Builder 64
Renders the OBJ 3D model to a 256x128 BMP banner for 3DS
"""

import struct
import os
from PIL import Image, ImageDraw, ImageOps
import numpy as np

BANNER_DIR = "banner 3d model"
OUTPUT_DIR = "build_3ds"
OBJ_FILE = os.path.join(BANNER_DIR, "bd1418b9c4a7125dfede4973bb7b5e9d.obj")
TEXTURE_FILE = os.path.join(BANNER_DIR, "texture_pbr_20250901.png")
MTL_FILE = os.path.join(BANNER_DIR, "material.mtl")

class SimpleOBJLoader:
    """Simplified OBJ loader that handles large files efficiently"""
    
    def __init__(self, filepath, max_vertices=50000):
        self.vertices = []
        self.uvs = []
        self.normals = []
        self.faces = []
        self.face_uvs = []
        self.max_vertices = max_vertices
        
        print(f"Loading OBJ: {filepath}")
        self._load_obj(filepath)
        print(f"Loaded {len(self.vertices)} vertices, {len(self.faces)} faces")
        
        if len(self.faces) > 10000:
            print(f"Decimating from {len(self.faces)} to ~10000 faces...")
            self._decimate(10000)
    
    def _load_obj(self, filepath):
        """Stream-load OBJ file to handle large files"""
        vertices = []
        uvs = []
        normals = []
        faces = []
        face_uvs = []
        
        # Every Nth vertex for large files
        skip_factor = 1
        
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            line_count = 0
            for line in f:
                line_count += 1
                if line_count % 100000 == 0:
                    print(f"  Processed {line_count} lines...")
                
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                
                parts = line.split()
                if not parts:
                    continue
                
                if parts[0] == 'v':
                    # Vertex
                    if len(parts) >= 4:
                        vertices.append([float(parts[1]), float(parts[2]), float(parts[3])])
                
                elif parts[0] == 'vt':
                    # UV
                    if len(parts) >= 3:
                        uvs.append([float(parts[1]), float(parts[2])])
                
                elif parts[0] == 'vn':
                    # Normal
                    if len(parts) >= 4:
                        normals.append([float(parts[1]), float(parts[2]), float(parts[3])])
                
                elif parts[0] == 'f':
                    # Face - can be v/vt/vn format
                    face_verts = []
                    face_uv = []
                    
                    for i in range(1, len(parts)):
                        # Parse "v/vt/vn" or "v//vn" or "v"
                        face_parts = parts[i].split('/')
                        v_idx = int(face_parts[0]) - 1  # OBJ is 1-indexed
                        face_verts.append(v_idx)
                        
                        if len(face_parts) > 1 and face_parts[1]:
                            vt_idx = int(face_parts[1]) - 1
                            face_uv.append(vt_idx)
                    
                    # Triangulate if needed (quads -> 2 triangles)
                    if len(face_verts) >= 3:
                        faces.append([face_verts[0], face_verts[1], face_verts[2]])
                        if face_uv:
                            face_uvs.append([face_uv[0], face_uv[1], face_uv[2]])
                        
                        if len(face_verts) == 4:
                            faces.append([face_verts[0], face_verts[2], face_verts[3]])
                            if face_uv:
                                face_uvs.append([face_uv[0], face_uv[2], face_uv[3]])
        
        self.vertices = np.array(vertices)
        self.uvs = np.array(uvs) if uvs else np.array([[0, 0]])
        self.normals = np.array(normals) if normals else np.array([[0, 0, 1]])
        self.faces = faces
        self.face_uvs = face_uvs
    
    def _decimate(self, target_faces):
        """Simple decimation - keep every Nth face"""
        keep_ratio = target_faces / len(self.faces)
        keep_every = max(1, int(1 / keep_ratio))
        
        new_faces = []
        new_face_uvs = []
        
        for i, face in enumerate(self.faces):
            if i % keep_every == 0:
                new_faces.append(face)
                if i < len(self.face_uvs):
                    new_face_uvs.append(self.face_uvs[i])
        
        self.faces = new_faces
        self.face_uvs = new_face_uvs
        print(f"Decimated to {len(self.faces)} faces")
    
    def get_bounds(self):
        """Get bounding box"""
        if len(self.vertices) == 0:
            return None
        return np.min(self.vertices, axis=0), np.max(self.vertices, axis=0)
    
    def normalize(self):
        """Center and scale model to fit in unit cube"""
        bounds = self.get_bounds()
        if bounds is None:
            return
        
        min_v, max_v = bounds
        center = (min_v + max_v) / 2
        size = np.max(max_v - min_v)
        
        if size > 0:
            self.vertices = (self.vertices - center) / size

def render_to_image(obj_loader, texture_img, width=256, height=128):
    """Render 3D model to 2D image"""
    
    print(f"Rendering to {width}x{height}...")
    
    # Create output image with grey background
    img = Image.new('RGB', (width, height), (60, 60, 60))
    pixels = np.array(img)
    
    # Get vertices
    verts = obj_loader.vertices.copy()
    
    # Better view angle - rotate to show front of model
    angle_y = -0.5  # Rotate around Y to see front
    angle_x = 0.1   # Slight tilt up/down
    
    # Rotation matrices
    cos_y, sin_y = np.cos(angle_y), np.sin(angle_y)
    rot_y = np.array([
        [cos_y, 0, sin_y],
        [0, 1, 0],
        [-sin_y, 0, cos_y]
    ])
    
    cos_x, sin_x = np.cos(angle_x), np.sin(angle_x)
    rot_x = np.array([
        [1, 0, 0],
        [0, cos_x, -sin_x],
        [0, sin_x, cos_x]
    ])
    
    # Apply rotations
    verts = verts @ rot_y.T @ rot_x.T
    
    # Move model in front of camera - closer for bigger appearance
    verts[:, 2] += 1.2
    
    # Get texture
    tex_w, tex_h = texture_img.size
    tex_pixels = np.array(texture_img)
    
    # Simple rasterizer
    def rasterize_triangle(p0, p1, p2, uv0, uv1, uv2):
        # Bounding box
        min_x = int(max(0, min(p0[0], p1[0], p2[0])))
        max_x = int(min(width - 1, max(p0[0], p1[0], p2[0])))
        min_y = int(max(0, min(p0[1], p1[1], p2[1])))
        max_y = int(min(height - 1, max(p0[1], p1[1], p2[1])))
        
        if min_x > max_x or min_y > max_y:
            return 0
        
        # Edge function
        def edge(a, b, c):
            return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0])
        
        area = edge(p0, p1, p2)
        if abs(area) < 0.001:
            return 0
        
        drawn = 0
        for y in range(min_y, max_y + 1):
            for x in range(min_x, max_x + 1):
                p = [x + 0.5, y + 0.5]
                
                w0 = edge(p1, p2, p) / area
                w1 = edge(p2, p0, p) / area
                w2 = edge(p0, p1, p) / area
                
                # Use >= -0.01 to handle edge cases
                if w0 >= -0.01 and w1 >= -0.01 and w2 >= -0.01:
                    # Normalize weights
                    sum_w = w0 + w1 + w2
                    if sum_w > 0:
                        w0, w1, w2 = w0/sum_w, w1/sum_w, w2/sum_w
                    
                    # Interpolate UV
                    u = w0 * uv0[0] + w1 * uv1[0] + w2 * uv2[0]
                    v = w0 * uv0[1] + w1 * uv1[1] + w2 * uv2[1]
                    
                    # Wrap UV
                    u = u % 1.0
                    v = 1.0 - (v % 1.0)
                    
                    # Sample texture
                    tx = int(u * (tex_w - 1)) % tex_w
                    ty = int(v * (tex_h - 1)) % tex_h
                    
                    pixels[y, x] = tex_pixels[ty, tx][:3]
                    drawn += 1
        
        return drawn
    
    # Sort faces by Z depth (painter's algorithm)
    face_depths = []
    for i, face in enumerate(obj_loader.faces):
        z = (verts[face[0]][2] + verts[face[1]][2] + verts[face[2]][2]) / 3
        face_depths.append((z, i))
    
    face_depths.sort(key=lambda x: x[0], reverse=True)
    
    # Render faces
    total_pixels = 0
    rendered = 0
    debug_count = 0
    
    for z, face_idx in face_depths[:8000]:
        face = obj_loader.faces[face_idx]
        
        # Get vertices
        v0, v1, v2 = verts[face[0]], verts[face[1]], verts[face[2]]
        
        # Skip if behind camera
        if v0[2] <= 0.1 or v1[2] <= 0.1 or v2[2] <= 0.1:
            continue
        
        # Perspective projection - bigger scale for larger model
        scale = min(width, height) * 1.2
        cx, cy = width // 2, height // 2
        
        p0 = [cx + v0[0] * scale / v0[2], cy - v0[1] * scale / v0[2]]
        p1 = [cx + v1[0] * scale / v1[2], cy - v1[1] * scale / v1[2]]
        p2 = [cx + v2[0] * scale / v2[2], cy - v2[1] * scale / v2[2]]
        
        # Debug: print first few projections
        if debug_count < 5:
            print(f"  Face {face_idx}: v0={v0}, p0={p0}")
            debug_count += 1
        
        # Get UVs
        if face_idx < len(obj_loader.face_uvs) and obj_loader.face_uvs[face_idx]:
            uv_idx = obj_loader.face_uvs[face_idx]
            uv0 = obj_loader.uvs[uv_idx[0]] if uv_idx[0] < len(obj_loader.uvs) else [0, 0]
            uv1 = obj_loader.uvs[uv_idx[1]] if uv_idx[1] < len(obj_loader.uvs) else [0, 0]
            uv2 = obj_loader.uvs[uv_idx[2]] if uv_idx[2] < len(obj_loader.uvs) else [0, 0]
        else:
            uv0 = [0, 0]
            uv1 = [0.5, 1]
            uv2 = [1, 0]
        
        pixels_drawn = rasterize_triangle(p0, p1, p2, uv0, uv1, uv2)
        if pixels_drawn > 0:
            total_pixels += pixels_drawn
            rendered += 1
        
        if rendered % 1000 == 0 and rendered > 0:
            print(f"  Rendered {rendered} faces, {total_pixels} pixels...")
    
    print(f"Rendered {rendered} faces, {total_pixels} total pixels")
    return Image.fromarray(pixels)

def create_icon_from_banner(banner_img):
    """Create 48x48 icon from banner"""
    # Crop center square and resize
    w, h = banner_img.size
    min_dim = min(w, h)
    left = (w - min_dim) // 2
    top = (h - min_dim) // 2
    
    icon = banner_img.crop((left, top, left + min_dim, top + min_dim))
    icon = icon.resize((48, 48), Image.LANCZOS)
    return icon

def save_bmp(img, filepath):
    """Save image as 24-bit BMP"""
    img = img.convert('RGB')
    w, h = img.size
    
    # BMP header
    row_size = ((24 * w + 31) // 32) * 4
    pixel_data_size = row_size * h
    file_size = 54 + pixel_data_size
    
    header = bytearray([
        0x42, 0x4D,  # BM
        file_size & 0xFF, (file_size >> 8) & 0xFF,
        (file_size >> 16) & 0xFF, (file_size >> 24) & 0xFF,
        0, 0, 0, 0,  # Reserved
        54, 0, 0, 0,  # Offset to pixel data
        40, 0, 0, 0,  # DIB header size
        w & 0xFF, (w >> 8) & 0xFF, (w >> 16) & 0xFF, (w >> 24) & 0xFF,
        h & 0xFF, (h >> 8) & 0xFF, (h >> 16) & 0xFF, (h >> 24) & 0xFF,
        1, 0,  # Planes
        24, 0,  # Bits per pixel
        0, 0, 0, 0,  # Compression
        pixel_data_size & 0xFF, (pixel_data_size >> 8) & 0xFF,
        (pixel_data_size >> 16) & 0xFF, (pixel_data_size >> 24) & 0xFF,
        0, 0, 0, 0,  # X ppm
        0, 0, 0, 0,  # Y ppm
        0, 0, 0, 0,  # Colors used
        0, 0, 0, 0,  # Important colors
    ])
    
    # Pixel data (bottom-up, BGR)
    pixels = np.array(img)
    with open(filepath, 'wb') as f:
        f.write(header)
        
        for y in range(h - 1, -1, -1):
            row = bytearray()
            for x in range(w):
                r, g, b = pixels[y, x]
                row.extend([b, g, r])
            # Pad to 4-byte boundary
            while len(row) % 4 != 0:
                row.append(0)
            f.write(row)

def main():
    print("=" * 50)
    print("Mario Builder 64 - 3D Banner Generator")
    print("=" * 50)
    print()
    
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    
    # Check if 3D model exists
    if not os.path.exists(OBJ_FILE):
        print(f"Error: OBJ file not found: {OBJ_FILE}")
        print("Falling back to generated banner...")
        # Use simple generator
        import generate_3ds_banner as fallback
        fallback.main()
        return
    
    print(f"Found 3D model: {OBJ_FILE}")
    print(f"File size: {os.path.getsize(OBJ_FILE) / (1024*1024):.1f} MB")
    print()
    
    # Load texture
    texture_img = None
    if os.path.exists(TEXTURE_FILE):
        print(f"Loading texture: {TEXTURE_FILE}")
        texture_img = Image.open(TEXTURE_FILE).convert('RGB')
        # Resize texture for memory efficiency
        max_tex_size = 1024
        if max(texture_img.size) > max_tex_size:
            texture_img = texture_img.resize(
                (min(texture_img.width, max_tex_size),
                 min(texture_img.height, max_tex_size)),
                Image.LANCZOS
            )
        print(f"Texture size: {texture_img.size}")
    else:
        print(f"Warning: Texture not found, using solid color")
        texture_img = Image.new('RGB', (256, 256), (200, 50, 50))
    
    print()
    
    # Load and process 3D model
    try:
        obj = SimpleOBJLoader(OBJ_FILE, max_vertices=100000)
        obj.normalize()
        
        print()
        
        # Render to banner
        banner_img = render_to_image(obj, texture_img, 256, 128)
        
        # Save banner
        banner_path = os.path.join(OUTPUT_DIR, "banner.bmp")
        save_bmp(banner_img, banner_path)
        print(f"Banner saved: {banner_path}")
        
        # Create icon
        icon_img = create_icon_from_banner(banner_img)
        icon_path = os.path.join(OUTPUT_DIR, "icon.bmp")
        save_bmp(icon_img, icon_path)
        print(f"Icon saved: {icon_path}")
        
        # Also save as PNG for viewing
        banner_img.save(os.path.join(OUTPUT_DIR, "banner_preview.png"))
        icon_img.save(os.path.join(OUTPUT_DIR, "icon_preview.png"))
        
        print()
        print("=" * 50)
        print("SUCCESS! 3D model rendered to banner.")
        print("=" * 50)
        print()
        print("Files created:")
        print(f"  - {banner_path} (256x128)")
        print(f"  - {icon_path} (48x48)")
        print(f"  - build_3ds/banner_preview.png")
        print(f"  - build_3ds/icon_preview.png")
        
    except Exception as e:
        print(f"Error processing 3D model: {e}")
        import traceback
        traceback.print_exc()
        print()
        print("Falling back to generated banner...")
        import generate_3ds_banner as fallback
        fallback.create_generated_banner()
        fallback.create_icon()

if __name__ == "__main__":
    main()
