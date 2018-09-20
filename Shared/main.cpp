//
// ProcMapGeneration.cpp: Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "irrlicht.h"
#include <functional>
#include "WorldScene.h"
#include "Utils.h"

using namespace irr;
using namespace irr::scene;
using namespace irr::video;

int main(int argc, char** argv) {
    auto params = []() -> const SIrrlichtCreationParameters {
        SIrrlichtCreationParameters params;
        params.AntiAlias = 2;
        params.DriverType = video::EDT_OPENGL;
        params.WindowSize = core::dimension2d<u32>(1920*0.75, 1080*0.75);
        params.Fullscreen = false;
        params.Vsync = true;
        params.Doublebuffer = true;
        params.WindowId = nullptr;
        params.DriverMultithreaded = true;
        params.Stencilbuffer = false;
        return params;
    }();
    
    IrrlichtDevice *device = irr::createDeviceEx(params);
    
	irr::core::stringw str = L"Procedural Terrain Generation";
	device->setWindowCaption(str.c_str());
	
    WorldScene ws{device};
    while(device->run() && device) {
        tom::threading::manageMainthreadCallbacks();
        ws.update(0);
        ws.render();
    }
    
	device->drop();
	
	return 0;
}

