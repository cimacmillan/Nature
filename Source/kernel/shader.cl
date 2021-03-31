
void PutPixelSDL(__global uint * write_buffer, int index, float3 colour)
{
  uint r = (uint)(clamp( 255*colour.x, 0.f, 255.f ) );
  uint g = (uint)(clamp( 255*colour.y, 0.f, 255.f ) );
  uint b = (uint)(clamp( 255*colour.z, 0.f, 255.f ) );

  write_buffer[index] = (128<<24) + (r<<16) + (g<<8) + b;
}

__kernel void Shader(__global uint * write_buffer, float time){
    int x_pos = get_global_id(0);
    int y_pos = get_global_id(1);
    int x_size = get_global_size(0);
    int index = x_pos + (y_pos * x_size);

    float red = fmod(((float)x_pos) + time, 100.0f) / 100.0f;
    float green = fmod(((float)y_pos) + time, 100.0f) / 100.0f;

    float3 colour = (float3)(red, green, 0.0f);

    PutPixelSDL(write_buffer, index, colour);
}

