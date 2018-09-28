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
        params.AntiAlias = 16;
        params.DriverType = video::EDT_OPENGL;
        params.WindowSize = core::dimension2d<u32>(1920*0.75, 1080*0.75);
        params.Fullscreen = false;
        params.Vsync = false;
        params.Doublebuffer = true;
        params.WindowId = nullptr;
        params.DriverMultithreaded = true;
        params.Stencilbuffer = false;
        params.HandleSRGB = true;
        params.WithAlphaChannel = true;
        return params;
    }();
    
    IrrlichtDevice *device = irr::createDeviceEx(params);
    
	irr::core::stringw str = L"Procedural Terrain Generation";
	device->setWindowCaption(str.c_str());
	
    WorldScene ws{device};
    
    u32 then = device->getTimer()->getTime();
    while(device->run() && device) {
        tom::threading::manageMainthreadCallbacks();
        
        const u32 now = device->getTimer()->getTime();
        const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
        then = now;
        
        ws.update(frameDeltaTime);
        ws.render();
    }
    
	device->drop();
	
	return 0;
}

