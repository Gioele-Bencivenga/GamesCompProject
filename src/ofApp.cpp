#include "ofApp.h"
#include "myObject.h"
#include "ship.h"

#define LENGTH 0.7      // chassis length
#define WIDTH 0.5       // chassis width
#define HEIGHT 0.2      // chassis height
#define RADIUS 0.18     // wheel radius
#define STARTZ 0.5      // starting height of chassis
#define CMASS 1         // chassis mass
#define WMASS 0.2       // wheel mass

static const dVector3 yunit = { 0, 1, 0 }, zunit = { 0, 0, 1 };

//--------------------------------------------------------------
void ofApp::setup(){
    /* Ensure texture sizes are normalized */
    /* Note that textures must be square! */
    ofDisableArbTex();

    int i;
    dMass m;

    // create world
    dInitODE2(0);
    world = dWorldCreate();
    space = dHashSpaceCreate(0);
    contactgroup = dJointGroupCreate(0);
    dWorldSetGravity(world,0,0,-0.5);
    ground = dCreatePlane (space,0,0,1,0);

    // chassis body
    body[0] = dBodyCreate (world);
    dBodySetPosition (body[0],0,0,STARTZ);
    dMassSetBox (&m,1,LENGTH,WIDTH,HEIGHT);
    dMassAdjust (&m,CMASS);
    dBodySetMass (body[0],&m);
    box[0] = dCreateBox (0,LENGTH,WIDTH,HEIGHT);
    dGeomSetBody (box[0],body[0]);

    // create car space and add it to the top level space
    car_space = dSimpleSpaceCreate(space);
    dSpaceSetCleanup(car_space,0);
    dSpaceAdd(car_space,box[0]);

    // environment
    ground_box = dCreateBox (space,2,1.5,1);
    dMatrix3 R;
    dRFromAxisAndAngle (R,0,1,0,-0.15);
    dGeomSetPosition (ground_box,2,0,-0.34);
    dGeomSetRotation (ground_box,R);

    // Set up the OpenFrameworks camera
    ofVec3f upVector;
    upVector.set(0, 0, 1);
    cam.setAutoDistance(false);
    cam.setNearClip(0.01);
    cam.setPosition(10,10,10);
    //cam.lookAt({0,0,0},upVector);
    cam.setUpAxis(upVector);

    dAllocateODEDataForThread(dAllocateMaskAll);

    // light
    m_light1.setPosition(0,0,20);
    //m_light1.lookAt(glm::vec3(0,0,0));
    m_light1.enable();

    // setup input keys
    for(unsigned int i=0; i<65536; i++) keys[i] = 0;
    // create player
    player = Ship(0, 0, 15, 1.2, 1.2, 0.35, *new ofQuaternion(0, 0, 0, 0), world, space);
    myObjects.push_back(&player);
    isPlayerExistent = true;

    // create some objects
    for(unsigned int p=0; p<60; p++) {
        myObjects.push_back(new MyObject(ofRandom(-30, 30), ofRandom(-30, 30), ofRandom(5, 40),
                                         ofRandom(0.1, 5), ofRandom(0.1, 5), ofRandom(0.1, 5),
                                         *new ofQuaternion(ofRandom(0, 50), 0, 0, 0), world, space));
    }
}

//--------------------------------------------------------------
void ofApp::update(){

    if(isPlayerExistent){
        getInput();
        const dReal *b = dBodyGetPosition(player.getBody());
        cam.setPosition(b[0],b[1]-15,b[2]+7);
        //cam.setTarget(player.objModel.getPosition());
        cam.lookAt(glm::vec3(b[0], b[1], b[2]), ofVec3f(0, 0, 1));
    }

    dSpaceCollide (space,0,&nearCallback);
    dWorldStep (world, 0.1);

    // remove all contact joints
    dJointGroupEmpty (contactgroup);

}

void ofApp::getInput(){
    player.speed = 0;
    player.steer = 0;

    if(player.speed < player.maxSpeed)
        if (keys[87] || keys[119] || keys[OF_KEY_UP]) // W/w/UP
            player.speed = player.maxSpeed;

    if(player.speed > -player.maxSpeed)
        if (keys[83] || keys[115] || keys[OF_KEY_DOWN]) // S/s/DOWN
            player.speed = -player.maxSpeed;

    if(player.steer < player.maxSteer)
        if (keys[68] || keys[100] || keys[OF_KEY_RIGHT]) // D/d/RIGHT
            player.steer = -player.maxSteer;

    if(player.steer > -player.maxSteer)
        if (keys[65] || keys[97] || keys[OF_KEY_LEFT]) // A/a/LEFT
            player.steer = player.maxSteer;

    if(keys[32]){ // SPACE
        player.lift = true;
    }else{
        player.lift = false;
    }

    std::cout << "Lift is " << player.lift << std::endl;

    player.updateMovement();
}

void ofApp::keyPressed(int _key){
    //  There is an OF_KEY_X macro for the keys, but you can also
    //  detect the numeric code and use that if you want to:
    //std::cout << "Keycode " << _key << "was pressed." << std::endl;

    keys[_key] = 1;

    switch(_key) {
    case ' ':
        player.speed = 0;
        player.steer = 0;
        break;
    case 'q':
        ofExit();
        break;
    }
}

