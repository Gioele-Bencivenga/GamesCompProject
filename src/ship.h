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
    Ship(float _x, float _y, float _z, dWorldID _world, dSpaceID _space);

    /// LENGTH, WIDTH, HEIGHT
    const float objLength = 1.2, objWidth = 1.5, objHeight = 0.2; // something's fucky with these dimensions, look out

    // we only want to draw the ship's hitbox when debugging
    bool drawObject = true;

    /**
     * @brief The draw() method is overridden since it's used to draw solid objects
     * in the parent class while here we only want to use it for debug purposes.
     */
    void draw() override;
};

#endif // SHIP_H
