#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <glad/glad.h>
#include <gl/glfw3.h>
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";
struct Buffering {
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
	
};

int width = 800;
int height = 600;
float normalizeX(float x) {
    return (2.0f * x) / width - 1.0f;
}
float normalizeY(float y) {
    // Pencere yüksekliði 600 ise, y'yi [-1, 1] aralýðýna normalize et
    return 1.0f - (2.0f * y) / height;
}
struct Buffering buffers;
int triangleShaderCount = 0;
int dataCount = 0;
float *vertices = NULL;
void glCheckError(const char* file, int line) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        printf("OpenGL Hatasý (%s:%d): %d\n", file, line, err);
    }
}
#define GL_CHECK() ;glCheckError(__FILE__, __LINE__)
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0,0,width,height);
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        float x = (float)normalizeX(xpos);
        float y = (float)normalizeY(ypos);
        //printf("Left mouse button pressed at (%f, %f)\n", x, y);
        if(triangleShaderCount < 3){
        	if (vertices == NULL) {
                vertices = (float*)malloc(3 * sizeof(float)); // Ýlk kez bellek tahsis et
            }
           /*
		    else {
            		if(dataCount % 3 == 0){
            		vertices = (float*)realloc(vertices,sizeof(vertices) * 3);
				}
			}
		   */
        	*(vertices+dataCount) = x;
        	*(vertices+dataCount+1) = y;
        	*(vertices+dataCount+2) = 0.f;
        	dataCount+=3;
        	triangleShaderCount++;
        		
		}
    }
}
struct Buffering create_buffers(){
	struct Buffering buffering;
	int count;
	/*for(count = 0;count<dataCount;count++){
		printf("%f\n",*(vertices + count));
	}*/
	unsigned int indices[] = {
		0,1,2
	};
	glGenVertexArrays(1,&buffering.VAO);
	glGenBuffers(1,&buffering.VBO);
	glGenBuffers(1,&buffering.EBO);
	glBindVertexArray(buffering.VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER,buffering.VBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(float) * dataCount,vertices,GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffering.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int) * 3,indices,GL_STATIC_DRAW);
	glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    return buffering;
}
unsigned int shadering(){
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n");
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

int main(int argc, char** argv) {
	setlocale(LC_ALL,"turkish");

	if(!glfwInit()){
		printf("GLFW Baþlatýlamadý");
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);/*OPENGL maksimum sürüm deðeri*/
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);/*OPENGL minimum sürüm deðeri*/
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);/*OPENGL Çekirdeðini Kullanmak Ýstediðimizi Ýþletim Sistemine Belirttik*/
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);/*Mac OS da çalýþmasý için*/
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Ders", NULL, NULL);/*Pencere Oluþturduk*/
	if(window == NULL){
		printf("Pencere Oluþturulamadý");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
	    printf("GLAD Baþlatýlamadý");
	    glfwTerminate();
	    return -1;
	}
	glfwSetMouseButtonCallback(window,mouse_callback);
	glViewport(0,0,width,height);
	unsigned int shaderProgram = shadering();
	while(!glfwWindowShouldClose(window)){
		glClearColor(0.2f, 0.7f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		if(triangleShaderCount == 3){
				buffers = create_buffers();
		}
		glUseProgram(shaderProgram);
        glBindVertexArray(buffers.VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
	    glfwPollEvents();
	}
	glDeleteVertexArrays(1, &buffers.VAO);
    glDeleteBuffers(1, &buffers.VBO);
    glDeleteBuffers(1, &buffers.EBO);
    glDeleteProgram(shaderProgram);
	free(vertices);
	glfwTerminate();
	return 0;
}
