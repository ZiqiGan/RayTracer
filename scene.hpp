#ifndef SCENE_HPP
#define SCENE_HPP
#include "Light.h"
#include "Camera.h"
#include "Film.h"
#include <vector>
#include <stack>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;
class scene{
    private:
        int width, height;
        vec3 cameraPos;
        vec3 cameraUp;
        vec3 cameraLookAt;
        int fovy;
        //Color attenuation;
        Color ambient;
        Color diffuse;
        Color emission;
        double shininess;
        double attenuation[3];
        vector<Light*> lights;
        vector<Shape*> shapes;
        vector<vec3> vertices;
        string outputFile;
        int maxdepth;
       
        Camera *camera;
        Film *film;
        Raytracer* rt;

    public:
    
        Scene(char* filename);
    
        int getWidth(){ return this->width;}
        
        int getHeight(){ return this->getHeight;}

        int getFovy(){ return this->fovy; }
       
        vec3 getCameraPos(){ return this->cameraPos;}

        vec3 getCameraUp(){ return this->cameraUp;}

        vec3 getCameraLookAt(){ return this->cameraLookAt;}
        
        Color getAmbient(){ return this->ambient;}

        Color getiDiffuse(){ return this->diffuse;}

        Color getSpecular(){ return this->specular;}

        vector<Light*> getLights() { return this->lights;}

        vector<Shape*> getShapes() { return this->shapes;} 

        int getMaxDepth() { return this->maxdepth;}

        bool generateSample(sample* pixel){
            if(pixel->getX<=width && pixel->getY<=height){
                return true;
            }            
            else{
                return false;
            }

        }

        void rightmultiply(const mat4 & M, stack<mat4> &transfstack)
        {
            mat4 &T = transfstack.top();
            T = T * M;
        }
        
