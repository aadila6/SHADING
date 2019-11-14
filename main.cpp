
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
void goShading(Point a, Point b, Point c, char mode);
Point swapCoordsyz(Point &v1);
Point swapCoordsxz(Point &v1);
/*initialize gl stufff*/
int main(int argc, char **argv)
{
    inputFileName = "cia.txt";
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
            vec3f point(x*200,y*200,z*200);
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
void drawMegaPixel(Point lol){
    //Point ht(vec3f(30,30,30),RGB(0,200,200));
    MegaPixel pix = halfToning(lol);
    int x = lol.point.x();
    int y = lol.point.y();
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(pix.mpixel[i][j] == 'r'){
                RGB drawColor(255,0,0);
                draw_pix(x*3+j,y*3+i,drawColor);
            }else if(pix.mpixel[i][j] == 'g'){
                RGB drawColor(0,255,0);
                draw_pix(x*3+j,y*3+i,drawColor);
            }else if(pix.mpixel[i][j] == 'b'){
                RGB drawColor(0,0,255);
                draw_pix(x*3+j,y*3+i,drawColor);
            }
        }
    }
}

void display()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    //drawSplitLines();
    
    Point c(vec3f(40,40,0),RGB(0,0,255)); // blue
    Point b(vec3f(20,20,0),RGB(0,255,0)); // green
    Point a(vec3f(70,10,0),RGB(255,0,0)); // red
    //     Point c(vec3f(0.375*200, 0.625*200, 0.625*200),RGB(0,0,255)); // blue
    //     Point b(vec3f(0.625*200, 0.375*200, 0.625*200),RGB(0,255,0));  // green
    //     Point a(vec3f(0.625*200, 0.625*200, 0.625*200),RGB(255,0,0)); // red
    
    //    draw_pix(0.375*200, 0.375*200,RGB(255,0,0));
    //    draw_pix(0.625*200, 0.375*200,RGB(255,0,0));
    //    draw_pix(0.375*200, 0.625*200,RGB(255,0,0));
    
    //GShading(a,c,b);
    
    //    std::cout<<"***********************************"<<std::endl;
    
    //    GShadHalfToningOn(b,a,c);
    //    GShading(b,c,a);
    //    GShading(a,c,b);
    //    GShading(c,a,b);
    //    GShading(c,b,a);
    
//    int faceNum = 12;
//    draw_pix(polygonList[0].vertices[polygonList[0].faces[faceNum].f0-1].point.x(),
//             polygonList[0].vertices[polygonList[0].faces[faceNum].f0-1].point.y(),RGB(255,0,0));
//    draw_pix(polygonList[0].vertices[polygonList[0].faces[faceNum].f1-1].point.x(),
//             polygonList[0].vertices[polygonList[0].faces[faceNum].f1-1].point.y(),RGB(255,0,0));
//    draw_pix(polygonList[0].vertices[polygonList[0].faces[faceNum].f2-1].point.x(),
//             polygonList[0].vertices[polygonList[0].faces[faceNum].f2-1].point.y(),RGB(255,0,0));
//
//    goShading(polygonList[0].vertices[polygonList[0].faces[faceNum].f0-1],
//              polygonList[0].vertices[polygonList[0].faces[faceNum].f1-1],
//              polygonList[0].vertices[polygonList[0].faces[faceNum].f2-1]);

        std::cout<<"***********************************"<<std::endl;
        for(int i = 0; i<polygonList[0].faces.size(); i++){
                goShading(polygonList[0].vertices[polygonList[0].faces[i].f0-1],
                         polygonList[0].vertices[polygonList[0].faces[i].f1-1],
                         polygonList[0].vertices[polygonList[0].faces[i].f2-1],'y');
    //       std::cout<<"Vertex : "<<polygonList[0].faces[i].f0<<" "<<polygonList[0].faces[i].f1<<" "<<polygonList[0].faces[i].f2 <<std::endl;
        }
    
    
    
    
    //GShading(a,b,c);
    //GShadHalfToningOn(polygonList[0].vertices[0],polygonList[0].vertices[2],polygonList[0].vertices[1]);
    
    //drawMegaPixel();
    //checks for opengl errors
    glutSwapBuffers();
    check();
    // polygonList.clear();
    // for(int u=0;u<cPolygonList.size();u++){
    //     cPolygonList[u].updateCentroid();
    //     polygonList.push_back(cPolygonList[u]);
    // }
    //writeFile(inputFileName, polygonList);
}

