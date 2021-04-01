
void PutPixelSDL(__global uint * write_buffer, int index, float3 colour)
{
  uint r = (uint)(clamp( 255*colour.x, 0.f, 255.f ) );
  uint g = (uint)(clamp( 255*colour.y, 0.f, 255.f ) );
  uint b = (uint)(clamp( 255*colour.z, 0.f, 255.f ) );

  write_buffer[index] = (128<<24) + (r<<16) + (g<<8) + b;
}

__kernel void Shader(__global uint * write_buffer, float time, cl_camera camera){
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

