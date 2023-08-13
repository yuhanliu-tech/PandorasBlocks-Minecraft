#version 150

in vec4 vs_Pos;

void main()
{
    gl_Position = vs_Pos; // pass vertex position down pipeline to assemble triangles (passthrough vertex shader)

    // one fragment for each pixel of screen

}
