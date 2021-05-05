// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>

using namespace std;
// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic.
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
#include "shader/shader.hpp"
#include "gol/gol.cpp"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == 	GLFW_KEY_TAB && action == GLFW_PRESS){
        fprintf( stdout, "NO\n" );
    }
    else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS){
        fprintf( stdout, "YES\n" );
    }

}

int load_board_size(ifstream& file_stream){
  string line;
  getline(file_stream, line);
  int board_size = stoi(line);
  return board_size;
}

int load_coord_size(ifstream& file_stream){
  string line;
  getline(file_stream, line);
  int coord_size = stoi(line)-1;
  return 2*coord_size;
}

void load_config(ifstream& file_stream, int* board_coord, int coord_size){
    string line;
    getline(file_stream, line);
    for(int i=0; i<coord_size; i=i+2){
        file_stream >> board_coord[i] >> board_coord[i+1];
    }
}

bool file_exists (string name) {
    ifstream f(name);
    return f.good();
}

int main(){
    // Load the configuration file
    string file;
    cout << "Enter configuration file: \n";
    getline(cin, file);
    if (! file_exists(file)){
      printf("File does not exist, using the default configuration\n");
      file = "config/config.txt";
    }
    ifstream infile(file);
    int board_size = load_board_size(infile);
    int coord_size = load_coord_size(infile);
    int board_coord[coord_size];
    load_config(infile, board_coord, coord_size);

    // Create game of life instance
    Gol game (board_size, coord_size, board_coord);

    // Initialise GLFW
    glewExperimental = true; // Needed for core profile
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    // Open a window using GLFW
    GLFWwindow* window;
    window = glfwCreateWindow(512, 512, "Conway's Game of Life", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    // Create its OpenGL context
    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // create a Vertex Array Object and set it as the current one
    // Do this once your window is created and before any other OpenGL call
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Define the square position to fill the entire screen
    const int num_square = board_size*board_size;
    float square_size = 2.0/float(board_size);
    float square[num_square][4];
    for(int i=0; i<board_size; i++){
        float x1 = i*square_size-1;
        float x2 = x1+square_size;
        for(int j=0; j<board_size; j++){
            float y1 = 1-j*square_size;
            float y2 = y1-square_size;
            square[j*board_size+i][0] = x1;
            square[j*board_size+i][1] = y1;
            square[j*board_size+i][2] = x2;
            square[j*board_size+i][3] = y2;
        }
    }

    // Map the square position to vertices
    // Need 6 vertices for one square
    // Each vertex has 3 position => 3*6 = 18 values
    GLfloat* g_vertex_buffer_data = new GLfloat[num_square*18];

    for(int i=0; i<num_square; i++){
        g_vertex_buffer_data[18*i] = square[i][0];
        g_vertex_buffer_data[18*i+1] = square[i][1];
        g_vertex_buffer_data[18*i+2] = 0.0f;

        g_vertex_buffer_data[18*i+3] = square[i][0];
        g_vertex_buffer_data[18*i+4] = square[i][3];
        g_vertex_buffer_data[18*i+5] = 0.0f;

        g_vertex_buffer_data[18*i+6] = square[i][2];
        g_vertex_buffer_data[18*i+7] = square[i][1];
        g_vertex_buffer_data[18*i+8] = 0.0f;

        g_vertex_buffer_data[18*i+9] = square[i][2];
        g_vertex_buffer_data[18*i+10] = square[i][1];
        g_vertex_buffer_data[18*i+11] = 0.0f;

        g_vertex_buffer_data[18*i+12] = square[i][0];
        g_vertex_buffer_data[18*i+13] = square[i][3];
        g_vertex_buffer_data[18*i+14] = 0.0f;

        g_vertex_buffer_data[18*i+15] = square[i][2];
        g_vertex_buffer_data[18*i+16] = square[i][3];
        g_vertex_buffer_data[18*i+17] = 0.0f;
    }

    // Color the squares with respect to the
    GLfloat* g_color_buffer_data = new GLfloat[num_square*18];
    for(int i=0; i<board_size; i++){
        for(int j=0; j<board_size; j++){
            float c;
            if(game.new_board[i][j]==true){
                c = 1.0f;
                for(int k=0; k<18; k++){
                    g_color_buffer_data[18*(i*board_size+j)+k] = c;
                }
            } else {
                c = 0.0f;
                for(int k=0; k<18; k++){
                    g_color_buffer_data[18*(i*board_size+j)+k] = c;
                }
            }
        }
    }


    // Define the vertex buffer and transfer data to OpenGL
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, num_square*18*sizeof(g_vertex_buffer_data[0]), g_vertex_buffer_data, GL_STATIC_DRAW);

    // Define the color buffer and transfer data to OpenGL
    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, num_square*18*sizeof(g_color_buffer_data[0]), g_color_buffer_data, GL_STATIC_DRAW);

    // Create and compile our GLSL program from the shaders
    char* vertex_shader = "shader/TransformVertexShader.vertexshader";
    char* fragment_shader = "shader/ColorFragmentShader.fragmentshader";
    GLuint programID = LoadShaders( vertex_shader , fragment_shader );

    do{
        // Clear the screen
        glfwSetKeyCallback(window, key_callback);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update color buffer
        for(int i=0; i<board_size; i++){
            for(int j=0; j<board_size; j++){
                float c;
                if(game.new_board[i][j]==true){
                    c = 1.0f;
                    for(int k=0; k<18; k++){
                        g_color_buffer_data[18*(i*board_size+j)+k] = c;
                    }
                } else {
                    c = 0.0f;
                    for(int k=0; k<18; k++){
                        g_color_buffer_data[18*(i*board_size+j)+k] = c;
                    }
                }
            }
        }

        glBufferData(GL_ARRAY_BUFFER, num_square*18*sizeof(g_color_buffer_data[0]), g_color_buffer_data, GL_STATIC_DRAW);

        // Use our shader
        glUseProgram(programID);

        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (const GLvoid *)0            // array buffer offset
        );

        // 2nd attribute buffer : colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, num_square*2*3);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        // update the game
        game.update_boards();
        usleep(50000);

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0 );


    // Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}
