#version 330 core

#define BIAS 0.01

void main()
{
    // this is already being done so
    // no need to do it explicitly
    gl_FragDepth = gl_FragCoord.z;
    //gl_FragDepth += gl_FrontFacing ? BIAS : 0.0;
    
}