#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    bbox.maxX = std::numeric_limits<float>::min();
    bbox.maxY = std::numeric_limits<float>::min();
    bbox.minX = std::numeric_limits<float>::max();
    bbox.minY = std::numeric_limits<float>::max();
    
    _useRamanan = false;
    
    // Load image
    _path = "/Users/spherik//Documents/WORK/PhD/Bosphorus/originals/bs000/bs000_E_FEAR_0.obj";
    //_path = "/home/spherik/Documents/PhD/Bosphorus/originals/bs000/bs000_E_FEAR_0.obj";
    //_path = "/Volumes/Dades/spherik/Documents/WORK/PhD/Bosphorus/originals/bs000/bs000_E_ANGER_0.obj";
    

    if(!loadMesh(_path))
    {
        ofExit();
    }
    cout << "BBox:" << endl << "* Min: " << bbox.minX << ", " << bbox.minY << endl << "* Max: " << bbox.maxX << ", " << bbox.maxY << endl << "* Center: " << bbox.minX+(bbox.maxX-bbox.minX)/2.0 << ", " << bbox.minY+(bbox.maxY-bbox.minY)<< endl;
    
    float diagonal = sqrt((bbox.maxX-bbox.minX)*(bbox.maxX-bbox.minX)+(bbox.maxY-bbox.minY)*(bbox.maxY-bbox.minY));
    // Material
    materialColor.setBrightness(250.f);
    //materialColor.setSaturation(200);
    material.setSpecularColor(materialColor);
    material.setAmbientColor(ofFloatColor(255.f,255.f,255.f,255.f));
    
    // Camera
    float viewAngle = 60.0;
    double distance = (diagonal/2.0)/sin((viewAngle*(pi/180)/2.0));
    std::cout << "Diagonal: " << diagonal << ". Distance: " << distance << endl;
    testCam.setupPerspective(true, viewAngle, 1, distance*1.5, ofVec2f(0.0));
    testCam.setPosition(bbox.minX+(bbox.maxX-bbox.minX)/2.0,bbox.minY+(bbox.maxY-bbox.minY)/2.0,distance);
    testCam.lookAt(ofVec3f(bbox.minX+(bbox.maxX-bbox.minX)/2.0,bbox.minY+(bbox.maxY-bbox.minY)/2.0,0.0), ofVec3f(0.0,1.0,0.0));
    
    cout << "MVP: " << testCam.getModelViewProjectionMatrix() << endl << "------------------------" << endl;
    
    ofEnableDepthTest();
    
    cout << "Setup Mesh has :" << _mesh.getNumVertices() << endl;
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    //ofTrueTypeFont ttf;
    //ttf.loadFont("verdana.ttf", 32, true, true, true);
    ofSetColor(255);
    ofDrawBitmapString("FPS: "+ ofToString((int) ofGetFrameRate()), 10, 20);
	testCam.begin(ofGetCurrentViewport());
    
	// Draw mesh
    material.begin();
    ofSetColor(255,255,255);
    //glPushName(0);
    _colorImage.getTextureReference().bind();
    _mesh.draw();
    _colorImage.getTextureReference().unbind();
    //glPopName();
    _mesh.axis().drawWireframe();
    material.end();
    
    // Draw black mesh
    ofSetColor(0.0, 0.0, 0.0);
    _blackMesh.drawWireframe();
    
    // Draw spheres
    ofSetColor(255,255,255);
    for(int i = 0; i < _verticesSpheres.size(); i++)
    {
        //glPushName(i);
        _verticesSpheres[i].draw();
        //glPopName();
    }
    testCam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == 's')
        save();
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	//cout << "Dragged: " << x << ", " << y << "->";
    
    if(_pickedId>-1)
    {
    	ofVec3f newPos = testCam.screenToWorld(ofVec3f(x,ofGetHeight()-y,0.999333), ofGetCurrentViewport());
    	newPos.z = 0.0;
        //cout << _mesh.getVertex(_pickedId) << " - " << x << ", " << y << endl;
        _mesh.disableTextures();
        _mesh.setTexCoord(_pickedId, ofVec2f(newPos.x/_colorImage.width,newPos.y/_colorImage.height));
        
        _mesh.enableTextures();
        _mesh.setVertex(_pickedId, newPos);
        _verticesSpheres[_pickedId].setPosition(newPos);
        newPos.z = 0.1;
        _blackMesh.setVertex(_pickedId, newPos);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    _pickedId = glSelect(x,y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    _pickedId = -1;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
//--------------------------------------------------------------
// From: http://forum.openframeworks.cc/t/problem-with-picking-objects-in-opengl/2143/5
int ofApp::glSelect(int x, int y)
{
	GLuint buff[512] = {0};
	GLint hits, view[4];
	//GLfloat proj_matrixgl[16];
	ofRectangle viewport = ofGetCurrentViewport();
	/*
     This choose the buffer where store the values for the selection data
     */
	glSelectBuffer(512, buff);
	/*
     This retrieves info about the viewport
     */
    glGetIntegerv(GL_VIEWPORT, view);
    //glGetFloatv(GL_PROJECTION_MATRIX, proj_matrixgl);
    
    ofMatrix4x4 proj_matrix = testCam.getProjectionMatrix(viewport);
	/*
     Switching in selecton mode
     */
	glRenderMode(GL_SELECT);
	/*
     Clearing the names' stack
     This stack contains all the info about the objects
     */
	glInitNames();
	/*
     Now modify the viewing volume, restricting selection area around the cursor
     */
    
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
    glLoadIdentity();
    /*
     restrict the draw to an area around the cursor
     */
    gluPickMatrix(x, y, 1.0, 1.0, view);
    glMultMatrixf(proj_matrix.getPtr());
    
    /*
     Draw the objects onto the screen
     */
    glMatrixMode(GL_MODELVIEW);
    /*
     draw only the names in the stack, and fill the array
     */
    /******** draw(); *****/
    glPushMatrix();
    glLoadIdentity();
    glMultMatrixf(testCam.getModelViewMatrix().getPtr());
    
    //glPushName(0);
    _mesh.draw();
    //glPopName();
    
    // Draw spheres
    for(int i = 0; i < _verticesSpheres.size(); i++)
    {
        glPushName(i);
        _verticesSpheres[i].draw();
        glPopName();
    }
    glPopMatrix();
    
    /*
     Do you remeber? We do pushMatrix in PROJECTION mode
     */
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    /*
     get number of objects drawed in that area
     and return to render mode
     */
	hits = glRenderMode(GL_RENDER);
	/*
     Get nearest object
     */
	unsigned int j;
	GLuint *ptr, minZ, minminZ, nearestId = -1, *ptrNames, numberOfNames;
	//printf ("hits = %d\n", hits);
	ptr = (GLuint *) buff;
	minminZ = 0xffffffff;
	for (int i = 0; i < hits; i++) {
		numberOfNames = *ptr;
		ptr++;
		minZ = *ptr;
		ptrNames = ptr+2;
		if(minminZ>minZ && numberOfNames>0){
			minminZ = minZ;
			nearestId = ptrNames[0];
		}
		/*printf("%d names found:",numberOfNames);
         for (j = 0; j < numberOfNames; j++,ptrNames++) {
         printf ("%d ", *ptrNames);
         }
         printf ("\n");*/
		ptr += numberOfNames+2;
	}
	printf("nearest id %d\n",nearestId);
    
    
	glMatrixMode(GL_MODELVIEW);
    /*
     Return nearest object
     */
    return nearestId;
}


void ofApp::save()
{
    string fpCoordFilename = _path;
    if(_useRamanan)
    {
    ofStringReplace(fpCoordFilename, "originals", "ramanan2D");
    
    }
    else
    {
        ofStringReplace(fpCoordFilename, "originals", "ToyFace");
    }
    
    ofStringReplace(fpCoordFilename, ".obj", ".dat");
    cout << "Save Mesh has :" << _mesh.getNumVertices() << endl;
    ofstream fout; //declare a file stream
    fout.open( ofToDataPath(fpCoordFilename).c_str() ); //open your text file
    fout << _mesh.getNumVertices();
    for (int i = 0; i < _mesh.getNumVertices(); i++) {
        ofVec3f pos = _mesh.getVertex(i);
        fout << endl << pos.x/_colorImage.width << " " << pos.y/_colorImage.height ;
    }
    fout.close();
}

bool ofApp::loadMesh(string filename)
{
    string imageFilename = filename;
    
    ofStringReplace(imageFilename, "originals", "images");
    ofStringReplace(imageFilename, ".obj", ".png");
    
    _colorImage.loadImage(imageFilename);
    
    //Load mesh
    string fpCoordFilename = filename;
    string fpTriFilename = filename;
    _mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    if(_useRamanan)
    {
        ofStringReplace(fpCoordFilename, "originals", "ramanan2D");
        ofStringReplace(fpCoordFilename, ".obj", ".dat");
        
        ofStringReplace(fpTriFilename, "originals", "ramanan3D");
        
        vector<string> splitString = ofSplitString( fpTriFilename, "/");
        ofStringReplace(fpTriFilename, splitString[splitString.size()-1], "ramanan.tri");
        ofStringReplace(fpTriFilename, splitString[splitString.size()-2], "");
    }
    else
    {
        ofStringReplace(fpCoordFilename, "originals", "ToyFace");
        ofStringReplace(fpCoordFilename, ".obj", ".dat");
        
        ofStringReplace(fpTriFilename, "originals", "ToyFace");
        
        vector<string> splitString = ofSplitString( fpTriFilename, "/");
        ofStringReplace(fpTriFilename, splitString[splitString.size()-1], "ramanan.tri");
        ofStringReplace(fpTriFilename, splitString[splitString.size()-2], "");
        cout << fpCoordFilename << endl;
    }
    //cout << fpTriFilename << endl;
    int numRows = 0;
    int readRows = 0;
    ifstream fin; //declare a file stream
    fin.open( ofToDataPath(fpCoordFilename).c_str() ); //open your text file
    
    vector<ofVec3f> vertices;
    vector<ofVec2f> texCoord;
    vector<ofIndexType> indexs;
    cout << imageFilename << endl;
    fin >> numRows;
    while(readRows < numRows) //as long as theres still text to be read
    {
        float x,y,z;
        ofSpherePrimitive spherePrimitive;
        spherePrimitive.setRadius(5.0);
        fin >> x >> y;
        
        spherePrimitive.setPosition(ofVec3f(1.0*x*_colorImage.width,1.0*y*_colorImage.height,0.0) );
        vertices.push_back(ofVec3f(1.0*x*_colorImage.width,1.0*y*_colorImage.height,0.0));
        texCoord.push_back(ofVec2f(1.0*x,1.0*y));
        _verticesSpheres.push_back(spherePrimitive);
        
        bbox.maxX = max(bbox.maxX, x*_colorImage.width);
        bbox.maxY = max(bbox.maxY, y*_colorImage.height);
        bbox.minX = min(bbox.minX, x*_colorImage.width);
        bbox.minY = min(bbox.minY, y*_colorImage.height);
        readRows++;
    }
    fin.close();
    
    cout << "I've read " << vertices.size() << "vertices" << endl;
    _blackMesh.addVertices(vertices);
    _mesh.addVertices(vertices);
    _mesh.addTexCoords(texCoord);
    
    fin.open( ofToDataPath(fpTriFilename).c_str() ); //open your text file
    
    fin >> numRows;
    while(fin!=NULL) //as long as theres still text to be read
    {
        int x,y,z;
        fin >> x >> y >> z;
        indexs.push_back(x-1);
        indexs.push_back(y-1);
        indexs.push_back(z-1);
    }
    fin.close();
    
    _mesh.addIndices(indexs);
    _blackMesh.addIndices(indexs);
    
    // Set normals
    ofEnableNormalizedTexCoords();
    int nV = _mesh.getNumVertices();
    int nT = _mesh.getNumIndices()/3;
    vector<ofPoint> norm(nV);
    
    for (int t=0; t<nT; t++) {
        int i1 = _mesh.getIndex(3*t);
        int i2 = _mesh.getIndex(3*t+1);
        int i3 = _mesh.getIndex(3*t+2);
        
        const ofPoint &p1 = _mesh.getVertex(i1);
        const ofPoint &p2 = _mesh.getVertex(i2);
        const ofPoint &p3 = _mesh.getVertex(i3);
        
        ofPoint dir = ((p2 -p1).cross(p3-p1)).normalized();
        
        norm[i1] -= dir;
        norm[i2] -= dir;
        norm[i3] -= dir;
    }
    
    for (int i = 0; i < nV; i++) {
        norm[i].normalize();
        ofVec3f v = _mesh.getVertex(i);
        v.z += 0.1;
        _blackMesh.setVertex(i, v);
    }
    
    _mesh.clearNormals();
    _mesh.addNormals(norm);

    
    return true;
}

