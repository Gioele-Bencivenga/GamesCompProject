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
    dWorldSetGravity(world,0,0,-1.77);

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

    // setup font
    myFont.load(OF_TTF_SANS, 64, true, true, true);
    barFont.load(OF_TTF_SANS, 32, true, true, true);

    // setup input keys
    for(unsigned int i=0; i<65536; i++) keys[i] = 0;

    // player
    player = Ship(0, 0, 0, 1.3, 0.7, 0.3, *new ofQuaternion(0, 0, 0, 0), world, space);
    myObjects.push_back(&player);
    isPlayerExistent = true;
    currPlayerPos = dBodyGetPosition(player.objBody);

    // light
    m_light1.setPosition(0,currPlayerPos[1] + 100, currPlayerPos[2] + 400);
    //m_light1.lookAt(glm::vec3(0,0,0));
    m_light1.enable();

    // sounds
    musicPlayer.load("music.mp3");
    musicPlayer.setLoop(true);
    musicPlayer.setVolume(0.2);
    deathSound.load("sounds_death.wav");
    deathSound.setVolume(1);
    //winSound.load("sounds_win.wav");
    //winSound.setVolume(0.8);

    // start block
    MyObject* startBlock = new MyObject(currPlayerPos[0], currPlayerPos[1], currPlayerPos[2] - 15,
            150, 200, 50,
            *new ofQuaternion(0, 0, 0, 0), world, space);
    dRFromAxisAndAngle(R, 1, 0, 0, -0.01);
    dBodySetRotation(startBlock->objBody, R);
    dBodySetKinematic(startBlock->objBody);
    myObjects.push_back(startBlock);

    playerOverBlock = 0;

    unsigned int pathSize = 248;
    // path creation algorithm
    for(unsigned int i=0; i < pathSize; i++)
    {
        MyObject* prevBlock = myObjects[i+1]; // get the last placed block
        const dReal* prevBlockPos = dBodyGetPosition(prevBlock->objBody); // get its position
        // create new block in position relative to previous
        MyObject* blockToAdd = new MyObject(prevBlockPos[0] + ofRandom(-80, 80), prevBlockPos[1] + prevBlock->objWidth, prevBlockPos[2] - ofRandom(-20, 85),
                ofRandom(5, 150), ofRandom(5, 150), ofRandom(5, 60),
                *new ofQuaternion(0, 0, 0, 0), world, space);

        float rot = 0;
        if(i > 15 && i <= 25)
            rot = 0.12f;
        if(i > 25 && i <= 35)
            rot = 0.25f;
        if(i > 35 && i <= 45)
            rot = 0.34f;
        if(i > 45 && i <= 55)
            rot = 0.45f;
        if(i > 55 && i <= 65)
            rot = 0.55f;
        if(i > 65)
            rot = 0.65f;

        dRFromAxisAndAngle(R, 1, 1, 0, ofRandom(-rot, rot));
        dBodySetRotation(blockToAdd->objBody, R);
        dBodySetKinematic(blockToAdd->objBody); // set stationary
        myObjects.push_back(blockToAdd); // add to group

        if(i == pathSize - 1) // last block
        {
            MyObject* blockToAdd = new MyObject(prevBlockPos[0], prevBlockPos[1] + prevBlock->objWidth, prevBlockPos[2],
                    200, 200, 80,
                    *new ofQuaternion(0, 0, 0, 0), world, space);

            dRFromAxisAndAngle(R, 0, 0, 0, 0);
            dBodySetRotation(blockToAdd->objBody, R);
            dBodySetKinematic(blockToAdd->objBody); // set stationary
            myObjects.push_back(blockToAdd); // add to group
        }
    }

    musicPlayer.play();
}

