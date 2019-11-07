
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
#include "vector.h"
#include "matrix.h"
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
bool* loadBuffer;
std::vector<Polygon> polygonList;
std::vector<Polygon> cPolygonList;
void drawLineDDA(std::vector<float> start, std::vector<float> end);
RGB calculateRGB(float y1, float y2, float y3, RGB color1, RGB color2);

/*initialize gl stufff*/
int main(int argc, char **argv)
{
    inputFileName = "testScene.txt";
    readinput(inputFileName, polygonList);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    /*initialize variables, allocate memory, create buffers, etc. */
    //create window of size (win_width x win_height
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //windown title is "glut demo"
    glutCreateWindow("Three Dimentional Drawing");
    
    /*defined glut callback functions*/
    glutDisplayFunc(display); //rendering calls here
    glutReshapeFunc(reshape); //update GL on window size change
    glutMouseFunc(mouse);     //mouse button events
    glutMotionFunc(motion);   //mouse movement events
    glutKeyboardFunc(key);    //Keyboard events
    glutIdleFunc(idle);       //Function called while program is sitting "idle"
    
    //initialize opengl variables
    init();
    //start glut event loop
    glutMainLoop();
    return 0;
}

void init()
{
    //set clear color (Default background to white)
    glClearColor(1.0, 1.0, 1.0, 1.0);
    //glLineWidth(1.0f);
    //checks for OpenGL errors
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, GRID_WIDTH, 0, GRID_HEIGHT, -1, 1);
    check();
}

//called repeatedly when glut isn't doing anything else
void idle(){
    //redraw the scene over and over again
    glutPostRedisplay();
    //    int choice;
    //    lineMode = 't';
    //    std::cout << "1. Rotation \n";
    //    std::cout << "2. Translation\n";
    //    std::cout << "3. Scalling \n";
    //    std::cout << "4. Exit \n";
    //    std::cout << "Please select one of options above for your operation: ";
    //    std::cin>> choice;
    //    switch (choice)
    //    {
    //        case 1:
    //            std::cout << "Please enter rotation angle: ";
    //            std::cin>> angleG;
    //            std::cout << "Please enter rotation Axis (EX. 1 1 1 0 0 0): ";
    //            std::cin >> rotX >> rotY >> rotZ >> ratX >> ratY >> ratZ;
    //            std::cout << "Please enter Polygon ID like 0,1,2,3.. for specific ones: ";
    //            std::cin>> iD;
    //            lineMode = 'r';
    //            break;
    //        case 2:
    //            std::cout << "Please enter translation vector seperated by a space (EX. 2 2 2): ";
    //            std::cin>> translationXG >> translationYG >> translationZG;
    //            std::cout << "Please enter Polygon ID like 0,1,2,3.. for specific ones: ";
    //            std::cin>> iD;
    //            lineMode = 't';
    //            break;
    //        case 3:
    //            std::cout << "Please enter scalling factor: " ;
    //            std::cin>> scaleG;
    //            std::cout << "Please enter Polygon ID like 0,1,2,3.. for specific ones: ";
    //            std::cin>> iD;
    //            lineMode = 's';
    //            break;
    //        case 4:
    //            writeFile(inputFileName, polygonList);
    //            exit(0);
    //            break;
    //        default:
    //            break;
    //}
}

