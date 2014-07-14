#pragma once

#include "ofMain.h"
#include <fstream>

typedef struct
{
    float minX;
    float minY;
    float maxX;
    float maxY;
}BBox;

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void save();
private:
    ofImage _colorImage;
    int _imgWidth, _imgHeight;
    int _pickedId;
    string _path;
    ofMesh _mesh,_blackMesh;
    vector<ofSpherePrimitive> _verticesSpheres;
    //void list_hits(GLint hits, GLuint *buffer);
    int glSelect(int x, int y);
    
    ofTexture 		texColor;
    
    int 			w, h;
    
    unsigned char 	* colorPixels;
    ofColor materialColor;
    ofPoint _lastPoint;
    ofMaterial material;
    BBox bbox;
    ofCamera testCam;
};