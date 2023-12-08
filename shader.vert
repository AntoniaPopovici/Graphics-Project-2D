#version 330 core

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor; // color
layout(location = 2) in int inObjectType; // object type

uniform mat4 myMatrix;
out vec4 fragColor;
flat out int objectType;

void main()
{
    gl_Position = myMatrix * inPosition;
    fragColor = inColor;
    objectType = int(inObjectType);
}
