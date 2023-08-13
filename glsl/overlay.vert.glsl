#version 150

uniform int u_liquidType;
uniform float u_Time;

in vec4 vs_Pos;
in vec4 vs_Col;
in vec2 vs_UV;

out vec4 fs_Col;
out vec2 fs_UV;

void main()
{

    fs_Col = vs_Col;    
    gl_Position = vs_Pos;

    fs_UV = vs_UV;

    //Scuffed water wiggle effect
    if (u_liquidType == 1 || u_liquidType == 2) {
        float x = vs_Pos.x;
        float z = vs_Pos.y;
        fs_UV.y += (sin(z + u_Time * 24.f) + sin(z * 2.f + u_Time * 30.f) * 0.1f + sin(z * 12.f + u_Time * 12.f) * 0.3f + cos(z * 0.75f + u_Time * 7.0f))/250.f;
        fs_UV.x += (sin(x + u_Time * 30.f) + sin(x * 5.f + u_Time * 16.f) * 0.1f + sin(x * 14.f + u_Time * 22.f) * 0.2f + cos(x * 0.25f + u_Time * 5.0f))/270.f;

    }



}
