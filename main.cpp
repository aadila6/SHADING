
#ifdef WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "tk.h"
#endif

#if defined(__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#else //linux
#include <GL/gl.h>
#include <GL/glut.h>
#endif

//other includes
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
//other files
#include "Geometry.h"

char *inputFileName;
constexpr int PIXEL_SIZE = 3;
constexpr int GRID_WIDTH= 200;
constexpr int GRID_HEIGHT = 200;
constexpr int WINDOW_HEIGHT = GRID_HEIGHT*PIXEL_SIZE;
constexpr int WINDOW_WIDTH= GRID_WIDTH*PIXEL_SIZE;

void init();
void idle();
void display();
void draw_pix(int x, int y);
void draw();
void reshape(int width, int height);
void key(unsigned char ch, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void check();
void readinput(char *filename, std::vector<Polygon> &polygons);
void writeFile(char *filename, std::vector<Polygon> &polygons);
void rasterization(Polygon &p);
/*initialize gl stufff*/
void init()
{
    //set clear color (Default background to white)
    glClearColor(1.0, 1.0, 1.0, 1.0);
    //glLineWidth(1.0f);
    //checks for OpenGL errors
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, grid_width, 0, grid_height, -1, 1);
    check();
}
void resetConstrains(){
    iD=-1;
    translationXG = 0;
    translationYG = 0;
    translationZG = 0;
    rotX = 0;
    rotY = 0;
    rotZ = 0;
    ratX = 0;
    ratY = 0;
    ratZ = 0;
    angleG = 0;
    scaleG = 1;
}

//called repeatedly when glut isn't doing anything else
void idle(){
    //redraw the scene over and over again
    glutPostRedisplay();
    resetConstrains();
    int choice;
    lineMode = 't';
    std::cout << "1. Rotation \n";
    std::cout << "2. Translation\n";
    std::cout << "3. Scalling \n";
    std::cout << "4. Exit \n";
    std::cout << "Please select one of options above for your operation: ";
    std::cin>> choice;
    switch (choice)
    {
        case 1:
            std::cout << "Please enter rotation angle: ";
            std::cin>> angleG;
            std::cout << "Please enter rotation Axis (EX. 1 1 1 0 0 0): ";
            std::cin >> rotX >> rotY >> rotZ >> ratX >> ratY >> ratZ;
            std::cout << "Please enter Polygon ID like 0,1,2,3.. for specific ones: ";
            std::cin>> iD;
            lineMode = 'r';
            break;
        case 2:
            std::cout << "Please enter translation vector seperated by a space (EX. 2 2 2): ";
            std::cin>> translationXG >> translationYG >> translationZG;
            std::cout << "Please enter Polygon ID like 0,1,2,3.. for specific ones: ";
            std::cin>> iD;
            lineMode = 't';
            break;
        case 3:
            std::cout << "Please enter scalling factor: " ;
            std::cin>> scaleG;
            std::cout << "Please enter Polygon ID like 0,1,2,3.. for specific ones: ";
            std::cin>> iD;
            lineMode = 's';
            break;
        case 4:
            writeFile(inputFileName, polygonList);
            exit(0);
            break;
        default:
            break;
}
}
void readinput(char *filename, std::vector<Polygon> &polygons){
    std::ifstream inputFile;
    inputFile.open(filename);
    std::string line;
    int count;
    int edgeCount;
    inputFile >> count;
    getline(inputFile, line); //line 1
    getline(inputFile, line); //point count
    for (int i=0; i< count; i++){
        int num;
        std::vector <Coord> vertices;
        std::vector <ECoord> edges;
        inputFile >> num;
        getline(inputFile, line);
        for (int j=0; j<num; j++){
            float x, y, z;
            std::string inputX, inputY, inputZ;
            getline(inputFile, line);
            std :: istringstream record(line);
            getline(record, inputX, ' ');
            getline(record, inputY, ' ');
            getline(record, inputZ);
            x = std::stof(inputX);
            y = std::stof(inputY);
            z = std::stof(inputZ);
            Coord point;
            point.x = x;
            point.y = y;
            point.z = z;
            vertices.push_back(point);
        }
        std :: string input;
        getline(inputFile, line);
        edgeCount = std::stoi(line);
        for (int j=0; j<edgeCount; j++){
            int x, y;
            std :: string inputX, inputY;
            getline(inputFile, line);
            std :: istringstream record(line);
            getline(record, inputX, ' ');
            getline(record, inputY);
            x = std::stoi(inputX);
            y = std::stoi(inputY);
            ECoord point;
            point.x = x;
            point.y = y;
            edges.push_back(point);
        }
        Polygon polygon(vertices, edges);
        polygons.push_back(polygon);
        getline(inputFile, line);
    }
    inputFile.close();
}
void writeFile(char *filename,std::vector<Polygon> &polygons){
    std::ofstream outputFile(filename);
    outputFile << polygons.size() << "\n\n";
    for (int i = 0; i<polygons.size();i++) {
        outputFile << polygons[i].count << std::endl;
        for(int j = 0; j<polygons[i].count;j++) {
            outputFile << polygons[i].vertices[j][0] << ' ' << polygons[i].vertices[j][1] << ' '<< polygons[i].vertices[j][2]<< std::endl;
        }
        outputFile << polygons[i].edgeCount<< std::endl;
        for(int k = 0; k<polygons[i].edgeCount;k++) {
            outputFile << polygons[i].edges[k][0] << ' ' << polygons[i].edges[k][1]<<std::endl;
        }
        outputFile << std::endl;
    }
}