//--------------------------------------------------------------
void ofApp::update()
{
    if(isPlayerExistent){
        getInput();

        currPlayerPos = dBodyGetPosition(player.objBody);
        cam.setPosition(currPlayerPos[0], currPlayerPos[1]-13, currPlayerPos[2]+9.5);
        cam.lookAt(glm::vec3(currPlayerPos[0], currPlayerPos[1], currPlayerPos[2]), ofVec3f(0, 0, 1));

        for(unsigned int i=0; i < myObjects.size(); i++)
        {
            const float* currObjPos = dBodyGetPosition(myObjects[i]->objBody);

            // keep track of which block the player reached
            if(currPlayerPos[1] > currObjPos[1])
            {
                playerOverBlock = i;
            }
        }

        const float* currBlockPos = dBodyGetPosition(myObjects[playerOverBlock]->objBody);
        if(currPlayerPos[2] < currBlockPos[2] - 300)
        {
            resetNumber++;

            if(!deathSound.isPlaying())
                deathSound.play();

            // reset player
            dBodySetPosition(player.objBody, 0, 0, 0);
            dMatrix3 R;
            dRFromAxisAndAngle(R, 0, 0, 0, 0);
            dBodySetRotation(player.objBody, R);
            player.liftAmount = player.maxLiftAmount;
            // reset world
            for(unsigned int i=0; i < myObjects.size(); i++)
            {
                myObjects[i]->assignColour();
            }
        }

        if(playerOverBlock == myObjects.size() -1)
        {
            playerWon = true;
            isPlayerExistent = false;

            tot = myObjects.size()-1;
            perc = (blocksAbsorbed/tot)*100;
            perc = (blocksAbsorbed / static_cast<float>(tot)) * 100;
            perc = 100 - perc;
        }
    }

    dSpaceCollide (space,0,&nearCallback);
    dWorldStep (world, 0.25);
    // remove all contact joints
    dJointGroupEmpty (contactgroup);
}

