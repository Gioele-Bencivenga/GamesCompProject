#include "ship.h"

Ship::Ship(){
    MyObject();
}

Ship::Ship(float _x, float _y, float _z, float _length, float _width, float _height, ofQuaternion _rotation, dWorldID _world, dSpaceID _space) : MyObject(_x, _y, _z, _length, _width, _height, _rotation, _world, _space)
{
    // movement stuff
    maxSpeed = 1, maxSteer = 0.2;
    speed = 0, steer = 0;
    // model
    setModel("ship_speederA.dae");
    // light?
    shipLight.setPosition(this->getModel().getPosition());
    shipLight.enable();
}

void Ship::updateMovement()
{
    currentVelocity = dBodyGetLinearVel(objBody);
    currentAngularVelocity = dBodyGetAngularVel(objBody);
    currentRotation = dBodyGetRotation(objBody);

    cout << currentRotation[6] << endl;

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
        if(currentVelocity[1] < 5)
        {
            dBodyAddRelForce(objBody, 0, maxSpeed, 0);
        }
    }
    else if(speed == -maxSpeed)
    {
        if(currentVelocity[1] > -5)
        {
            dBodyAddRelForce(objBody, 0, -maxSpeed, 0);
        }
    }
    else
    {
    }

    /// LEFT/RIGHT
    if(steer == maxSteer) // steering left
    {
        if(currentAngularVelocity[2] < 1.5) // limit velocity
        {
            dBodyAddRelTorque(objBody, -0.001, 0, 0);
            //dBodyAddTorque(objBody, 0, -0, maxSteer);
        }
    }
    else if(steer == -maxSteer) // steering right
    {
        if(currentAngularVelocity[2] > -1.5) // limit
        {
            dBodyAddRelTorque(objBody, 0.001, 0, 0);
            //dBodyAddTorque(objBody, 0, -0, -maxSteer);
        }
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

    /// AUTO ROTATION
    if(currentRotation[2] > 0.1)
    {
        if(currentAngularVelocity[1] < 0.6)
        {
            dBodyAddRelTorque(objBody, 0, -0.05, 0);
        }
        else if(currentAngularVelocity[1] > -0.6)
        {
            dBodyAddRelTorque(objBody, 0, 0.05, 0);
        }
    }
    else if(currentRotation[2] < -0.1)
    {
        if(currentAngularVelocity[1] < 0.6)
        {
            dBodyAddRelTorque(objBody, 0, 0.05, 0);
        }
        else if(currentAngularVelocity[1] > -0.6)
        {
            dBodyAddRelTorque(objBody, 0, -0.05, 0);
        }
    }
    // front/back rotation
    if(currentRotation[5] < 0.6) // tilted too high up
    {
        //dBodyAddRelTorque(objBody, -0.1, 0, 0);
    }
    else if(currentRotation[5] > 0.95)
    {

    }

    /// ROLL DAMPENER
    if(currentAngularVelocity[1] < 0) // rolling left
    {
        if(currentRotation[2] < 0) // leaning left
        {
            dBodyAddRelTorque(objBody, 0, 0.05, 0); // contrast the rolling left
        }
        else if(currentRotation[2] > 0) // leaning right
        {
            // do nothing
        }
    }
    else if(currentAngularVelocity[1] > 0) // rolling right
    {
        if(currentRotation[2] > 0) // leaning right
        {
            dBodyAddRelTorque(objBody, 0, -0.05, 0); // contrast the rolling right
        }
        else if(currentRotation[2] < 0) // leaning left
        {
            // do nothing
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
