
#include "Geometry.h"

Polygon::Polygon(std::vector<Coord> &vert, std::vector <ECoord> &edges) {
        float xtotal = 0, ytotal = 0;
        this -> count = vert.size();
        this ->edgeCount = edges.size();
        for(int i = 0; i < vert.size(); i++){
            vertices.push_back(std::vector<float>{vert[i].x, vert[i].y, vert[i].z});
        }
        for(int j=0; j<edges.size();j++){
            this->edges.push_back(std::vector<int>{edges[j].x, edges[j].y});
        }
        this -> updateCentroid();
        
        transVec.x = 0.0;
        transVec.y = 0.0;
        transVec.z = 0.0;
        angle = 0.0;
        scale = 1.0;
    }

    void Polygon::printPolygon() {
    std::cout<<"numVertices: "<<this->count<<std::endl;
    for(int i = 0; i < this->count; i++) {
        std::cout<<"x: "<<(this->vertices[i])[0]<<" y: "<<(this->vertices[i])[1]<<" z: "<<(this->vertices[i])[2]<<std::endl;
    }
    //  for(int j = 0; j < this->edgeCount; j++) {
    //     std::cout<<"Edges "<<(this->edges[j])[0]<<" and "<<(this->edges[j])[1]<<std::endl;
    // }
    std::cout<<std::endl;
}

void Polygon::updateCentroid() {
    float xtotal = 0, ytotal = 0, ztotal=0;
    for(int i = 0; i < this->count; i++){
        xtotal += this->vertices[i][0];
        ytotal += this->vertices[i][1];
        ztotal += this->vertices[i][2];
    }
    this->position = {xtotal/(float)(this->count), ytotal/(float)(this->count),  ztotal/(float)(this->count)};

    // mat4f M = mat4f::identity();
    // M.index(0, 2)
    // mat4f N = inverse(M);
}