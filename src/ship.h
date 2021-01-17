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
    explicit Ship(float _x, float _y, float _z,
                  float _length, float _width, float _height, ofQuaternion _rotation,
                  dWorldID _world, dSpaceID _space);
    const float* currentVelocity;
    const float* currentAngularVelocity;
    const float* currentRotation;
    /**
     * @brief Are we trying to get lift?
     */
    bool lift;
    /**
     * @brief amount of lift the ship has.
     */
    float liftAmount;
    /**
     * @brief maximum amount of lift the ship can have.
     */
    float maxLiftAmount;
    /**
     * @brief The actual speed and rotation speed of the ship.
     */
    dReal speed,steer;
    /**
     * @brief Maximum amount of speed.
     */
    dReal maxSpeed;
    /**
     * @brief Maximum amount of steering speed.
     */
    dReal maxSteer;
    /**
     * @brief engine sound player.
     */
    ofSoundPlayer shipEngineSound;
    ofSoundPlayer emptyEngineSound; // empty lift
    ofSoundPlayer rechargeSound; // lift recharge
    // light?
    ofLight shipLight;
    // we only want to draw the ship's hitbox when debugging
    bool drawObject = true;

    void alignToZAxis();

    void updateMovement();
    /**
     * @brief The draw() method is overridden since it's used to draw solid objects
     * in the parent class while here we only want to use it for debug purposes.
     */
    void draw() override;
};

#endif // SHIP_H
