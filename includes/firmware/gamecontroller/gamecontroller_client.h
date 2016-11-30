#ifndef _GAMECONTROLLER_CLIENT_H_
#define _GAMECONTROLLER_CLIENT_H_

#include <stdint.h>
#include <string>
#include <RoboCupGameControlData.h>

namespace Gamecontroller {
const std::string GC_INFO_SOCK_NAME = "gcInfoSocket";

enum GameState {
    INITIAL  = (STATE_INITIAL),
    READY    = (STATE_READY),
    SET      = (STATE_SET),
    PLAY     = (STATE_PLAYING),
    FINISHED = (STATE_FINISHED)
};

};

#endif /* _GAMECONTROLLER_API_H_ */

