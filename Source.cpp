#include <include/glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include "irrKlang/include/irrKlang.h"
#include "shader.h"

using namespace std;
using namespace irrklang;

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 1080
#define MAX_SCORE 20

int Speed = 1;
int Score = 0;
int Direction = 1;
GLfloat vertices[12];

void KeyBoardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);

// This class takes care of all the work related the snake and the food aka Border
class Renderer {
public:
	// Defined things which I need
	GLfloat GridX = 0, GridY = 0;						// Coordinates of the corner of the Snake or Food 
	GLfloat PosX=0, PosY = 0;							// Back up coordinates usable in the main function
	GLuint VAO[2], VBO[2], EBO[2];						// Declaring the VAO, VBO and the indices needed
	GLuint ShaderProgramBorder, ShaderProgramSnake;		// Declaring the shaderprogram for the food and the snake
	
	double crntTime = 0;								// Decalring a variable to take the current time

	// The Constructor and the Decontructor
	Renderer(GLfloat x, GLfloat y) {
		GridX = x;
		GridY = y;
	};				
	~Renderer() {
	};

	// Takes in the coordinates declared above as input for creating the box as snake and food
	void drawGrid(GLfloat GridX, GLfloat GridY, int fragShaderNo) {
		PosX = GridX;
		PosY = GridY;
		//GLfloat vertices[] = /*{
		//	0.0f, 0.0f, 0.0f,
		//	0.0f, 0.1f, 0.0f,
		//	0.1f, 0.1f, 0.0f,
		//	0.1f, 0.0f, 0.0f,
		//};*/
		//{
		//   GridX,			GridY,			0.0f,
		//   GridX,			GridY + 0.1f,	0.0f,
		//   GridX + 0.1f,	GridY + 0.1f,	0.0f,
		//   GridX + 0.1f,	GridY,			0.0f
		//};

		vertices[0] = GridX;		vertices[1] = GridY;		vertices[2] = 0.0f;		// Left Bottom Corner
		vertices[3] = GridX;		vertices[4] = GridY + 0.1f; vertices[5] = 0.0f;		// Left Top Corner
		vertices[6] = GridX + 0.1f;	vertices[7] = GridY + 0.1f;	vertices[8] = 0.0f;		// Right Top Corner
		vertices[9] = GridX + 0.1f;	vertices[10]= GridY;		vertices[11]= 0.0f;		// Right Bottom Corner

		// Declaring indices to help create the grid
		GLuint indices[] = {
			0,1,2,
			2,3,0
		};

		// Declaring the vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		// Takes Fragment shader no. as the input and binds it with the Shader Program
		if (fragShaderNo == 0) {

			GLuint fragShaderBorder = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(fragShaderBorder, 1, &fragmentShaderSource_Border, NULL);
			glCompileShader(fragShaderBorder);

			ShaderProgramBorder = glCreateProgram();
			glAttachShader(ShaderProgramBorder, vertexShader);
			glAttachShader(ShaderProgramBorder, fragShaderBorder);

			glLinkProgram(ShaderProgramBorder);

			glDeleteShader(vertexShader);
			glDeleteShader(fragShaderBorder);
		}
		if (fragShaderNo == 1) {
			
			GLuint fragShaderSnake = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(fragShaderSnake, 1, &fragmentShaderSource_Snake, NULL);
			glCompileShader(fragShaderSnake);

			ShaderProgramSnake = glCreateProgram();
			glAttachShader(ShaderProgramSnake, vertexShader);
			glAttachShader(ShaderProgramSnake, fragShaderSnake);

			glLinkProgram(ShaderProgramSnake);

			glDeleteShader(vertexShader);
			glDeleteShader(fragShaderSnake);
		}

		// Creating the VAO and binding it with VBO and EBO
		glGenVertexArrays(1, &VAO[fragShaderNo]);
		glGenBuffers(1, &VBO[fragShaderNo]);
		glGenBuffers(1, &EBO[fragShaderNo]);

		glBindVertexArray(VAO[fragShaderNo]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[fragShaderNo]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[fragShaderNo]);

		// Introduce the vertices into the VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// Configuring the Vertex Attribute so that OpenGL knows how to read the VBO
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		// Enable the Vertex Attribute so that OpenGL knows to use it
		glEnableVertexAttribArray(0);
		// Bind both VAO & VBO to 0, to avoid modifying accidentally
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT);
	};

	// Makes the box solid or border depending upon the input
	void Render(int fragShaderNo) {

		// Instructing to create Border sqaure for Food and Solid Square for Snake
		if (fragShaderNo == 0) {
			// intructing OpenGL that we want to use Shader Program 
			glUseProgram(ShaderProgramBorder);
			glBindVertexArray(VAO[fragShaderNo]);
			glDrawArrays(GL_LINE_LOOP, 0, 4);

		}
		if (fragShaderNo == 1) {
			// intructing OpenGL that we want to use Shader Program 
			glUseProgram(ShaderProgramSnake);
			glBindVertexArray(VAO[fragShaderNo]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	};

	// Animating the moton of the snake
	void animate(int Speed, double prevTime, int fragShaderNo) {
		
		crntTime = glfwGetTime();					// Changing the value of the variable to current time
		
		// Changing the value of y coordinate to run the snake in loop
		if (GridY >= 1.0f)							
			GridY = -1.0f;
		else if (GridY <= -1.0f)
			GridY = 1.0f;

		// Changing the value of x coordinate to run the snake in loop
		if (GridX >= 1.0f)
			GridX = -1.0f;
		else if (GridX <= -1.0f)
			GridX = 1.0f;
		
		// Making the Snake move UP if Direction is 1 
		if (crntTime - prevTime >= 1 && Direction == 1) {
			GridY = GridY + 0.10f * Speed / 100;
			//std::cout << "X: " << GridX << ", del_t: " << crntTime << std::endl;
			prevTime = crntTime;
		}

		// Making the Snake move DOWN if Direction is 2 
		if (crntTime - prevTime >= 1 && Direction == 2) {
			GridY = GridY - 0.10f * Speed / 100;
			//std::cout << "X: " << GridX << ", del_t: " << crntTime << std::endl;
			prevTime = crntTime;
		}

		// Making the Snake move LEFT if Direction is 3
		if (crntTime - prevTime >= 1 && Direction == 3) {
			GridX = GridX - 0.10f * Speed / 100;
			//std::cout << "X: " << GridX << ", del_t: " << crntTime << std::endl;
			prevTime = crntTime;
		}

		// Making the Snake move UP if Direction is 1 
		if (crntTime - prevTime >= 1 && Direction == 4) {
			GridX = GridX + 0.10f * Speed / 100;
			//std::cout << "X: " << GridX << ", del_t: " << crntTime << std::endl;
			prevTime = crntTime;
		}

		// Sending the coordinates to drawgrid function to make the food or Snake
		drawGrid(GridX, GridY, fragShaderNo);				
	};

	// Creating food at random locations
	void makeFood(GLfloat X, GLfloat Y,GLfloat SnakeX,GLfloat SnakeY) {
		srand(time(0));			// Generating random values

		// Making the food to generate at the abve generated random values
		// Based on 4 different entry cases of the snake
		if (SnakeX <= X && SnakeX + 0.1 >= X &&
			SnakeY <= Y && SnakeY + 0.1 >= Y) {
			sndPlaySound(TEXT("crunch.wav"), SND_ASYNC);
			X = -1.0f * (float)rand() / RAND_MAX;
			Y = 1.0f * (float)rand() / RAND_MAX;
			drawGrid(X, Y, 0);

			Score++;
			cout << "\nScore increased!!\nNew Score:" << Score << endl;

		}
		else if (SnakeX <= X + 0.1 && SnakeX + 0.1 >= X +0.1 &&
			SnakeY <= Y && SnakeY + 0.1 >= Y) {
			sndPlaySound(TEXT("crunch.wav"), SND_ASYNC);
			X = (float)rand() / RAND_MAX - 1;
			Y = (float)rand() / RAND_MAX - 1;
			drawGrid(X, Y, 0);
			/*cout << "\nfood X: " << X
				<< " Snake X: " << SnakeX << endl;
			cout << "food Y: " << Y
				<< " Snake Y: " << SnakeY << endl;*/
			Score++;
			cout << "\nScore increased!!\nNew Score:" << Score << endl;

		}
		else if (SnakeX <= X && SnakeX + 0.1 >= X &&
			SnakeY <= Y + 0.1 && SnakeY + 0.1 >= Y + 0.1) {
			sndPlaySound(TEXT("crunch.wav"), SND_ASYNC);
			X = (float)rand() / RAND_MAX;
			Y = (float)rand() / RAND_MAX;
			drawGrid(X, Y, 0);
			/*cout << "\nfood X: " << X
				<< " Snake X: " << SnakeX << endl;
			cout << "food Y: " << Y
				<< " Snake Y: " << SnakeY << endl;*/
			Score++;
			cout << "\nScore increased!!\nNew Score:" << Score << endl;
		}
		else if (SnakeX <= X + 0.1 && SnakeX + 0.1 >= X + 0.1 &&
			SnakeY <= Y + 0.1 && SnakeY + 0.1 >= Y + 0.1) {
			sndPlaySound(TEXT("crunch.wav"), SND_ASYNC);
			X = (float)rand() / RAND_MAX;
			Y = (float)rand() / RAND_MAX;
			drawGrid(X, Y, 0);
			/*cout << "\nfood X: " << X
				<< " Snake X: " << SnakeX << endl;
			cout << "food Y: " << Y
				<< " Snake Y: " << SnakeY << endl;*/
			Score++;
			cout << "\nScore increased!!\nNew Score:" << Score << endl;
		}
		
		// Retraining the food to generate at the ends
		if (X > 0.9f || Y > 0.9f || X < -1.0f || Y < -1.0f) {
			X = (float)rand() / RAND_MAX;
			Y = (float)rand() / RAND_MAX;
			drawGrid(X, Y, 0);
		}
	
	}
	// Deleting the shaderprograms, VAO, VBO and EBO created
	void DelGrid(int fragShaderNo) {
		
			glDeleteVertexArrays(1, &VAO[fragShaderNo]);
			glDeleteBuffers(1, &VBO[fragShaderNo]);
			glDeleteBuffers(1, &EBO[fragShaderNo]);

		if (fragShaderNo == 0)
			glDeleteProgram(ShaderProgramBorder);
		if (fragShaderNo == 1)
			glDeleteProgram(ShaderProgramSnake);
	};
};

int main() {
	
	//Setting the difficulty Level
	cout << "Attention user! You have chosen to play the SNAKE GAME" << endl;
	cout << "USE \n'A' or Left Arrow key to Left up" << endl;
	cout << "'D' or Right Arrow key to move Right" << endl;
	cout << "'W' or Up Arrow key to move up" << endl;
	cout << "'S' or Down Arrow key to Down up" << endl;

	// Asking the level until appropriate values are provided
	int level = 0;
	while (5) {
		cout << "\n\nChoose from difficulty level(1,2,3): " << endl;
		cin >> level;
		if (level > 0 && level < 4)
			break;
	}
	
	// Setting the speed based on the chosen level by the user
	if (level == 1)
		Speed = 1;
	else if (level == 2)
		Speed = 2;
	else if (level == 3)
		Speed = 4;

	// Initaillizing to add GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake Game", NULL, NULL);
	if (window == NULL) {
		cout << "Window failed to open" << endl;
		glfwTerminate();
		return -1;
	}

	// Making the window current 
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//food.initGrid(0.0f, 0.0f);
	Renderer food = Renderer(-0.75f, 0.4f);				// Generating Food
	Renderer Snake = Renderer(-0.7f, 0.0f);				// Generating Snake
	
	srand(time(0));							// Generating random values
	// Rendering the food at the given location
	food.drawGrid((float)rand() / RAND_MAX, (float)rand() / RAND_MAX - 1, 0);		
	// Rendering the snake at the given location
	Snake.drawGrid(0.0f, 0.0f, 1);
	glfwSetKeyCallback(window, KeyBoardCallback);
	double prevTime = glfwGetTime();		// Getting the previous time

	// Making Announce the game is going to start with sound


	ISoundEngine* SoundEngine = createIrrKlangDevice();
	sndPlaySound(TEXT("Get Ready.wav"), SND_ASYNC);			
	//SoundEngine->play2D("audio/Get Ready.wav", true);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glfwPollEvents();
		glClearColor(0.1f, 0.1f, 0.10f, 1.0f);		// Giving color to the window created
		glClear(GL_COLOR_BUFFER_BIT);

		if (Score >= 5)
			Speed = 7;
		Snake.animate(Speed, prevTime, 1);			// Animating the snake with given speed
		
		// Regenerating the food if it is eaten by the snake
		food.makeFood(food.PosX, food.PosY, Snake.GridX, Snake.GridY);		

		food.Render(0);
		Snake.Render(1);

		glfwSwapBuffers(window);

		// Congratulating you if you have WON with music
		if (Score >= MAX_SCORE) {
			glfwSetWindowShouldClose(window, true);
			//sndPlaySound(TEXT("Winner.wav"), SND_ASYNC);
			SoundEngine->play2D("audio/Winner.wav", true);
			cout << "\t\t\t\t\t\tCongratulations You have WON!!!" << endl;
		}

		// Announcing if the GAME is OVER with sound
		if (glfwWindowShouldClose(window) && Score < MAX_SCORE) {
			//sndPlaySound(TEXT("Game Over.wav"), SND_ASYNC);
			cout << "\t\t\t\t\t\t\tGAME OVER!!! \n\t\t\t\t\t\t\tYour Score:" << Score << endl;
			SoundEngine->play2D("audio/Game Over.wav", true);
		}
	Snake.DelGrid(1);				// Deleting the Snake
	}
	food.DelGrid(0);


	Snake.~Renderer();				// Deconstructing Snake
	food.~Renderer();				// Deconstructing food
	glfwDestroyWindow(window);		// Destroying the window
	glfwTerminate();				// Terminating glfw
	return 0;
}

// Taking the keyboard input and changing the direction of the snake
void KeyBoardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//std::cout << key << std::endl;
	if (key == 87 || key == 265) {
		//std::cout << "Up: " << key << std::endl;
		Direction = 1;
	}
	else if (key == 83 || key == 264)
		Direction = 2; 
		//std::cout << "Down: " << key << std::endl;
	else if (key == 65 || key == 263)
		Direction = 3; 
		//std::cout << "Left: " << key << std::endl;
	else if (key == 68 || key == 262)
		Direction = 4; 
		//std::cout << "Right: " << key << std::endl;
}

// Closing the window if ESC is pressed
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}