#include "myObject.h"
#include "ode/ode.h"

MyObject::MyObject(){}

/**
 * @brief Creates the object with its physical and graphical components according to passed values.
 * @param _x x position where the object will be created
 * @param _y y position where the object will be created
 * @param _z z position where the object will be created
 * @param _world world where the object will be created
 * @param _space space where the object will be created?
 */
MyObject::MyObject(float _x, float _y, float _z,
                   float _length, float _width, float _height,
                   ofQuaternion _rotation, dWorldID _world, dSpaceID _space)
{
    setPosition(_x, _y, _z);

    objLength = _length, objWidth = _width, objHeight = _height;
    objRotation = _rotation;

    setupPhysics(_x, _y, _z, _world, _space);

    setModel("alien.dae");

    // colour randomly assigned
    objColour.set(ofRandom(1, 254), ofRandom(1, 254), ofRandom(1, 254));
}

void MyObject::setupPhysics(float _x, float _y, float _z, dWorldID _world, dSpaceID _space){
    // physics objs setup
    objBody = dBodyCreate(_world);
    dBodySetPosition(objBody, _x, _y, _z);
    dMassSetBox(&objMass, 1, objLength, objWidth, objHeight);
    dMassAdjust(&objMass, 1);
    dBodySetMass(objBody, &objMass);
    objGeom = dCreateBox(_space, objLength, objWidth, objHeight);
    dGeomSetBody(objGeom, objBody);
}

void MyObject::setPosition(float _x, float _y, float _z)
{
    x = _x; y = _y; z = _z;
}

dBodyID MyObject::getBody(){
    return objBody;
}

ofxAssimpModelLoader MyObject::getModel()
{
    return objModel;
}

void MyObject::setModel(string _modelName){
    // load model
    objModel.loadModel(_modelName, true);

    // adjust model orientation
    double scale = 1.0 / objModel.getNormalizedScale();
    objModel.setScale(scale, scale, scale);
    objModel.setRotation(0, 90.0, 1, 0, 0);
    objModel.setPosition(objModel.getPosition().x, objModel.getPosition().y + 0.35, objModel.getPosition().z - 0.3);
}

void MyObject::draw()
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
        ofSetColor(objColour, 256);
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
