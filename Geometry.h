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
    float r, g, b;
    RGB(){
        this->r = 0;
        this->g = 0;
        this->b = 0;
    }
    RGB(float c0, float c1, float c2) {
        this->r = c0;
        this->g = c1;
        this->b = c2;
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

struct PhongModel{
    float ambient;
    RGB kd;
    float ks;
    float IL;
    vec3f fPoint;
    vec3f pPoint;
    vec3f lPoint;
    vec3f nVector;
    vec3f lVector;
    vec3f rVector;
    vec3f vVector;
    float K;
    float n;
    PhongModel(){
        this->ambient = 0.5;
        this->kd = {0, 0, 0};
        this->ks = 0.5;
        this->IL = 0.8;
        this->pPoint = {0, 0, 0};
        this->lPoint = {1, 1, 0};
        this->K = 0.5;
        this->n = 1;
        this->lVector = lPoint - pPoint;
        this->rVector =
        this->vVector = fPoint - pPoint;
    }
    PhongModel(Point dot, vec3f facenormal,vec3f frontPoint, vec3f light, float spec){
        this->ambient = 0.5;
        this->kd = dot.intensity;
        this->ks = 0.5;
        this->IL = 0.8;
        this->fPoint = frontPoint;
        this->pPoint = dot.point;
        this->lPoint = light;
        this->nVector = facenormal;
        this->lVector = light - dot.point;
        this-> rVector ;
        this->vVector = this->fPoint - pPoint;
        float K = 0.5;
        float n = spec;
        RGB phongInten(1,1,1);
        calculatePhong(dot);
    }
    RGB calculatePhong(Point dot){
        float rnumber = 0;
        float gnumber = 0;
        float bnumber = 0;
        rnumber = ambient + IL*(kd.r*(lVector.dot(nVector)) +pow(ks*(rVector.dot(vVector)),n) )/(vVector.norm() * K);
        std::cout<<rnumber<<std::endl;
        gnumber = ambient + IL*(kd.g*(lVector.dot(nVector)) +pow(ks*(rVector.dot(vVector)),n) )/(vVector.norm() * K);
        std::cout<<gnumber<<std::endl;
        bnumber = ambient + IL*(kd.b*(lVector.dot(nVector)) +pow(ks*(rVector.dot(vVector)),n) )/(vVector.norm() * K);
        std::cout<<bnumber<<std::endl;
        RGB value(rnumber,gnumber,bnumber);
        return value;
        
    }
};

//0:Empty, 1:red, 2:green, 3:blue
struct MegaPixel{
    char mpixel[3][3];
    MegaPixel(){
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                mpixel[i][j] = 'e';
            }
        }
    }
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
