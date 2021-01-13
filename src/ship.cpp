#include "ship.h"

Ship::Ship(){
    MyObject();
}

Ship::Ship(float _x, float _y, float _z, float _length, float _width, float _height, ofQuaternion _rotation, dWorldID _world, dSpaceID _space) : MyObject(_x, _y, _z, _length, _width, _height, _rotation, _world, _space)
{
    // movement stuff
    maxSpeed = 10, maxSteer = 2;
    speed=0, steer=0;
    // model
    setModel("ship_speederA.dae");
    // light?
    shipLight.setPosition(this->getModel().getPosition());
    shipLight.enable();
}

void Ship::draw()
{
    /* Get data from ODE */
    const dReal* thePos = dBodyGetPosition(objBody);
    const dReal* oderot = dBodyGetQuaternion(objBody);

    // Set the position of this object)
    setPosition(thePos[0],thePos[1], thePos[2]);

    /* Get ODEs rotation quaternion, convert it to an OF one,
     * and then collect the angle and vector from that: */
    objRotation = ofQuaternion(oderot[1], oderot[2], oderot[3], oderot[0]);
    float rotationAmount;
    ofVec3f rotationAngle;
    objRotation.getRotate(rotationAmount, rotationAngle);

    /* We can draw a transparent white box where the ODE object is, to
     * make sure that our model is being drawn approximately correctly */
    if(drawObject) {
        ofSetColor(ofColor::white,128);
        /* Save the current state of the graphics transform stack: */
        ofPushMatrix();

        /* Translate to the correct position: */
        ofTranslate(x,y,z);

        /* Rotate by the correct amount: */
        ofRotateDeg(rotationAmount, rotationAngle.x, rotationAngle.y, rotationAngle.z);

        /* Draw the box */
        ofDrawBox(objWidth,objLength,objHeight);

        /* Restore the graphics transform stack: */
        ofPopMatrix();
    }
    /* Draw the model */
    /* Save the current state of the graphics transform stack: */
    ofPushMatrix();

    /* Translate to the correct position: */
    ofTranslate(x,y-0.05,z);

    /* Rotate by the correct amount: */
    ofRotateDeg(rotationAmount, rotationAngle.x, rotationAngle.y, rotationAngle.z);

    /* Draw the model: */
    objModel.drawFaces();

    /* Restore the transformation matrix: */
    ofPopMatrix();
}
