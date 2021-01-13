#pragma once

#include "ofMain.h"
#include "ode/ode.h"
#include "myObject.h"
#include "ship.h"

#include "ofxAssimpModelLoader.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    ofEasyCam cam;

    /* These variables are straight from demo_buggy.cpp */
    dWorldID world;
    dSpaceID space;
    dBodyID body[1];
    dJointGroupID contactgroup;
    dGeomID ground;
    dSpaceID car_space;
    dGeomID box[1];
    dGeomID ground_box;
    dReal speed,steer;

    /* The actual implementation of the broadphase collision callback;
     * see below for how this works with the ODE library. This code
     * is straight from nearCallback in demo_buggy.cpp.
     */
    void collide (dGeomID o1, dGeomID o2);

    void drawBox(const dReal*pos_ode, const dQuaternion rot_ode, const dReal*sides_ode);
    void drawCyl(const dReal*pos_ode, const dQuaternion rot_ode, dReal len, dReal rad);

    /// LIGHTS
    ofLight m_light1;

    /// PLAYER
    Ship player;
    bool isPlayerExistent = false; // flag to know whether the player is instantiated or not
    glm::vec3 playerPos;

    ///OBJECTS
    std::vector<MyObject*> myObjects;
};

/* ODE requires a global function to use as the collision callback; this
 * function, combined with the ofApp pointer, allows us to put the collision
 * code within myApp. Look at the .cpp for details of how this works.
 */
static void nearCallback (void *, dGeomID o1, dGeomID o2);
extern ofApp *myApp;
