#include "Common.h"

PlayerState* Player::findState(float time) {
	for(vector<PlayerState>::reverse_iterator itor = StateList.rbegin(); itor != StateList.rend(); ++itor) {
		if(itor->Time < time)
			return &*itor;
	}
	return NULL;
}
