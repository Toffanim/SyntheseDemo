#if !defined(CONTROLLER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */

#define CONTROLLER_H

#include <vector>
#include <functional>
#include <map>

class Controller
{
public:
    Controller();
    #if 0
    void processKey( int key, int action );
    void processCursor( double xpos, double ypos );
    void processMouse( int button, int action );

    void setKeyCallback(SDL_Keycode code, const std::function<void(void)> &func);
    void setMiscCallback(unsigned char code, const std::function<void(void)> &func);
    void setMouseCallback(std::function<void(int, int)> &func);
private:
    std::map< std::pair<int,int>, std::function<void(void)> > mappingKeyRelease;
    std::map<unsigned char, std::function<void(void)>> mappingMisc;
    std::function<void(int, int)> mouseEvent;
#endif
};
#endif
