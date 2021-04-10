
void PutPixelSDL(__global uint * write_buffer, int index, float3 colour)
{
  uint r = (uint)(clamp( 255*colour.x, 0.f, 255.f ) );
  uint g = (uint)(clamp( 255*colour.y, 0.f, 255.f ) );
  uint b = (uint)(clamp( 255*colour.z, 0.f, 255.f ) );

  write_buffer[index] = (128<<24) + (r<<16) + (g<<8) + b;
}

__kernel void PixelShader(__global uint * write_buffer, float time, cl_camera camera) {
    int x_pos = get_global_id(0);
    int y_pos = get_global_id(1);
    int x_size = get_global_size(0);
    int y_size = get_global_size(1);
    int index = x_pos + (y_pos * x_size);
    float aspectRatio = (float)x_size / (float) y_size;
    float x_world_pos = ((((float)x_pos / (float)x_size) * 2.0f - 1.0f) * camera.zoom + camera.pos.x);
    float y_world_pos = (((((float)y_pos / (float)y_size) * 2.0f - 1.0f) / aspectRatio) * camera.zoom + camera.pos.y);

// TODO below should be removed
    if (x_world_pos < 0) {
        x_world_pos = -x_world_pos;
    }

     if (y_world_pos < 0) {
        y_world_pos = -y_world_pos;
    }
    float red = fmod(x_world_pos, 1.0f) / 1.0f;
    float green = fmod(y_world_pos, 1.0f) / 1.0f;
    float3 colour = (float3)(red, green, 0.0f);

    PutPixelSDL(write_buffer, index, colour);
}

#define POINT_RADIUS 1.0f
__kernel void PointShader(__global uint * write_buffer, cl_camera camera, int screenWidth, int screenHeight, __global cl_point * point_buffer) {
    int pointId = get_global_id(0);
    int pointSize = get_global_size(0);
    float2 screen = (float2)(screenWidth, screenHeight);
    
    cl_point point = point_buffer[pointId];

    float2 transformedPoint = (point.pos - camera.pos) * camera.zoom;
    float2 projectedPoint = transformedPoint * (screen * 0.5f) + (screen * 0.5f);
    int screenX = (int)projectedPoint.x;
    int screenY = (int)projectedPoint.y;

    int radius = max((int)(POINT_RADIUS * camera.zoom), 1);

    int x1 = screenX - radius;
    int x2 = screenX + radius;

    int y1 = screenY - radius;
    int y2 = screenY + radius;

    x1 = min(max(x1, 0), screenWidth - 1);
    x2 = min(max(x2, 0), screenWidth - 1);
    y1 = min(max(y1, 0), screenHeight - 1);
    y2 = min(max(y2, 0), screenHeight - 1);

    for (int x = x1; x < x2; x++) {
        for (int y = y1; y < y2; y++) {
            int index = x + (y * screenWidth);
            float3 colour = (float3)(1.0f, 1.0f, 1.0f);
            PutPixelSDL(write_buffer, index, colour);
        }
    }
} 

__kernel void PointResolver(__global cl_point * point_buffer, __global cl_point * point_destination_buffer) {
    int pointId = get_global_id(0);
    int pointSize = get_global_size(0);

    float2 pos = point_buffer[pointId].pos;
    float2 vel = point_buffer[pointId].vel;

    for (int i = 0; i < pointSize; i++) {
        if (pointId == i) {
            continue;
        }
        
        float2 destPos = point_buffer[i].pos;
        float2 destVel = point_buffer[i].vel;

        float dis = distance(pos, destPos);
        float2 normal = normalize(destPos - pos);

        float force = (1.0f / (dis)) * 0.000001f;
        vel = vel + normal * force;
    }

    cl_point destination;
    destination.pos = pos + vel * 0.1f;
    destination.vel = vel;

    point_destination_buffer[pointId] = destination;
} 