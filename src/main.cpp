#include "ofMain.h"
#include "ofApp.h"
#include "ode/ode.h"

ofApp *myApp;

//========================================================================
int main( ){
    myApp = new ofApp();
    ofSetupOpenGL(3000,1700,OF_WINDOW);
    ofRunApp(myApp);
}