        void readFile(char* filename){
            string str, cmd;
            ifstream in;
            in.open(filename);
            if (in.is_open()){

                //set up the transforamtion stack
                stack<mat4> transfstack;
                transfstack.push(mat(1.0));
                
                vector<vec3> triVertex = vector<vec3>(3);
                int vtCount = 0;
                int maxverts;
                
                getline(in,str);
                while(in){
                    if((str.find_first_not_of("\t\r\n")!=
                        string::npos)&&(str[0]!='#')){
                            stringstream s(str);
                            s>>cmd;
                        }

                    //read in the size of the image
                    if( cmd=="size" ) {
                        s>> this->width;
                        s>> this->height;
                    }

                    //read in the output file 
                    else if( cmd=="output" ){
                        s>>outputFile;
                    }
                    
                    else if( cmd=="maxdepth"){
                        s>>maxdepth;
                    }
                    //-----------------------------------------
                    //Below is the section for material properties
                    //-----------------------------------------
                    else if(cmd == "emission"){
                        double values[3];
                        for(int i=0;i<3;i++)
                        {
                            s>>values[i];
                        }
                        emission = Color(values[0],values[1],values[2]);
                    }

                    else if(cmd == "diffuse"){
                        double values[3];
                        for(int i=0;i<3;i++)
                        {
                            s>>values[i];
                        }
                        diffuse = Color(values[0],values[1],values[2]);
                    }

                    else if(cmd == "specular"){
                        double values[3];
                        for(int i=0;i<3;i++)
                        {
                            s>>values[i];
                        }
                        specualr = Color(values[0],values[1],values[2]);
                     }
                    
                     else if(cmd == "shininess"){
                         s>>shininess;
                     }

                    //read in camera values
                    else if(cmd == "camera"){
                        for(int i=0;i<3;i++)
                        {
                            s>>cameraPos[i];
                        }
                        for(int i=0;i<3;i++)
                        {
                            s>>cameraUp[i];
                        }
                        for(int i=0;i<3;i++)
                        {
                            s>>cameraLookAt[i];
                        }
                        s>>this->fovy;
                    }

                    //maxverts
                    else if(cmd == "maxverts"){
                        s>>maxverts;
                        vecrtices = vector<vec3>(maxverts);
                    }

                    //vertices
                    else if(cmd == "vertex"){
                        double values[3];
                        for(int i=0;i<3;i++){
                            s>>values[i];
                        }
                        vec3 newVertex = vec3(values[0],values[1],values[2]);
                        vecrtices.push_back(newVertex);
                    }


                    //-----------------------------------------
                    //Below is the section for light properties
                    //-----------------------------------------
                    
                    //ambient value
                    else if(cmd == "ambient"){
                        int red,green,blue;
                        s>>red;
                        s>>green;
                        s>>blue;
                        ambient = Color(red,green,blue);
                    }

                    //attenuation value
                    else if(cmd == "attenuation"){
                        for(int i=0;i<3;i++){
                            s>>attenuation[i];
                        }
                    }

                    //directional light
                    else if(cmd == "directional"){
                        //TODO
                    }

                    //point light
                    else if(cmd == "point"){
                        //TODO
                    }
                    
                    //--------------------------------------------------
                    //--Below is the section for matrix transforamtion--
                    //--------------------------------------------------
                   
                    //read in transaltion value
                    else if(cmd == "translate") {
                        double[] values;
                        for(int i=0;i<3;i++){
                            s>>values[i];
                        }
                        mat4 tmat = Transform::translate(values[0],
                                    values[1],values[2]);
                        rightmultiply(tmat,transfstack);
                    }
                    
                    //read in scaling value
                    else if(cmd == "scale") {
                        double[] values;
                        for(int i=0;i<3;i++){
                            s>>values[i];
                        }

                        mat4 tmat = Transform::scale(values[0],
                                    values[1],values[2]);
                        rightmultipy(tmat,transfstack);
                    }

                    //read in rotation value
                    else if(cmd == "rotate") {
                        double[] values;
                        for(int i=0;i<4;i++){
                            s>>values[i];
                        }
                        vec3 axis = glm::normalize(vec3(values[0],
                            values[1],values[2]));
                        mat4 tmat = mat4(Transform::rotate(values[3],axis));
                        rightmultiply(tmat,transfstack);
                    }
                
                    else if(cmd == "pushTransform"){
                        transfstack.push(tranfstack.top());
                    }
                    else if(cmd == "popTransform"){
                        transfstack.pop();
                    }


                    //----------------------------------------
                    //Below is the section for parsing objects
                    //----------------------------------------
                    
                    //read in objects
                    else if(cmd == "Sphere"){
                        vec3 center;
                        double radius;
                        //read in the center vector
                        s>>center.x;
                        s>>center.y;
                        s>>center.z;
                        s>>radius;
                        shapes->push_back(new Sphere(center,radius,diffuse,
                                            specular,emission,shininess));
                    }

                    else if(cmd == "tri"){
                        vtCount++;
                        int values[3];
                        
                        for(int i=0;i<3;i++)
                        {
                            s>>values[i];
                        }

                        triVertex[vtCount] = vec3(values[0],values[1],values[2]);
                        
                        if(vtCount==2)
                        {
                            vec4 A = vec4(triVertex[0]);
                            vec4 B = vec4(triVertex[1]);
                            vec4 C = vec4(triVertex[2]);
                            vec3 vecA = vec4(transfstack.top()*A);
                            vec3 vecB = vec4(transfstack.top()*B);
                            vec3 vecC = vec4(transfstack.top()*C);
                            shapes->push_back(new Triangle(vecA,vecB,vecC,
                                            diffuse,specular,emission,shininess));
                            vtCount=0;
                        }
                        
                    }
                    
                }
            }
        }


        void Render(){
            
            cout<<"Start Rendering the Image......"<<endl;
            
            while(generateSample(&sample)){
                cout<<"Generating camera ray...... ";
                Ray ray = camera->generateRay(sample);
                cout<<"Done!"<<endl;
                Cout<<"Ray tracing.......";
                Color color = rt->trace(ray,maxdepth);
                cout<<"Done!"<<endl;
                film->commit(sample,color);
            }
            cout<<"Generating image......";
            film->writeImage();
            cout<<"Done!"<<endl;
        }
};

#endif