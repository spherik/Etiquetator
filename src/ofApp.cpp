#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    
    
    
    
    
    bbox.maxX = std::numeric_limits<float>::min();
    bbox.maxY = std::numeric_limits<float>::min();
    bbox.minX = std::numeric_limits<float>::max();
    bbox.minY = std::numeric_limits<float>::max();
    
    _useRamanan = true;
    
    // Load image
    //_path = "/Users/spherik//Documents/WORK/PhD/Bosphorus/originals/bs000/bs000_E_FEAR_0.obj";
    //_path = "/home/spherik/Documents/PhD/Bosphorus/originals/bs000/bs000_E_FEAR_0.obj";
    //_path = "/Volumes/Dades/spherik/Documents/WORK/PhD/Bosphorus/originals/bs000/bs000_E_ANGER_0.obj";
    _path = "/Volumes/Dades/spherik/Documents/WORK/PhD/Bosphorus/originals/";
    
    ofDirectory dir(_path);
    dir.listDir();
    std::vector< ofFile > dirs = dir.getFiles();
    
    for (int i = 0; i <  dirs.size(); i++) {
        if (dirs.at(i).isDirectory()) {
            _subjectsList.push_back(dirs.at(i));
        }
    }
    
    _subjectIdx = 0;
    _expressionIdx = 0;
    _subject = _subjectsList[_subjectIdx].getFileName();
    
    dir.allowExt("obj");
    dir.listDir(_path+"/"+_subject);
    _expressionsList = dir.getFiles();
    _expression = _expressionsList.at(_expressionIdx).getFileName();
    
    std::cout << "Loading " << _path+_subject+"/"+_expression << std::endl;
    
    if(!loadMesh(_path+_subject+"/"+_expression))
        //if(!loadMesh("/Volumes/Dades/spherik/Documents/WORK/PhD/Bosphorus/originals/bs000/bs000_E_ANGER_0.obj"))
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
    //testCam.enableOrtho();
    testCam.setPosition(bbox.minX+(bbox.maxX-bbox.minX)/2.0,bbox.minY+(bbox.maxY-bbox.minY)/2.0,distance);
    testCam.lookAt(ofVec3f(bbox.minX+(bbox.maxX-bbox.minX)/2.0,bbox.minY+(bbox.maxY-bbox.minY)/2.0,0.0), ofVec3f(0.0,1.0,0.0));
    
    cout << "MVP: " << testCam.getModelViewProjectionMatrix() << endl << "------------------------" << endl;
    
    //ofEnableDepthTest();
    
    cout << "Setup Mesh has :" << _mesh.getNumVertices() << endl;
    
    
    setupUI();
    
}
//--------------------------------------------------------------
void ofApp::setupUI()
{
    string expressionString = ofSplitString(_expression,"_")[2];
    
    /*******************************************/
    gui = new ofxUICanvas();
    gui->addLabel("SUBJECT", "SUBJECT: "+_subject,  OFX_UI_FONT_LARGE);
    gui->addSpacer();
    //gui->addLabel("BUTTONS");
    gui->addLabelButton("NEXT S.", false);
    gui->addLabelButton("PREV S.", false);
    gui->addSpacer();
    gui->addLabel("EXPRESSION", "EXPRESSION: "+expressionString,  OFX_UI_FONT_LARGE);
    gui->addLabelButton("NEXT E.", false);
    gui->addLabelButton("PREV E.", false);
    gui->addSpacer();
    
    gui->autoSizeToFitWidgets();
    
    ofAddListener(gui->newGUIEvent,this,&ofApp::guiEvent);
    /***********************************************/
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    //ofTrueTypeFont ttf;
    //ttf.loadFont("verdana.ttf", 32, true, true, true);
    ofSetColor(255);
    //ofDrawBitmapString("FPS: "+ ofToString((int) ofGetFrameRate()), 10, 20);
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
    //cout << "Dragged: " << x << ", " << y << " button: " << button << std::endl;
    
    if(_pickedId>-1 && button == 0) // There's something picked and Left button: vertex movement
    {
        ofVec3f newPos = testCam.screenToWorld(ofVec3f(x,ofGetHeight()-y,0.0/*0.999333*/), ofGetCurrentViewport());
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
    else if (button == 1) // Wheel button: pan
    {
        ofPoint displacement(x-this->panStart.x,y-this->panStart.y);
        this->panStart.x = x;
        this->panStart.y = y;
        //std::cout << "Dragged Mouse point " << x << ", " << y << ". PanStart: " << displacement.x << ", " << displacement.y << std::endl;
        testCam.setPosition(testCam.getPosition().x+displacement.x,
                            testCam.getPosition().y-displacement.y,
                            testCam.getPosition().z);
        
        std::cout << "testCam.setPosition " << testCam.getPosition().x << ", " << testCam.getPosition().y <<  std::endl;
        
    }
    else if (button == 2)   // Right button: zoom
    {
        ofPoint displacement(x-this->panStart.x,y-this->panStart.y);
        
        
        //std::cout << "Dragged Mouse point " << x << ", " << y << ". PanStart: " << displacement.x << ", " << displacement.y << std::endl;
        testCam.setPosition(testCam.getPosition().x,
                            testCam.getPosition().y,
                            testCam.getPosition().z-displacement.y);
        
        this->panStart.x = x;
        this->panStart.y = y;
        
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    if(!gui->isHit(x,y))    // We don't want to process gui events http://forum.openframeworks.cc/t/how-to-get-quot-event-capture-quot-with-ofxui/10685/2
    {
        if(button == 0) // Left mouse button
        {
            _pickedId = glSelect(x,y);
        }
        else if (button == 1 || button == 2) // Wheel mouse button
        {
            this->panStart.x = x;
            this->panStart.y = y;
            //std::cout << "Pressed Mouse point " << x << ", " << y << ". PanStart: " << this->panStart.x << ", " << this->panStart.y << std::endl;
        }
    }
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

//--------------------------------------------------------------
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
//--------------------------------------------------------------
bool ofApp::loadMesh(string filename)
{
    string imageFilename = filename;
    
    ofStringReplace(imageFilename, "originals", "images");
    ofStringReplace(imageFilename, ".obj", ".png");
    
    _colorImage.loadImage(imageFilename);
    
    
    if(_blackMesh.getVertices().size()>0)
    {
        _blackMesh.clear();
        
    }
    /*if(_verticesSpheres.size()>0)
     {
     _verticesSpheres.clear();
     }*/
    
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
        cout << fpCoordFilename << endl;
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
//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    bool update = false;
    string name = e.widget->getName();
    int kind = e.widget->getKind();
    
    if(name == "NEXT S." )
    {
        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
        
        if(button->getValue()) // If is pressed
        {
            _subjectIdx++;
            if (_subjectIdx == _subjectsList.size()) {
                _subjectIdx = 0;
            }
            update = true;
        }
    }
    else if(name == "PREV S.")
    {
        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
        
        if(button->getValue()) // If is pressed
        {
            _subjectIdx--;
            if(_subjectIdx<0)
                _subjectIdx = _subjectsList.size()-1;
            update = true;
        }
    }
    else if(name == "NEXT E.")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        cout << "value: " << toggle->getValue() << endl;
    }
    else if(name == "PREV E.")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        cout << "value: " << toggle->getValue() << endl;
    }
    
    
    if(update)
    {
        ofDirectory dir;
        
        // Update subject info on GUI
        _subject = _subjectsList[_subjectIdx].getFileName();
        ((ofxUILabel*)gui->getWidget("SUBJECT"))->setLabel("SUBJECT: "+_subject);
        
        
        // Update expression info on GUI
        dir.allowExt("obj");
        dir.listDir(_path+"/"+_subject);
        _expressionsList = dir.getFiles();
        _expression = _expressionsList[_expressionIdx].getFileName();
        ((ofxUILabel*)gui->getWidget("EXPRESSION"))->setLabel("EXPRESSION: "+_expression);
        
        if(!loadMesh(_path+"/"+_subject+"/"+_expression))
            //if(!loadMesh("/Volumes/Dades/spherik/Documents/WORK/PhD/Bosphorus/originals/bs000/bs000_E_ANGER_0.obj"))
        {
            ofExit();
        }
        
    }
    
}