#define GL_SILENCE_DEPRECATION
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
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <algorithm>
//other files
#include "Geometry.h"
#include "vector.h"
#include "matrix.h"
using namespace std;
char *inputFileName;
constexpr int PIXEL_SIZE = 1;
constexpr int GRID_WIDTH= 800;
constexpr int GRID_HEIGHT = 800;
constexpr int WINDOW_HEIGHT = GRID_HEIGHT*PIXEL_SIZE;
constexpr int WINDOW_WIDTH= GRID_WIDTH*PIXEL_SIZE;
constexpr float SCALE= GRID_WIDTH*.5;
bool phongSwitch;
bool htSwitch;
void init();
void idle();
void display();
void draw_pix(int x, int y, RGB color);
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
void GShading(Point v1, Point v2, Point v3);
MegaPixel halfToning(Point x);
void GShadHalfToningOn(Point a, Point b, Point c);
void drawMegaPixel(Point point);
void goShading(Point a, Point b, Point c, char mode, vec3f fn, float spe);
Point swapCoordsyz(Point &v1);
Point swapCoordsxz(Point &v1);
void goroShading(Point p1, Point p2, Point p3,vec3f fn, float spect, char mode);
int ddaIntercepts(int yvalue, Point start, Point end);
void setGlobalValues();
void initGlobalValues();
char fileG;
//
vec3f lightSource(10,10,100);
vec3f frontPoint(10,10,10);
//vec3f lightSource(0,100,100);
//vec3f frontPoint(-10,10,10);
RGB ambient(0,0,0);
/*initialize gl stufff*/
float a,b,c,d,e,f,g,h,i,j;
int main(int argc, char **argv)
{
    inputFileName = "bunny.txt";
    readinput(inputFileName, polygonList);
    initGlobalValues();
    fileG= 'c';
    phongSwitch = false;
    htSwitch = false;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
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
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glLineWidth(1.0f);
    //checks for OpenGL errors
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, GRID_WIDTH, 0, GRID_HEIGHT, -1, 1);
    check();
}
void initGlobalValues(){
    a = 0;
    b = 0;
    c = 0;
    d = 0;
    e = 0;
    f = 0;
    g = 0;
    h = 0;
    i = 0;
}
void setGlobalValues(){
    ambient.r = a;
    ambient.g = b;
    ambient.b = c;
    frontPoint.x() = d;
    frontPoint.y() = e;
    frontPoint.z() = f;
    lightSource.x() = g;
    lightSource.y() = h;
    lightSource.z() = i;
    
}
//called repeatedly when glut isn't doing anything else
void idle(){
    //redraw the scene over and over again
    glutPostRedisplay();
    char res = 't';
    std::cout << "Which file would you like to read and see?  \n";
    std::cout << "Cube_and_icosahedron file, Enter 'c' \n";
    std::cout << "Bunny file, Enter 'b'\n";
    std::cout << "Your choice is:  ";
    std::cin>> res;
    if(res == 'b'){
        fileG = 'b';
    }else{
        fileG = 'c';
    }
        int choice;
        std::cout << "1. Flat Shading \n";
        std::cout << "2. Phong + Shading\n";
        std::cout << "3. HalfToning + Shading\n";
        std::cout << "4. Exit \n";
        std::cout << "Please select one of options above for your operation: ";
        std::cin>> choice;
        switch (choice)
        {
            case 1:
            htSwitch = false;
            phongSwitch = false;
            break;
                
            case 2:
                htSwitch = false;
                phongSwitch = true;
                std::cout << "Please enter Ambiet(K*I) (EX. 0.5 0.5 0.5): ";
                std::cin >> a >> b >> c;
                std::cout << "Please enter your front Point(f) (EX. 10 10 10): ";
                std::cin >> d >> e >> f;
                std::cout << "Please enter your light source point (EX. 10 10 10): ";
                std::cin >> g >> h >> i;
                setGlobalValues();
                break;
            case 3:
                htSwitch = true;
                phongSwitch = false;
                break;
            case 4:
                exit(0);
                break;
            default:
                break;
    }
}

void readinput(char *filename, std::vector<Polygon> &polygons){
    std::ifstream inputFile;
    //inputFile.open(filename);
    inputFile.open(filename);
    if (!inputFile) {
        std::cout << "could not open" << std::endl;
        exit(0);
    }
    
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
            vec3f point(x*GRID_WIDTH,y*GRID_WIDTH,z*GRID_WIDTH);
            vertices.push_back(point);
        }
        //reading vertex COLORSs
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
            RGB rgb(x/255,y/255,z/255);
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
        polygons.push_back(polygon);
        getline(inputFile, line);
    }
    inputFile.close();
}