void ofApp::keyReleased(int _key){
    keys[_key] = 0;
}

void ofApp::draw(){

    // draw the scene
    ofBackground(20);
    cam.begin();

    ofEnableDepthTest();

    ofPushMatrix();

    ofSetColor(ofColor::lightGrey);

    ofDrawGrid(0.3f,250, false, false, false, true);

    ofDrawAxis(10);

    // chassis
    ofSetColor(ofColor::yellow);
    const dReal sides[3] = {LENGTH,WIDTH,HEIGHT};
    const dReal* pos_ode = dBodyGetPosition(body[0]);
    const dReal* rot_ode = dBodyGetQuaternion(body[0]);
    drawBox(pos_ode, rot_ode, sides);

    // ground box
    ofSetColor(ofColor::blue);
    dVector3 ss; dQuaternion r;
    dGeomBoxGetLengths (ground_box,ss);
    dGeomGetQuaternion(ground_box,r);
    drawBox(dGeomGetPosition(ground_box),r,ss);

    for(auto x: myObjects ) x->draw();

    ofDisableDepthTest();
    cam.end();

    ofPopMatrix();
}

void ofApp::exit() {
    // delete all of the geometries of the objects in the group
    for(unsigned int p=0; p<myObjects.size() - 1; p++) {
        dGeomDestroy(myObjects[p]->objGeom);
    }

    dJointGroupDestroy(contactgroup);
    dSpaceDestroy (space);
    dWorldDestroy (world);
    dCloseODE();
}
//--------------------------------------------------------------
static void nearCallback (void *, dGeomID o1, dGeomID o2) {

    myApp->collide(o1,o2);
}

void ofApp::drawCyl(const dReal*pos_ode, const dQuaternion rot_ode, dReal len, dReal rad)
{
    ofCylinderPrimitive c;

    // ODE's and glm's quaternions are stored in a different order to openFrameworks:
    // ofQuaternion::ofQuaternion(float x, float y, float z, float w)
    // dQuaternion	dReal[4]	[ w, x, y, z ], where w is the real part and (x, y, z) form the vector part.
    // glm::quat(w,x,y,z)
    ofQuaternion rot_of(rot_ode[1], rot_ode[2], rot_ode[3], rot_ode[0]);

    // The OF Cylinder lies along the y axis (its length is along Y); ODE's stands tall in Z.
    // Let's fix that by creating a simple "rotate around X axis by 90 degrees" quaternion:
    ofQuaternion fix_cy; fix_cy.makeRotate(90,1,0,0);

    // And create a final orientation by combining that first rotation with the actual orientation of
    // the cylinder we got from ODE (in of quaternion ordering):
    ofQuaternion rot_final = fix_cy * rot_of;

    // ofCylinder dimensions: its length is 80 and is radius 60
    // std::cout << c.getHeight() << ", " <<c.getRadius() << std::endl;
    // scale it to be unit length and radius * the actual size:
    c.setScale(glm::vec3(rad/60.0,len/80.0,rad/60.0));

    // Use our calculated quaternion to orient the cylinder properly:
    c.setGlobalOrientation(rot_final);

    // Now set the cylinder's position according to ODE physics:
    c.setGlobalPosition(glm::vec3(pos_ode[0],pos_ode[1],pos_ode[2]));

    // Draw it:
    c.draw();
}

void ofApp::drawBox(const dReal*pos_ode, const dQuaternion rot_ode, const dReal*sides_ode)
{
    ofBoxPrimitive b;
    // ofBox dimensions: 100 * 100 * 100
    // std::cout << b.getSize() << std::endl;

    // scale it to be unit w, h, d * the actual size:
    b.setScale(glm::vec3(0.01*sides_ode[0],0.01*sides_ode[1],0.01*sides_ode[2]));

    // Simply set the orientation based on ODE's quaternion. Since we are using glm::quat
    // this time, the ordering is the same as ODE:
    b.setGlobalOrientation(glm::quat(rot_ode[0],rot_ode[1],rot_ode[2],rot_ode[3]));

    // Now set the box's position according to ODE physics:
    b.setGlobalPosition(glm::vec3(pos_ode[0],pos_ode[1],pos_ode[2]));

    // Draw it:
    b.draw();

}

void ofApp::collide(dGeomID o1, dGeomID o2)
{
    //int g1 = (o1 == ground || o1 == ground_box);
    //int g2 = (o2 == ground || o2 == ground_box);
    //if (!(g1 ^ g2)) return; // if we are not colliding with anything we return

    const int N = 15;
    dContact contact[N];
    int i,n;
    n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));
    if (n > 0) {
        for (i=0; i<n; i++) {
            contact[i].surface.mode = dContactSlip1 | dContactSlip2 |
                    dContactSoftERP | dContactSoftCFM | dContactApprox1;
            contact[i].surface.mu = 2;
            contact[i].surface.slip1 = 0.1;
            contact[i].surface.slip2 = 0.1;
            contact[i].surface.soft_erp = 0.5;
            contact[i].surface.soft_cfm = 0.3;
            dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);
            dJointAttach (c,
                          dGeomGetBody(contact[i].geom.g1),
                          dGeomGetBody(contact[i].geom.g2));
        }
    }
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