void readinput(char *filename, std::vector<Polygon> &polygons){
    std::ifstream inputFile;
    inputFile.open(filename);
    std::string line;
    int count;
    int faceCount;
    inputFile >> count;
    getline(inputFile, line); //line 1
    getline(inputFile, line); //point count
    for (int i=0; i< count; i++){
        int num;
        std::vector <vec3f> vertices;
        std::vector <Point> points;
        std::vector <Face> faces;
        std::vector <RGB> colors;
        std::vector<float> specs;
        inputFile >> num;
        std::vector<float> specsfile;
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
            vec3f point(x,y,z);
            vertices.push_back(point);
        }
        //reading vertex COLORS
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
            RGB rgb(x,y,z);
            colors.push_back(rgb);
        }
        std :: string input;
        getline(inputFile, line);
        faceCount = std::stoi(line);
        for (int j=0; j<faceCount; j++){
            int x, y,z;
            std :: string inputX, inputY, inputZ;
            getline(inputFile, line);
            std :: istringstream record(line);
            getline(record, inputX, ' ');
            getline(record, inputY, ' ');
            getline(record, inputZ);
            x = std::stoi(inputX);
            y = std::stoi(inputY);
            z = std::stoi(inputZ);
            Face face(x,y,z);
            faces.push_back(face);
        }
        for (int j=0; j<faceCount; j++){
            float x;
            std :: string inputX;
            getline(inputFile, line);
            std :: istringstream record(line);
            getline(record, inputX);
            x = std::stof(inputX);
            specsfile.push_back(x);
        }
        for(int i = 0; i<vertices.size();i++){
            Point temp(vertices[i],colors[i]);
            points.push_back(temp);
        }
        Polygon polygon(points,faces,specsfile);
        polygon.printPolygon();
        polygons.push_back(polygon);
        getline(inputFile, line);
    }
    inputFile.close();
}
// void writeFile(char *filename,std::vector<Polygon> &polygons){
//     std::ofstream outputFile(filename);
//     outputFile << polygons.size() << "\n\n";
//     for (int i = 0; i<polygons.size();i++) {
//         outputFile << polygons[i].count << std::endl;
//         for(int j = 0; j<polygons[i].count;j++) {
//             outputFile << polygons[i].vertices[j][0] << ' ' << polygons[i].vertices[j][1] << ' '<< polygons[i].vertices[j][2]<< std::endl;
//         }
//         outputFile << polygons[i].edgeCount<< std::endl;
//         for(int k = 0; k<polygons[i].edgeCount;k++) {
//             outputFile << polygons[i].edges[k][0] << ' ' << polygons[i].edges[k][1]<<std::endl;
//         }
//         outputFile << std::endl;
//     }
// }

void display()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    //drawSplitLines();
    
    glutSwapBuffers();
    //checks for opengl errors
    check();
    // polygonList.clear();
    // for(int u=0;u<cPolygonList.size();u++){
    //     cPolygonList[u].updateCentroid();
    //     polygonList.push_back(cPolygonList[u]);
    // }
    //writeFile(inputFileName, polygonList);
}
void draw_pix(int x, int y)
{
    glBegin(GL_POINTS);
    glColor3f(0.41, 0.4, 0.4);
    glVertex3f(x + .5, y + .5, 0);
    glEnd();
}

void drawLineDDA(std::vector<float> start, std::vector<float> end)
{
    int startX = (int)(start[0] + 0.5);
    int startY = (int)(start[1] + 0.5);
    int endX = (int)(end[0] + 0.5);
    int endY = (int)(end[1] + 0.5);
    
    int dX = endX - startX;
    int dY = endY - startY;
    int steps, k;
    float incX, incY;
    float x = (float)startX;
    float y = (float)startY;
    
    if(fabs(dX) > fabs(dY)) {
        steps = fabs(dX);
    } else {
        steps = fabs(dY);
    }
    incX = (float)dX / (float)steps;
    incY = (float)dY / (float)steps;
    
    loadBuffer[(int)(x + GRID_WIDTH * y + 0.5)] = true;
    for(int i = 0; i < steps; i++) {
        x += incX;
        y += incY;
        int roundX = (int)(x+0.5);
        int roundY = (int)(y+0.5);
        loadBuffer[roundX + GRID_WIDTH * roundY] = true;
    }
}
/*Gets called when display size changes, including initial craetion of the display*/
void reshape(int width, int height)
{
    /*set up projection matrix to define the view port*/
    //update the ne window width and height
    
    //creates a rendering area across the window
    glViewport(0, 0, width, height);
    // up an orthogonal projection matrix so that
    // the pixel space is mapped to the grid space
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, GRID_WIDTH, 0, GRID_HEIGHT, -10, 10);
    
    //clear the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //set pixel size based on width, if the aspect ratio
    //changes this hack won't work as well
    //PIXEL_SIZE = width / (float)GRID_WIDTH;
    
    //set pixel size relative to the grid cell size
    glPointSize(PIXEL_SIZE);
    //check for opengl errors
    check();
}

//gets called when a key is pressed on the keyboard
void key(unsigned char ch, int x, int y)
{
    switch (ch)
    {

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
    printf("MOUSE AT PIXEL: %d %d, GRID: %d %d\n", x, y, (int)(x / PIXEL_SIZE), (int)((WINDOW_HEIGHT - y) / PIXEL_SIZE));
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

RGB calculateRGB(float y1, float y2, float y3, RGB color1, RGB color2)
{
    float a = (y3 - y2)/(y1 - y2);
    float b = (y1 - y3)/(y1 - y2);
    float c0,c1,c2;
    c0 = a*color1.c0 + b*color2.c0;
    c1 = a*color1.c1 + b*color2.c1;
    c2 = a*color1.c2 + b*color2.c2;
    RGB temp(c0,c1,c2);
    return temp;
}