void drawMegaPixel(Point lol){
    //Point ht(vec3f(30,30,30),RGB(0,200,200));
    MegaPixel pix = halfToning(lol);
    int x = lol.point.x();
    int y = lol.point.y();
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(pix.mpixel[i][j] == 'r'){
                RGB drawColor(1,0,0);
                draw_pix(x*3+j,y*3+i,drawColor);
            }else if(pix.mpixel[i][j] == 'g'){
                RGB drawColor(0,1,0);
                draw_pix(x*3+j,y*3+i,drawColor);
            }else if(pix.mpixel[i][j] == 'b'){
                RGB drawColor(0,0,1);
                draw_pix(x*3+j,y*3+i,drawColor);
            }
        }
    }
}
void drawSplitLines(){
    glColor3f(0.41, 0.4, 0.4);
    glBegin(GL_LINES);
    glVertex2f(0.5f*GRID_WIDTH,0.0f);
    glVertex2f(0.5f*GRID_WIDTH, 1.0f*GRID_HEIGHT);
    glVertex2f(0.0f*GRID_WIDTH, 0.5f*GRID_HEIGHT);
    glVertex2f(1.0f*GRID_WIDTH, 0.5f*GRID_HEIGHT);
    glEnd();
}
void displayLight(){
    draw_pix(lightSource.x()*.5, lightSource.y()*.5, RGB(0,1,0));
    draw_pix((lightSource.x()+1)*.5, (lightSource.y()+1)*.5, RGB(0,1,0));
    draw_pix(lightSource.y()*.5, lightSource.z()*0.5+.5*WINDOW_HEIGHT, RGB(1,1,1));
    draw_pix((lightSource.x()+1)*.5, lightSource.z()*+.5*0.5*WINDOW_HEIGHT, RGB(1,1,1));
    draw_pix(lightSource.x()*.5+.5*WINDOW_WIDTH, lightSource.z()*0.5+.5*WINDOW_HEIGHT, RGB(1,1,1));
    draw_pix((lightSource.x()+1)*.5+.5*WINDOW_WIDTH, lightSource.z()*0.5+.5*WINDOW_HEIGHT, RGB(1,1,1));
    
}

void display()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    drawSplitLines();
    polygonList.clear();
    
    if(fileG == 'b'){
         inputFileName = "bunny.txt";
    }else if (fileG =='c'){
         inputFileName = "cia.txt";
    }
    readinput(inputFileName, polygonList);

    for(int id=0;id<polygonList.size();id++){
        //Apply painter's algorithm
        vector<int> sortedXY;
        sortedXY.clear();
        sortedXY = polygonList[id].sortZ();
        
        vector<int> sortedXZ;
        sortedXZ.clear();
        sortedXZ = polygonList[id].sortY();
        
        vector<int> sortedYZ;
        sortedYZ.clear();
        sortedYZ = polygonList[id].sortX();
        
       
        //do with temp
        for(int i = 0; i<polygonList[id].faces.size(); i++){
           goroShading(polygonList[id].vertices[polygonList[id].faces[sortedYZ[i]].f0-1],
                        polygonList[id].vertices[polygonList[id].faces[sortedYZ[i]].f1-1],
                        polygonList[id].vertices[polygonList[id].faces[sortedYZ[i]].f2-1],
                        polygonList[id].faces[i].normal,polygonList[id].faces[id].spectNum, 'z');
            goroShading(polygonList[id].vertices[polygonList[id].faces[sortedXZ[i]].f0-1],
                        polygonList[id].vertices[polygonList[id].faces[sortedXZ[i]].f1-1],
                        polygonList[id].vertices[polygonList[id].faces[sortedXZ[i]].f2-1],
                        polygonList[id].faces[i].normal,polygonList[id].faces[id].spectNum, 'y');
            goroShading(polygonList[id].vertices[polygonList[id].faces[sortedXY[i]].f0-1],
                        polygonList[id].vertices[polygonList[id].faces[sortedXY[i]].f1-1],
                        polygonList[id].vertices[polygonList[id].faces[sortedXY[i]].f2-1],
                        polygonList[id].faces[i].normal,polygonList[id].faces[id].spectNum, 'x');
        }
    }
    glutSwapBuffers();
    initGlobalValues();
    //glFlush();
    check();
}

