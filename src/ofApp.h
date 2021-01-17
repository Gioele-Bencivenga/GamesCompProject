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

    /**
     * @brief We use the keyPressed event to keep track of keys being held down.
     * We store whether they are 1 or 0 in the keys[] array.
     * We can query the array in update() to see if we need to act
     * on keypresses, though can still handle 'q' for quit here.
     * @param _key the key that's being pressed.
     */
    void keyPressed(int _key);
    /**
     * @brief When a key is released we set it back to 0 in the keys array.
     * @param _key the key that was released.
     */
    void keyReleased(int _key);
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

    /**
     * @brief Array to keep track of whether keys are being pressed.
     * 65536 potential character codes with UTF encoding.
     */
    unsigned int keys[65536];
    /**
     * @brief Listens for the keys and does what has to be done when the keys are pressed.
     */
    void getInput();

    //void placeBlock(int _x, int _y, int _z, );

    /**
     * @brief The actual implementation of the broadphase collision callback.
     * See below for how this works with the ODE library.
     */
    void collide (dGeomID o1, dGeomID o2);

    void drawBox(const dReal*pos_ode, const dQuaternion rot_ode, const dReal*sides_ode);
    void drawCyl(const dReal*pos_ode, const dQuaternion rot_ode, dReal len, dReal rad);

    /// FONTS
    ofTrueTypeFont myFont;

    /// LIGHTS
    ofLight m_light1;

    /// SOUNDS
    ofSoundPlayer musicPlayer;
    ofSoundPlayer deathSound;
    ofSoundPlayer winSound;

    /// PLAYER
    Ship player;
    bool isPlayerExistent = false; // flag to know whether the player is instantiated or not
    const dReal* currPlayerPos;
    int resetNumber = 0;

    /**
     * @brief number of block in the path the player is over.
     * Warning: will often be 0 as the player is often over himself.
     */
    int playerOverBlock;

    ///OBJECTS
    std::vector<MyObject*> myObjects;

    /// TUTORIAL
    bool wPressed = false, aPressed = false, sPressed = false, dPressed = false;
};

/* ODE requires a global function to use as the collision callback; this
 * function, combined with the ofApp pointer, allows us to put the collision
 * code within myApp. Look at the .cpp for details of how this works.
 */
static void nearCallback (void *, dGeomID o1, dGeomID o2);
extern ofApp *myApp;
