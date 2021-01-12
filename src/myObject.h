#include "ode/ode.h"
#include "ofApp.h"
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

#ifndef MYOBJECT_H // include/ifdef guard to avoid double inclusion problems
#define MYOBJECT_H

/**
 * @brief This class wraps an ODE physics entity and its graphics elements together into a single game object.
 * Thanks to Adam Stanton for providing it.
 *
 * In this program this represents a simple object and it's the base upon which the ship class is build.
 */
class MyObject
{
public:
    MyObject(float _x, float _y, float _z,
             float _length, float _width, float _height,
             ofQuaternion _rotation, dWorldID _world, dSpaceID _space);

    /// ODE OBJECTS
    dBodyID objBody;
    dMass objMass;
    dGeomID objGeom;
    dSpaceID objSpace;

    /// GRAPHICS
    ofxAssimpModelLoader objModel;
    bool drawObject = true; // whether we draw this object's hitbox or not
    ofColor objColour;

    ///ATTRIBUTES
    float x, y, z;
    ofQuaternion objRotation;

    /// LENGTH, WIDTH, HEIGHT
    float objLength = 0.1, objWidth = 0.1, objHeight = 0.1;

    /// METHODS
    /**
     * @brief Sets the object's position.
     * @param _x the new x position
     * @param _y the new y position
     * @param _z the new z position
     */
    void setPosition(float _x, float _y, float _z);
    /**
     * @brief Gets the object's body
     */
    dBodyID getBody();
    /**
     * @brief Gets the object's model
     */
    ofxAssimpModelLoader getModel();
    /**
     * @brief Sets the object's model and adjusts its orientation and scale.
     * @param _modelName the filename of the .dae model.
     */
    void setModel(string _modelName);
    /**
     * @brief The draw method pulls the latest data from ODE and draws the model at that location and orientation.
     * Thanks to Adam Stanton for providing it.
     */
    virtual void draw();
};

#endif // MYOBJECT_H