void draw_pix(int x, int y, RGB color)
{
    glBegin(GL_POINTS);
    //glColor3f(0.41, 0.4, 0.4);
    glColor3f(color.r, color.g, color.b);
    glVertex3f(x + .5, y + .5, 0);
    glEnd();
}

//will sort all vec2fs and return in order of v1,v2,v3
void sortYlocation(Point &a, Point &b,Point &c){
    if(a.point.y()<b.point.y()){
        Point tmp(a.point,a.intensity);
        a = b;
        b = tmp;
    }
    if(b.point.y()<c.point.y()){
        Point tmp(b.point,b.intensity);
        b=c;
        c = tmp;
    }
    if(a.point.y()<c.point.y()){
        Point tmp(a.point,a.intensity);
        a=c;
        c=tmp;
    }
    if(a.point.y()<b.point.y()){
        Point tmp(a.point,a.intensity);
        a = b;
        b = tmp;
    }
    if(b.point.y()<c.point.y()){
        Point tmp(b.point,b.intensity);
        b=c;
        c = tmp;
    }
    if(a.point.y()<c.point.y()){
        Point tmp(a.point,a.intensity);
        a=c;
        c=tmp;
    }
    
}
Point swapCoordsyz(Point &v1){
    return(Point(vec3f(v1.point.y(),v1.point.z(),v1.point.x()), v1.intensity));
}
Point swapCoordsxz(Point &v1){
    return(Point(vec3f(v1.point.x(),v1.point.z(),v1.point.y()), v1.intensity));
}

int rdf(float x){
    return (int)(x+0.5);
}


void GShadHalfToningOn(Point a, Point b, Point c){
    Point v1 = a;
    Point v2 = b;
    Point v3 = c;
    sortYlocation(v1,v2,v3);
    //sortYlocation(v1, v2, v3);
    int dX1 = (int)(v2.point.x() - v1.point.x());
    int dX2 = (int)(v3.point.x() - v1.point.x());
    int dY1 = (int)(v2.point.y() - v1.point.y());
    int dY2 = (int)(v3.point.y() - v2.point.y());//will use this later
    int dY3 = (int)(v3.point.y() - v1.point.y());
    float incX1, incX2;
    float x1 = (float)v1.point.x();
    float x2 = (float)v1.point.x();
    float y = (float)v1.point.y();
    int steps = fabs(dY1);
    incX1 = (float)dX1 / (float)steps;
    incX2 = (float)dX2 / (float)fabs(dY3);
    RGB midstore(0,0,0);
    //from p1 - p2 && p1 - p3
    for(int i = 0; i < steps; i++) {
        x1 += incX1;
        x2 += incX2;
        y -= 1;
        //Assume I have the intensity from PHONG
        RGB I1 = calculateRGB(v1.point.y(),v2.point.y(),y,v1.intensity,v2.intensity);
        RGB I2 = calculateRGB(v1.point.y(),v3.point.y(),y,v1.intensity,v3.intensity);
        Point la(vec3f(x1,y,0),I1);
        drawMegaPixel(la);
        Point lo(vec3f(x2,y,0),I2);
        drawMegaPixel(lo);
        for(int j = 1; j<= x2 - x1; j++){
            RGB Ibetween = calculateRGB(x1,x2,x1+j,I1,I2);
            //RGB Ibetween = calIntense(I1,I2,x1+j,x1,x2);
            Point lala(vec3f(x1+j,y,0),Ibetween);
            drawMegaPixel(lala);
            //draw_pix(x1+j,y,Ibetween);
        }
        if(i == steps -1){
            midstore = I2;
        }
    }
    float midX = x2;
    float midY = v2.point.y();
    dX1 = (int)(v3.point.x() - v2.point.x());
    //dX2 = (int)(v3.point.x() - midX);
    steps = fabs(dY2);
    incX1 = (float)dX1 / (float)steps;
    //incX2 = (float)dX2 / (float)steps;
    x1 = (float)v2.point.x();
    x2 = midX;
    //y = midY;
    for(int i = 0; i < steps; i++) {
        x1 += incX1;
        x2 += incX2;
        y -= 1;
        //Assume I have the intensity from PHONG
        RGB I1 = calculateRGB(v2.point.y(),v3.point.y(),y,v2.intensity,v3.intensity);
        RGB I2 = calculateRGB(midY,v3.point.y(),y,midstore,v3.intensity);
        //RGB lala(0, 0, 255);
        Point la(vec3f(x1,y,0),I1);
        drawMegaPixel(la);
        Point lo(vec3f(x2,y,0),I2);
        drawMegaPixel(lo);
        for(int j = 1; j<= x2 - x1; j++){
            RGB Ibetween = calculateRGB(x1,x2,x1+j,I1,I2);
            //RGB Ibetween = calIntense(I1,I2,x1+j,x1,x2);
            //draw_pix(x1+j,y,Ibetween);
            Point lala(vec3f(x1+j,y,0),Ibetween);
            drawMegaPixel(lala);
        }
    }
}
int findOn(float max){
    int on = 0;
    if(max<0.5){
        if(max >= 0.1 && max < 0.2){
            on = 1;
        }else if(max >= 0.2 && max < 0.3){
            on = 2;
        }else if(max >= 0.3 && max < 0.4){
            on = 3;
        }else{
            on = 4;
        }
    }else{
        if(max >= 0.5 && max < 0.6){
            on = 5;
        }else if(max >= 0.6 && max < 0.7){
            on = 6;
        }else if(max >= 0.7 && max < 0.8){
            on = 7;
        }else if(max >= 0.8 && max < 0.9){
            on = 8;
        }else{
            on = 9;
        }
    }
    return on;
}
//find the max of rgb and devide each rgb value by max;
MegaPixel halfToning(Point x){
    float r = x.intensity.r/255;
    float g = x.intensity.g/255;
    float b = x.intensity.b/255;
    float max = fmax(r,fmax(g,b));
    int on = findOn(max);
    int numRed = rdf(r*on /(r+g+b));
    int numGreen = rdf(g*on /(r+g+b));
    int numBlue = rdf(b*on /(r+g+b));
    
    std::vector<char> array;
    for(int i =0;i<numRed;i++){
        array.push_back('r');
    }
    for(int i =0;i<numGreen;i++){
        array.push_back('g');
    }
    for(int i =0;i<numBlue;i++){
        array.push_back('b');
    }
    
    MegaPixel pixel;
    int taken = 0;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(taken<array.size()){
                pixel.mpixel[i][j] = array[taken];
                taken++;
            }
        }
    }
    return pixel;
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
        case 'p':
            phongSwitch = !phongSwitch;
