#include "vector.h"
#include "matrix.h"

typedef mmm::vector<2, float> vec2f;
typedef mmm::vector<3, float> vec3f;
typedef mmm::vector<2, int> vec2i;
typedef mmm::vector<3, int> vec3i;

typedef mmm::matrix<3, 3, float> mat3f;
typedef mmm::matrix<4, 4, float> mat4f;

struct Edge
{
    int start, end;
    Edge(int start, int end) : start(start), end(end) {}
};

struct Face
{
    int v0, v1, v2;
    Face(int v0, int v1, int v2) : v0(v0), v1(v1), v2(v2) {}
};

class Polygon {
    //Center of mass is 0 respect to polygon itself
    //But position vector is the centroid from the viewPort
public:
    int count;
    std::vector<float> position;
    int edgeCount;
    vec3f transVec;
    float angle;
    float scale;
    std::vector<vec3f> vertices;
    std::vector<Edge> edges;
    std::vector<Face> faces;
    
public:
    void printPolygon();
    void updateCentroid();
    Polygon(){};
    
    Polygon(std::vector<Coord> &vert, std::vector <ECoord> &edges);
};