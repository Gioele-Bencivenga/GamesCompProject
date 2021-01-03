#include "ode/ode.h"
#include "ofApp.h"
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

#ifndef MYOBJECT_H // include/ifdef guard to avoid double inclusion problems
#define MYOBJECT_H

/**
 * @brief This class wraps an ODE physics entity and its graphics elements together into a single game object.
 * Thanks to Adam Stanton for providing it.
 */
class myObject
{
public:
    myObject(float _x, float _y, float _z, dWorldID _world, dSpaceID _space);

    // ODE objects
    dBodyID objBody;
    dMass objMass;
    dGeomID objGeom;

    // 3D model
    ofxAssimpModelLoader objModel;

    // attributes
    float x, y, z;
    ofQuaternion objRotation;

    void setPosition(float _x, float _y, float _z);
    void draw();

    bool debugDraw = false;

    // object's length, width and height
    const float objLength = 1, objWidth = 1, objHeight = 0.1;
};

#endif // MYOBJECT_H
