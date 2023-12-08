#version 330 core
in vec4 fragColor; 
flat in int objectType;

out vec4 outColor;

void main()
{
    if (objectType == 1) {
        outColor = vec4(0.6, 0.0, 0.0, 1.0); // red
    } else if (objectType == 0) {
        outColor = vec4(0.0, 0.0, 0.0, 1.0); // green
    } else if(objectType == 2){
        outColor = vec4(0.0, 0.5, 0.0, 1.0); // black (default)
    }

}