void display()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    //find xMax mins.
    drawSplitLines();

    glutSwapBuffers();
    //checks for opengl errors
    check();
    polygonList.clear();
    for(int u=0;u<cPolygonList.size();u++){
        cPolygonList[u].updateCentroid();
        polygonList.push_back(cPolygonList[u]);
    }
    writeFile(inputFileName, polygonList);
}
void draw_pix(int x, int y)
{
    glBegin(GL_POINTS);
    glColor3f(0.41, 0.4, 0.4);
    glVertex3f(x + .5, y + .5, 0);
    glEnd();
}

/*Gets called when display size changes, including initial craetion of the display*/
void reshape(int width, int height)
{
    /*set up projection matrix to define the view port*/
    //update the ne window width and height
    win_width = width;
    win_height = height;
    
    //creates a rendering area across the window
    glViewport(0, 0, width, height);
    // up an orthogonal projection matrix so that
    // the pixel space is mapped to the grid space
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, grid_width, 0, grid_height, -10, 10);
    
    //clear the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //set pixel size based on width, if the aspect ratio
    //changes this hack won't work as well
    pixel_size = width / (float)grid_width;
    
    //set pixel size relative to the grid cell size
    glPointSize(pixel_size);
    //check for opengl errors
    check();
}

//gets called when a key is pressed on the keyboard
void key(unsigned char ch, int x, int y)
{
    switch (ch)
    {
        case 'b':
            lineMode = 'b';
            break;
            
        case 'd':
            lineMode = 'd';
            break;
            
        default:
            //prints out which key the user hit
            printf("User hit the \"%c\" key\n", ch);
            break;
    }
    //redraw the scene after keyboard input
    glutPostRedisplay();
}


//gets called when a mouse button is pressed
void mouse(int button, int state, int x, int y)
{
    //print the pixel location, and the grid location
    printf("MOUSE AT PIXEL: %d %d, GRID: %d %d\n", x, y, (int)(x / pixel_size), (int)((win_height - y) / pixel_size));
    switch (button)
    {
        case GLUT_LEFT_BUTTON: //left button
            printf("LEFT ");
            break;
        case GLUT_RIGHT_BUTTON: //right button
            printf("RIGHT ");
        default:
            printf("UNKNOWN "); //any other mouse button
            break;
    }
    if (state != GLUT_DOWN) //button released
        printf("BUTTON UP\n");
    else
        printf("BUTTON DOWN\n"); //button clicked
    
    //redraw the scene after mouse click
    glutPostRedisplay();
}

//gets called when the curser moves accross the scene
void motion(int x, int y)
{
    //redraw the scene after mouse movement
    glutPostRedisplay();
}

//checks for any opengl errors in the previous calls and
//outputs if they are present
void check()
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("GLERROR: There was an error %s\n", "error");
        exit(1);
    }
}