void ofApp::getInput(){
    player.speed = 0;
    player.steer = 0;

    if(player.speed < player.maxSpeed)
    {
        if (keys[87] || keys[119] || keys[OF_KEY_UP]) // W/w/UP
        {
            player.speed = player.maxSpeed;

            if(wPressed == false) // stop displaying tutorial
                wPressed = true;
        }
    }

    if(player.speed > -player.maxSpeed)
    {
        if (keys[83] || keys[115] || keys[OF_KEY_DOWN]) // S/s/DOWN
        {
            player.speed = -player.maxSpeed;

            if(sPressed == false) // stop displaying tutorial
                sPressed = true;
        }
    }

    if(player.steer < player.maxSteer)
    {
        if (keys[68] || keys[100] || keys[OF_KEY_RIGHT]) // D/d/RIGHT
        {
            player.steer = -player.maxSteer;

            if(dPressed == false) // stop displaying tutorial
                dPressed = true;
        }
    }

    if(player.steer > -player.maxSteer)
    {
        if (keys[65] || keys[97] || keys[OF_KEY_LEFT]) // A/a/LEFT
        {
            player.steer = player.maxSteer;

            if(aPressed == false) // stop displaying tutorial
                aPressed = true;
        }
    }

    if(keys[32]){ // SPACE
        player.lift = true;
    }else{
        player.lift = false;
    }

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
    ofBackground(179, 242, 255);
    cam.begin();

    ofEnableDepthTest();

    ofPushMatrix();

    for(auto x: myObjects ) x->draw();

    ofDisableDepthTest();
    cam.end();

    ofPushMatrix();

    /// INTERFACE
    ofSetColor(ofColor::black);
    ofDrawRectangle(10, 10, 0, player.maxLiftAmount, 100);

    ofSetColor(ofColor::yellow);
    ofDrawRectangle(10, 20, 0, player.liftAmount, 80);

    ofSetColor(ofColor::black);
    barFont.drawString("Energy", 40, 80);

    /// TUTORIAL
    if(wPressed == false){
        ofSetColor(ofColor::white);
        myFont.drawString("W/up to go forward", (ofGetWindowWidth()/2) - 400, 100);
    }

    if(aPressed == false){
        ofSetColor(ofColor::white);
        myFont.drawString("A/left to turn left", 20, ofGetWindowHeight()/2 - 10);
    }

    if(sPressed == false){
        ofSetColor(ofColor::white);
        myFont.drawString("S/down to go backwards", (ofGetWindowWidth()/2) - 400, ofGetWindowHeight() - 150);
    }

    if(dPressed == false){
        ofSetColor(ofColor::white);
        myFont.drawString("D/right to turn right", ofGetWindowWidth() - 800, ofGetWindowHeight()/2 - 10);
    }

    if(resetNumber == 0 &&(playerOverBlock > 0 && playerOverBlock < 8)){
        ofSetColor(ofColor::black);
        myFont.drawString("Reach the last block in the path!", 40, ofGetWindowHeight()/4);
    }

    if(resetNumber == 0 &&(playerOverBlock > 1 && playerOverBlock < 14))
    {
        ofSetColor(ofColor::black);
        myFont.drawString("Press SPACE to fly, uses energy (up left)", 40, ofGetWindowHeight()/4+120);
    }

    if(resetNumber == 0 &&(playerOverBlock > 5 && playerOverBlock < 20))
    {
        ofSetColor(ofColor::black);
        myFont.drawString("Touch coloured boxes to replenish your energy!", 40, ofGetWindowHeight()/4+240);
    }

    if(resetNumber == 0 &&(playerOverBlock > 8 && playerOverBlock < 23))
    {
        ofSetColor(ofColor::black);
        myFont.drawString("Keep your energy and reach the end by\nabsorbing as little boxes as possible!", 40, ofGetWindowHeight()/4+360);
    }

    if(resetNumber > 0 &&(playerOverBlock >= 0 && playerOverBlock < 2))
    {
        ofSetColor(ofColor::black);
        myFont.drawString("You fell off the path!\nHappens to the best,\nBetter luck this time!", 40, ofGetWindowHeight()/4);
    }

    /// WIN
    if(playerWon == true){
        ofSetColor(ofColor::black);
        myFont.drawString("Congratulations, You Won! Great job.", (ofGetWindowWidth()/2) - 400, (ofGetWindowHeight()/3));
        barFont.drawString("Your performance\nTotal boxes: " + to_string(tot), (ofGetWindowWidth()/2) - 400, (ofGetWindowHeight()/3) + 100);
        barFont.drawString("Absorbed boxes: " + to_string(blocksAbsorbed), (ofGetWindowWidth()/2) - 400, (ofGetWindowHeight()/3) + 200);
        barFont.drawString("Rating: " + to_string(perc) + "%!", (ofGetWindowWidth()/2) - 400, (ofGetWindowHeight()/3) + 300);
    }

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

void ofApp::collide(dGeomID geom1, dGeomID geom2)
{
    const int N = 15;
    dContact contact[N];

    int i,n;
    n = dCollide(geom1, geom2, N, &contact[0].geom,sizeof(dContact));
    if (n > 0) {
        for (i=0; i<n; i++) {
            contact[i].surface.mode = dContactSlip1 | dContactSlip2 |
                    dContactSoftERP | dContactSoftCFM | dContactApprox1 | dContactBounce;
            contact[i].surface.mu = 0.0001;
            contact[i].surface.slip1 = 0.1;
            contact[i].surface.slip2 = 0.1;
            contact[i].surface.soft_erp = 0.5;
            contact[i].surface.soft_cfm = 0.001;
            contact[i].surface.bounce = 2;
            dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);
            dJointAttach (c,
                          dGeomGetBody(contact[i].geom.g1),
                          dGeomGetBody(contact[i].geom.g2));
        }

        for(unsigned int i = 0; i < myObjects.size(); i++){
            if((geom1 == player.objGeom && geom2 == myObjects[i]->objGeom))
            {
                if(myObjects[i]->objColour != ofColor::black)
                {
                    if(player.liftAmount < player.maxLiftAmount)
                    {
                        myObjects[i]->objColour.set(ofColor::black);
                        if(i == myObjects.size()-1)
                            myObjects[i]->objColour.set(ofColor::white);
                        player.rechargeSound.play();
                        player.liftAmount = player.maxLiftAmount;
                        blocksAbsorbed++;
                    }
                }
            }
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
