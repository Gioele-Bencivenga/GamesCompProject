#include "myObject.h"
#include "ode/ode.h"

/**
 * @brief Creates the object with its physical and graphical components according to passed values.
 * @param _x x position where the object will be created
 * @param _y y position where the object will be created
 * @param _z z position where the object will be created
 * @param _world world where the object will be created
 * @param _space space where the object will be created?
 */
myObject::myObject(float _x, float _y, float _z, dWorldID _world, dSpaceID _space)
{
    // assign passed in values to our vars
    x = _x; y = _y; z = _z;

    // physics objs setup
    objBody = dBodyCreate(_world);
    dBodySetPosition(objBody, _x, _y, _z);
    dMassSetBox(&objMass, 1, objLength, objWidth, objHeight);
    dMassAdjust(&objMass, 1);
    dBodySetMass(objBody,&objMass);
    objGeom = dCreateBox(_space, objLength, objWidth, objHeight);
    dGeomSetBody(objGeom, objBody);

    // graphic objs setup
    objModel.loadModel("ship_cargoA.dae", 20);
    double scale = 1.0 / objModel.getNormalizedScale();

    objModel.setScale(scale, scale * .5, scale);
    objModel.setRotation(0, 90.0, 1, 0, 0);
}

/**
 * @brief Sets the object's position.
 * @param _x the new x position
 * @param _y the new y position
 * @param _z the new z position
 */
void myObject::setPosition(float _x, float _y, float _z)
{
    x = _x; y = _y; z = _z;
}

/**
 * @brief The draw method pulls the latest data from ODE and draws the model at that location and orientation.
 * Thanks to Adam Stanton for providing it.
 */
void PalletObject::draw()
{
    /* Get data from ODE */
    const dReal* thePos = dBodyGetPosition(m_body);
    const dReal* oderot = dBodyGetQuaternion(m_body);

    /* Set the position (of this PalletObject object) */
    setPosition(thePos[0],thePos[1], thePos[2]);

    /* Get ODEs rotation quaternion, convert it to an OF one,
     * and then collect the angle and vector from that: */
    m_rotation = ofQuaternion(oderot[1], oderot[2], oderot[3], oderot[0]);
    float rotationAmount;
    ofVec3f rotationAngle;
    m_rotation.getRotate(rotationAmount, rotationAngle);

    /* We can draw a transparent white box where the ODE object is, to
     * make sure that our model is being drawn approximately correctly */
    if(debug_draw) {
        ofSetColor(ofColor::white,128);
        /* Save the current state of the graphics transform stack: */
        ofPushMatrix();

        /* Translate to the correct position: */
        ofTranslate(x,y,z);

        /* Rotate by the correct amount: */
        ofRotateDeg(rotationAmount, rotationAngle.x, rotationAngle.y, rotationAngle.z);

        /* Draw the box */
        ofDrawBox(1,1,0.1);

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
    m_model.drawFaces();

    /* Restore the transformation matrix: */
    ofPopMatrix();
}
