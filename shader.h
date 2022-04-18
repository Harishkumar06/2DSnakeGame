#ifndef SHADER_H
#define SHADER_H


//Vertex Shader source code
const char* vertexShaderSource = "#version 450 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); \n"
"}\0";

//fragment shader source code for Border
const char* fragmentShaderSource_Border = "#version 450 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(0.85f, 0.85f, 0.85f, 1.0f); \n"
"}\n\0";

//fragment shader source code for Snake
const char* fragmentShaderSource_Snake = "#version 450 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(0.85f, 0.85f, 0.85f, 1.0f); \n"
"}\n\0";

////fragment shader source code for Food
//const char* fragmentShaderSource_Food = "#version 450 core\n"
//"out vec4 FragColor;\n"
//"void main()\n"
//"{\n"
//"	FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f); \n"
//"}\n\0";

#endif // !SHADER_H
