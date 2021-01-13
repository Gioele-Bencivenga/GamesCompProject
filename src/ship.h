#include "ode/ode.h"
#include "ofApp.h"
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

#ifndef SHIP_H // include/ifdef guard to avoid double inclusion problems
#define SHIP_H

/**
 * @brief This class represents a ship in the game.
 */
class Ship : public MyObject
{
public:
    Ship();
    explicit Ship(float _x, float _y, float _z, float _length, float _width, float _height, ofQuaternion _rotation, dWorldID _world, dSpaceID _space);

    ofLight shipLight;

    // we only want to draw the ship's hitbox when debugging
    bool drawObject = false;

    /**
     * @brief The draw() method is overridden since it's used to draw solid objects
     * in the parent class while here we only want to use it for debug purposes.
     */
    void draw() override;
};

#endif // SHIP_H
