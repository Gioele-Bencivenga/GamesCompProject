#include "ship.h"

Ship::Ship(){
    MyObject();
}

Ship::Ship(float _x, float _y, float _z, float _length, float _width, float _height, ofQuaternion _rotation, dWorldID _world, dSpaceID _space) : MyObject(_x, _y, _z, _length, _width, _height, _rotation, _world, _space)
{
    // movement stuff
    maxSpeed = 1, maxSteer = 0.3;
    speed = 0, steer = 0;
    // model
    setModel("ship_speederA.dae");
    // light?
    shipLight.setPosition(this->getModel().getPosition());
    shipLight.enable();

    dBodySetMaxAngularSpeed(objBody, 0.6);
    dBodySetDamping(objBody, 0.03, 0.15);
}

// found on http://ode.org/wikiold/htmlfile156.html
void Ship::alignToZAxis( )
{
    dBodyID bodyID = objBody;
    const dReal *rot = dBodyGetAngularVel( bodyID );
    const dReal *quat_ptr;
    dReal quat[4], quat_len;
    quat_ptr = dBodyGetQuaternion( bodyID );
    quat[0] = quat_ptr[0];
    quat[1] = 0;
    quat[2] = 0;
    quat[3] = quat_ptr[3];
    quat_len = sqrt( quat[0] * quat[0] + quat[3] * quat[3] );
    quat[0] /= quat_len;
    quat[3] /= quat_len;
    dBodySetQuaternion( bodyID, quat );
    dBodySetAngularVel( bodyID, 0, 0, rot[2] );
}

void Ship::updateMovement()
{
    alignToZAxis();

    currentVelocity = dBodyGetLinearVel(objBody);
    currentAngularVelocity = dBodyGetAngularVel(objBody);
    currentRotation = dBodyGetRotation(objBody);

    const float* yPos = dBodyGetPosition(objBody);
    cout << yPos[0] << endl;

    /// UPWARDS
    if(lift == true)
    {
        if(currentVelocity[2] < 1)
        {
            dBodyAddForce(objBody, 0, 0, 0.8);
        }
    }

    /// FORWARD/BACKWARDS
    if(speed == maxSpeed)
    {
        dBodyAddRelForce(objBody, 0, maxSpeed, 0);
    }
    else if(speed == -maxSpeed)
    {
        dBodyAddRelForce(objBody, 0, -maxSpeed, 0);
    }
    else
    {
    }

    /// LEFT/RIGHT
    if(steer == maxSteer) // steering left
    {
        //dBodySetRotation(objBody, currentRotation);

        //dBodyAddRelTorque(objBody, -0.002, 0, 0);
            dBodyAddRelTorque(objBody, 0, 0.1, maxSteer);
    }
    else if(steer == -maxSteer) // steering right
    {
        //dBodyAddRelTorque(objBody, 0.002, 0, 0);
            dBodyAddRelTorque(objBody, 0, 0.1, -maxSteer);
    }
    else // if we are not steering
    {
        // the ship slows down its rotation
        if(currentAngularVelocity[2] > 0) // turning right?
        {
            //dBodyAddTorque(objBody, 0, 0, -0.01); // contrast force
        }
        else if (currentAngularVelocity[2] < 0) // turning left?
        {
            //dBodyAddTorque(objBody, 0, 0, 0.01); // contrast force
        }
    }
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