//        case 'h':
//            htSwitch = !htSwitch;
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
    c0 = a*color1.r + b*color2.r;
    c1 = a*color1.g + b*color2.g;
    c2 = a*color1.b + b*color2.b;
    RGB temp(c0,c1,c2);
    return temp;
}



int ddaIntercepts(int yvalue, Point start, Point end)
{
    //check if it intercepts
    if(yvalue > rdf(max(start.point.y(), end.point.y())) || yvalue < rdf(min(start.point.y(), end.point.y())))
        return -1;
    //DDA to find interception
    float dx = end.point.x() - start.point.x(), dy = end.point.y() - start.point.y(), steps;
    float xIncrement, yIncrement, x = start.point.x(), y = start.point.y();
    
    if (fabs(dx) > fabs(dy))
        steps = fabs(dx);
    else
        steps = fabs(dy);
    xIncrement = dx / steps;
    yIncrement = dy / steps;
    while (rdf(y) != yvalue)
    {
        x += xIncrement;
        y += yIncrement;
    }
    return rdf(x);
}

void goroShading(Point p1, Point p2, Point p3,vec3f fn, float spect, char mode){
    if(mode == 'y'){
        p1 = swapCoordsyz(p1);
        p2 = swapCoordsyz(p2);
        p3 = swapCoordsyz(p3);
    }else if(mode == 'z'){
        p1 = swapCoordsxz(p1);
        p2 = swapCoordsxz(p2);
        p3 = swapCoordsxz(p3);
    }
    int count = 0;
    int ymax, ymin;
    std::vector<int> records;
    std::vector<RGB> rgbs;
    ymax = rdf(max(max(p1.point.y(),p2.point.y()),p3.point.y()));
    ymin = rdf(min(min(p1.point.y(),p2.point.y()),p3.point.y()));
    Point r1 = p1;
    Point r2 = p2;
    Point r3 = p3;
    r1.point = r1.point / GRID_WIDTH;
    r2.point = r2.point / GRID_WIDTH;
    r3.point = r3.point / GRID_WIDTH;
    
    PhongModel one(r1,fn,frontPoint, lightSource,spect,ambient);
    PhongModel two(r2,fn,frontPoint, lightSource,spect,ambient);
    PhongModel three(r3,fn,frontPoint, lightSource,spect,ambient);
    int lol = 0;
    for(int i=ymin; i<ymax; i++){
        int x = 0;
        x = ddaIntercepts(i, p1, p2);
        RGB temp = calculateRGB(p1.point.y(),p2.point.y(),i,p1.intensity,p2.intensity);
        if(phongSwitch){
            temp = calculateRGB(p1.point.y(),p2.point.y(),i,one.phongrgb,two.phongrgb);
        }
        if(x!= -1){
            bool found = false;
            for(int k = 0; k<records.size();k++){
                if(records[k] == x){
                    found = true;
                    break;
                }
            }
            if(!found){
                int h = 0;
                for(int h = 0; h<records.size();h++){
                    if(x<records[h]){
                        break;
                    }
                }
                records.insert(records.begin()+h,x);
                rgbs.insert(rgbs.begin() + h, temp);
            }
        }
        
        x = ddaIntercepts(i, p1, p3);
        temp = calculateRGB(p1.point.y(),p3.point.y(),i,p1.intensity,p3.intensity);
        if(phongSwitch){
            temp = calculateRGB(p1.point.y(),p3.point.y(),i,one.phongrgb,three.phongrgb);
        }
        if(x!= -1){
            bool found = false;
            for(int k = 0; k<records.size();k++){
                if(records[k] == x){
                    found = true;
                    break;
                }
            }
            if(!found){
                int h = 0;
                for(int h = 0; h<records.size();h++){
                    if(x<records[h]){
                        break;
                    }
                }
                records.insert(records.begin()+h,x);
                rgbs.insert(rgbs.begin() + h, temp);
            }
        }
        
        x = ddaIntercepts(i, p2, p3);
        temp = calculateRGB(p2.point.y(),p3.point.y(),i,p2.intensity,p3.intensity);
        if(phongSwitch){
            temp = calculateRGB(p2.point.y(),p3.point.y(),i,two.phongrgb,three.phongrgb);
        }
        if(x!= -1){
            bool found = false;
            for(int k = 0; k<records.size();k++){
                if(records[k] == x){
                    found = true;
                    break;
                }
            }
            if(!found){
                int h = 0;
                for(int h = 0; h<records.size();h++){
                    if(x<records[h]){
                        break;
                    }
                }
                records.insert(records.begin()+h,x);
                rgbs.insert(rgbs.begin() + h, temp);
            }
        }
        //开始画
        if (records.size() == 1){
            lol = 1;
            if(mode == 'y'){
                if(htSwitch){
                    if(lol==0){
                        Point la(vec3f(records[0], i,0)/3,rgbs[0]);
                        drawMegaPixel(la);
                    }else if(lol == 1){
                        lol++;
                    }else if(lol == 2){
                        lol = 0;
                    }
                }else{
                    draw_pix(records[0]*.5, i*.5+0.5*WINDOW_HEIGHT, rgbs[0]);
                }
            }else if(mode == 'z'){
                if(htSwitch){
                    Point la(vec3f(records[0]*.5+.5*WINDOW_WIDTH, i*.5+0.5*WINDOW_HEIGHT,0)/3,rgbs[0]);
                    drawMegaPixel(la);
                }else{
                    draw_pix(records[0]*.5+.5*WINDOW_WIDTH, i*.5+0.5*WINDOW_HEIGHT, rgbs[0]);
                }
            }
            else{
                if(htSwitch){
                    if(lol==0){
                        Point la(vec3f(records[0]*.5, i*.5,0),rgbs[0]);
                        drawMegaPixel(la);
                    }else if(lol == 1){
                        lol++;
                    }else if(lol == 2){
                        lol = 0;
                    }
                }else{
                    draw_pix(records[0]*.5, i*.5, rgbs[0]);
                }
            }
        }
        
        else if (records.size() == 2)
        {
            //            glm::vec3 colorbtw = glm::vec3(0);
            int dx;
            int start,end;
            dx = records[1] - records[0];
            lol = 1;
            if(dx<0){
                start = records[1];
                end = records[0];
                count++;
//                std::cout<<"dx<0!!!!"<<std::endl;
            }else{
                start = records[0];
                end = records[1];
            }
            
            for (int k = start; k <= end; k++)
            {
                //linear interpolation
                RGB colorbtw;
                colorbtw.r= (records[1] - k) * 1.0f / (dx * 1.0f) * rgbs[0].r + (k - records[0]) * 1.0f / (dx * 1.0f) * rgbs[1].r;
                colorbtw.g= (records[1] - k) * 1.0f / (dx * 1.0f) * rgbs[0].g + (k - records[0]) * 1.0f / (dx * 1.0f) * rgbs[1].g;
                colorbtw.b= (records[1] - k) * 1.0f / (dx * 1.0f) * rgbs[0].b + (k - records[0]) * 1.0f / (dx * 1.0f) * rgbs[1].b;
                if(mode == 'y'){
                    if(htSwitch){
                        if(lol==0){
                            Point la(vec3f(k*.5,i*.5+0.5*WINDOW_HEIGHT,0)/3,colorbtw);
                            drawMegaPixel(la);
                        }else if(lol == 1){
                            lol++;
                        }else if(lol == 2){
                            lol = 0;
                        }
                    }else{
                        draw_pix(k*.5, i*.5+0.5*WINDOW_HEIGHT, colorbtw);
                    }
                    
                }else if(mode == 'z'){
                    if(htSwitch){
                        if(lol==0){
                            Point la(vec3f(k*.5+.5*WINDOW_WIDTH,i*.5+0.5*WINDOW_HEIGHT,0)/3,colorbtw);
                            drawMegaPixel(la);
                        }else if(lol == 1){
                            lol++;
                        }else if(lol == 2){
                            lol = 0;
                        }
                    }else{
                        draw_pix(k*.5+.5*WINDOW_WIDTH, i*.5+0.5*WINDOW_HEIGHT, colorbtw);
                    }
                }else{
                    if(htSwitch){
                        if(lol==0){
                            Point la(vec3f(k*.5,i*.5,0)/3,colorbtw);
                            drawMegaPixel(la);
                        }else if(lol == 1){
                            lol++;
                        }else if(lol == 2){
                            lol = 0;
                        }
                    }else{
                        draw_pix(k*.5, i*.5, colorbtw);
                    }
                }
            }
        }else{
            int dx;
            int start,end;
            dx = records[2] - records[0];
            lol = 1;
            if(dx<0){
                start = records[2];
                end = records[0];
                count++;
//                std::cout<<"dx<0!!!!"<<std::endl;
            }else{
                start = records[0];
                end = records[2];
            }
            for (int k = start; k <= end; k++)
            {
                //linear interpolation
                RGB colorbtw;
                colorbtw.r = (records[2] - k) * 1.0f / (dx * 1.0f) * rgbs[0].r + (k - records[0]) * 1.0f / (dx * 1.0f) * rgbs[2].r;
                colorbtw.g = (records[2] - k) * 1.0f / (dx * 1.0f) * rgbs[0].g + (k - records[0]) * 1.0f / (dx * 1.0f) * rgbs[2].g;
                colorbtw.b = (records[2] - k) * 1.0f / (dx * 1.0f) * rgbs[0].b + (k - records[0]) * 1.0f / (dx * 1.0f) * rgbs[2].b;
//                int lol = 0;
                if(mode == 'y'){
                    if(htSwitch){
                        if(lol==0){
                            Point la(vec3f(k*.5,i*.5+0.5*WINDOW_HEIGHT,0)/3,colorbtw);
                            drawMegaPixel(la);
                        }else if(lol == 1){
                            lol++;
                        }else if(lol == 2){
                            lol = 0;
                        }
                    }else{
                        draw_pix(k*.5, i*.5+0.5*WINDOW_HEIGHT, colorbtw);
                    }
                    
                }else if(mode == 'z'){
                    if(htSwitch){
                        if(lol==0){
                            Point la(vec3f(k*.5+.5*WINDOW_WIDTH,i*.5+0.5*WINDOW_HEIGHT,0)/3,colorbtw);
                            drawMegaPixel(la);
                        }else if(lol == 1){
                            lol++;
                        }else if(lol == 2){
                            lol = 0;
                        }
                    }else{
                        draw_pix(k*.5+.5*WINDOW_WIDTH, i*.5+0.5*WINDOW_HEIGHT, colorbtw);
                    }
                }else{
                    if(htSwitch){
                        if(lol==0){
                            Point la(vec3f(k*.5,i*.5,0)/3,colorbtw);
                            drawMegaPixel(la);
                        }else if(lol == 1){
                            lol++;
                        }else if(lol == 2){
                            lol = 0;
                        }
                    }else{
                        draw_pix(k*.5, i*.5, colorbtw);
                    }
                }
            }
        }
        records.clear();
        rgbs.clear();
        lol = 1;
    }
//    std::cout<<count<<std::endl;
}







