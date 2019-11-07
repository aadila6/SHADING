
#include "Geometry.h"

Polygon::Polygon(std::vector<Point> vert, std::vector<Face> faces, std::vector<float>specs) {
    this -> count = vert.size();
    this -> edgeCount = edges.size();
    this -> faceCount = faces.size();
    
    for(int i = 0; i < count; i++){
        //this->vertices.push_back(std::vector<vec3f>{vert[i].x(), vert[i].y(), vert[i].z()});
        this->vertices.push_back(vert[i]);
    }
    for(int k=0;k<faces.size();k++){
        //this->faces.push_back(std::vector<Face>{faces[k].f0, faces[k].f1, faces[k].f2});
        this->faces.push_back(faces[k]);
    }
    for(int k=0;k<faces.size();k++){
        this->specs.push_back(specs[k]);
    }
    this->updateNormalforfaces();
    //this -> updateCentroid();
}

void Polygon::printPolygon() {
    std::cout<<"numVertices: "<<this->count<<std::endl;
    for(int i = 0; i < this->count; i++) {
        std::cout<<"x: "<< this->vertices[i].point.x()<<" y: "<<this->vertices[i].point.y()<<" z: "<<this->vertices[i].point.y()<<std::endl;
    }
    std::cout<<std::endl;
}
void Polygon::updateNormalforfaces(){
    for(int i = 0;i < faces.size(); i++) {
        Face &f = this->faces[i];
        vec3f a = this->vertices[f.f1].point - this->vertices[f.f0].point;
        vec3f b = this->vertices[f.f2].point - this->vertices[f.f0].point;
        vec3f n = (a.cross(b)).normalized();
        f.normal = n;
    }
}

void Polygon::updateCentroid() {
    float xtotal = 0, ytotal = 0, ztotal=0;
    for(int i = 0; i < this->count; i++){
        xtotal += this->vertices[i].point.x();
        ytotal += this->vertices[i].point.y();
        ztotal += this->vertices[i].point.y();
    }
    this->position = {xtotal/(float)(this->count), ytotal/(float)(this->count),  ztotal/(float)(this->count)};
}

