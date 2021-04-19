
void PutPixelSDL(__global uint * write_buffer, int index, float3 colour)
{
  uint r = (uint)(clamp( 255*colour.x, 0.f, 255.f ) );
  uint g = (uint)(clamp( 255*colour.y, 0.f, 255.f ) );
  uint b = (uint)(clamp( 255*colour.z, 0.f, 255.f ) );

  write_buffer[index] = (128<<24) + (r<<16) + (g<<8) + b;
}

#define TRACER_MAG 0.f
__kernel void PixelShader(__global uint * write_buffer) {
    int x_pos = get_global_id(0);
    int y_pos = get_global_id(1);
    int x_size = get_global_size(0);
    int y_size = get_global_size(1);
    int index = x_pos + (y_pos * x_size);

    uint existing_colour = write_buffer[index];

    uint r = (existing_colour >> 16) & 0xFF;
    uint g = (existing_colour >> 8) & 0xFF;
    uint b = (existing_colour) & 0xFF;

    float redFloat = ((float)r) / (255.f);
    float greenFloat = ((float)g) / (255.f);
    float blueFloat = ((float)b) / (255.f);

    float3 background = (float3)(0.0f, 0.0f, 0.0f);
    float3 prevColour = (float3)(redFloat, greenFloat, blueFloat);
    float3 colour = (prevColour * TRACER_MAG) + (background * (1.0f - TRACER_MAG));

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

// Return the force against A when joint with B
float2 resolveJoint(cl_point a, cl_point b, cl_joint joint) {
    float dis = distance(a.pos, b.pos);

    if (dis == 0) {
        return (float2)(0.f, 0.f);
    }

    float2 norm = normalize(b.pos - a.pos);
    float displacement = dis - joint.target;

    float2 force = displacement * norm * 0.002f;

    return force;
}

#define DISTANCE_MIN 0.5f
#define DISTANCE_MAX 25.0f

__kernel void PointResolver(__global cl_point * point_buffer, __global cl_point * point_destination_buffer, __global cl_joint * joint_buffer, int jointCount) {
    int pointId = get_global_id(0);
    int pointSize = get_global_size(0);

    float2 pos = point_buffer[pointId].pos;
    float2 vel = point_buffer[pointId].vel;
    float2 force = (float2)(0.0f, 0.0f);
    float mass = 1.0f;

    // for (int i = 0; i < pointSize; i++) {
    //     if (pointId == i) {
    //         continue;
    //     }
        
    //     float2 destPos = point_buffer[i].pos;
    //     float2 destVel = point_buffer[i].vel;

    //     float dis = max(min(distance(pos, destPos), DISTANCE_MAX), DISTANCE_MIN);
    //     float2 normal = normalize(destPos - pos);

    //     float gravity = (1.0f / (dis * dis)) * 0.000001f;
    //     force += (gravity * normal);
    // }

    // TODO replace this with joint kernel
    for (int i = 0; i < jointCount; i++) {
        cl_joint joint = joint_buffer[i];

        if (joint.pointA == pointId) {
            force += resolveJoint(point_buffer[pointId], point_buffer[joint.pointB], joint);
        }

        if (joint.pointB == pointId) {
            force += resolveJoint(point_buffer[pointId], point_buffer[joint.pointA], joint);
        }
    }

    float2 friction = -vel * 0.02f;
    force += friction;

    float2 acc = force / mass;

    cl_point destination;
    destination.vel = vel + acc;
    destination.pos = pos + vel;
    point_destination_buffer[pointId] = destination;
} 