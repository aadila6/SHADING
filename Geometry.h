#include "vector.h"
#include "matrix.h"
#include <vector>
typedef mmm::vector<2, float> vec2f;
typedef mmm::vector<3, float> vec3f;
typedef mmm::vector<2, int> vec2i;
typedef mmm::vector<3, int> vec3i;

typedef mmm::matrix<3, 3, float> mat3f;
typedef mmm::matrix<4, 4, float> mat4f;

struct Edge
{
    int start, end;
    Edge(int start, int end) {
        this->start=(start);
        this->end=(end);
    }
    
};
struct RGB
{
    float c0, c1, c2;
    RGB(){
        this->c0 = 0;
        this->c1 = 0;
        this->c2 = 0;
    }
    RGB(float c0, float c1, float c2) {
        this->c0 = c0;
        this->c1 = c1;
        this->c2 = c2;
    }
};
struct Point
{
    vec3f point;
    RGB intensity;
    Point(vec3f inputPoint, RGB intensity){
        this->point = inputPoint;
        this->intensity= intensity;
    }
    Point(vec3f inputPoint){
        this->point = inputPoint;
        RGB temp(0,0,0);
        this->intensity = temp;
    }
};

struct Face
{
    int f0, f1, f2;
    vec3f normal;
    Face(int f0, int f1, int f2) : normal()
    {
        this->f0=(f0); 
        this->f1=(f1); 
        this->f2=(f2);
    }
    
};

struct Phong{
    float ambient = 0.5;
    RGB kd;
    float ks = 1;
    vec3f eyef, P, N, R, V;
};

class Polygon {
    //Center of mass is 0 respect to polygon itself
    //But position vector is the centroid from the viewPort
public:
    int count;
    std::vector<float> position;
    int edgeCount;
    int faceCount;
    std::vector<Point> vertices;
    std::vector<Edge> edges;
    std::vector<Face> faces;
    //std::vector<RGB> colors;
    std::vector<float> specs;
public:
    void printPolygon();
    void updateCentroid();
    void updateNormalforfaces();
    Polygon(){};
    Polygon(std::vector<Point> vert, std::vector<Face> faces, std::vector<float>specs);
};