void draw_pix(int x, int y, RGB color)
{
    glBegin(GL_POINTS);
    //glColor3f(0.41, 0.4, 0.4);
    glColor3f(color.r/255, color.g/255, color.b/255);
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
void goShading(Point a, Point b, Point c, char mode){
    Point v1 = a;
    Point v2 = b;
    Point v3 = c;
    sortYlocation(v1,v2,v3);
    if(mode == 'y'){
        v1 = swapCoordsyz(v1);
        v2 = swapCoordsyz(v2);
        v3 = swapCoordsyz(v3);
    }else if(mode == 'z'){
        v1 = swapCoordsxz(v1);
        v2 = swapCoordsxz(v2);
        v3 = swapCoordsxz(v3);
    }
    //two case, v1.y=v2.y or v2.y = v3.y
    int dX1 = (int)(v2.point.x() - v1.point.x());
    int dX2 = (int)(v3.point.x() - v1.point.x());
    int dX3 = (int)(v3.point.x() - v2.point.x());
    
    int dY1 = (int)(v2.point.y() - v1.point.y());
    int dY2 = (int)(v3.point.y() - v1.point.y());
    int dY3 = (int)(v3.point.y() - v2.point.y());
    float x1 = 0;
    float x2 = 0;
    float incX1, incX2;
    int steps = 0;
    float y = (float)v1.point.y();
    
    if(dY1 == 0){ //Case I: v1.y = v2.y where dY1 = 0
        x1 = (float)v1.point.x();
        x2 = (float)v2.point.x();
        incX1 = (float)dX2 / (float)fabs(dY2); //1-3
        incX2 = (float)dX3 / (float)fabs(dY3);//2-3
        int steps = fabs(dY2);
        for(int i =0; i<steps; i++){
            x1 += incX1;
            x2 += incX2;
            y -= 1;
            RGB I1 = calculateRGB(v1.point.y(),v3.point.y(),y,v1.intensity,v3.intensity);
            RGB I2 = calculateRGB(v2.point.y(),v3.point.y(),y,v2.intensity,v3.intensity);
            draw_pix(x1,y,I1);
            draw_pix(x2,y,I2);
            
            std::cout<<"CASE I: "<<x1<<" "<<y<<" "<<x2 <<" "<<y<<std::endl;
            for(int j = 1; j<= fabs(x2 - x1); j++){
                if(x2>x1){
                    RGB Ibetween = calculateRGB(x1,x2,x1+j,I1,I2);
                    //RGB Ibetween = calIntense(I1,I2,x1+j,x1,x2);
                    draw_pix(x1+j,y,Ibetween);
                }else{
                    RGB Ibetween = calculateRGB(x2,x1,x2+j,I2,I1);
                    //RGB Ibetween = calIntense(I1,I2,x1+j,x1,x2);
                    draw_pix(x2+j,y,Ibetween);
                }
            }
        }
    }else if(dY3 == 0){ //Case II: v2.y = v3.y
        x1 = (float)v1.point.x();
        x2 = (float)v1.point.x();
        incX1 = (float)dX1 / (float)fabs(dY1);
        incX2 = (float)dX2 / (float)fabs(dY2);
        float y = (float)v1.point.y();
        steps = fabs(dY1);
        RGB midstore(0,0,0);
        for(int i = 0; i < steps; i++) {
            x1 += incX1;
            x2 += incX2;
            y -= 1;
            //Assume I have the intensity from PHONG
            RGB I1 = calculateRGB(v1.point.y(),v2.point.y(),y,v1.intensity,v2.intensity);
            RGB I2 = calculateRGB(v1.point.y(),v3.point.y(),y,v1.intensity,v3.intensity);
            draw_pix(x1,y,I1);
            draw_pix(x2,y,I2);
            std::cout<<"CASE II: "<<x1<<" "<<y<<" "<<x2 <<" "<<y<<std::endl;
            for(int j = 1; j<= fabs(x2 - x1); j++){
                if(x2>x1){
                    RGB Ibetween = calculateRGB(x1,x2,x1+j,I1,I2);
                    //RGB Ibetween = calIntense(I1,I2,x1+j,x1,x2);
                    draw_pix(x1+j,y,Ibetween);
                }else{
                    RGB Ibetween = calculateRGB(x2,x1,x2+j,I2,I1);
                    //RGB Ibetween = calIntense(I1,I2,x1+j,x1,x2);
                    draw_pix(x2+j,y,Ibetween);
                }
            }
        }
        
    }else{
        steps = fmin(fabs(dY1),fabs(dY2));
        x1 = (float)v1.point.x();
        x2 = (float)v1.point.x();
        incX1 = (float)dX1 / (float)fabs(dY1);
        incX2 = (float)dX2 / (float)fabs(dY2);
        float y = (float)v1.point.y();
        RGB midstore(0,0,0);
        for(int i = 0; i < steps; i++) {
            x1 += incX1;
            x2 += incX2;
            y -= 1;
            //Assume I have the intensity from PHONG
            RGB I1 = calculateRGB(v1.point.y(),v2.point.y(),y,v1.intensity,v2.intensity);
            RGB I2 = calculateRGB(v1.point.y(),v3.point.y(),y,v1.intensity,v3.intensity);
            draw_pix(x1,y,I1);
            draw_pix(x2,y,I2);
            std::cout<<"CASE III: "<<x1<<" "<<y<<" "<<x2 <<" "<<y<<std::endl;
            for(int j = 1; j<= fabs(x2 - x1); j++){
                if(x2>x1){
                    RGB Ibetween = calculateRGB(x1,x2,x1+j,I1,I2);
                    //RGB Ibetween = calIntense(I1,I2,x1+j,x1,x2);
                    draw_pix(x1+j,y,Ibetween);
                }else{
                    RGB Ibetween = calculateRGB(x2,x1,x2+j,I2,I1);
                    //RGB Ibetween = calIntense(I1,I2,x1+j,x1,x2);
                    draw_pix(x2+j,y,Ibetween);
                }
            }
            if(i == steps -1){
                midstore = I2;
            }
        }
            float midY = v2.point.y();
            steps = fabs(dY3);
            incX1 = (float)dX3 / (float)steps;
            x1 = (float)v2.point.x();
//            y = midY;
            for(int i = 0; i < steps; i++) {
                x1 += incX1;
                x2 += incX2;
                y -= 1;
                //Assume I have the intensity from PHONG
                RGB I1 = calculateRGB(v2.point.y(),v3.point.y(),y,v2.intensity,v3.intensity);
                RGB I2 = calculateRGB(midY,v3.point.y(),y,midstore,v3.intensity);
                draw_pix(x1,y,I1);
                draw_pix(x2,y,I2);
                for(int j = 1; j<= fabs(x2 - x1); j++){
                    if(x2>x1){
                        RGB Ibetween = calculateRGB(x1,x2,x1+j,I1,I2);
                        //RGB Ibetween = calIntense(I1,I2,x1+j,x1,x2);
                        draw_pix(x1+j,y,Ibetween);
                    }else{
                        RGB Ibetween = calculateRGB(x2,x1,x2+j,I2,I1);
                        //RGB Ibetween = calIntense(I1,I2,x1+j,x1,x2);
                        draw_pix(x2+j,y,Ibetween);
                    }
                }
        }
    }
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
            //RGB lala(0, 0, 255);
            //std::cout<<"O1: "<<v1.intensity.c0<<" "<<v1.intensity.c1<<" "<<v1.intensity.c2 <<std::endl;
            //std::cout<<"I1: "<<I1.c0<<" "<<I1.c1<<" "<<I1.c2 <<std::endl;
            //std::cout<<"I2: "<<I2.c0<<" "<<I2.c1<<" "<<I2.c2 <<std::endl;
            
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
    
    
    
