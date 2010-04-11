#include "Common.h"

Player::Player() : Position(0,0), Velocity(0,0) {

}

PlayerState* Player::findState(float time) {
	for(vector<PlayerState>::reverse_iterator itor = StateList.rbegin(); itor != StateList.rend(); ++itor) {
		if(itor->Time < time)
			return &*itor;
	}
	return NULL;
}


void Player::update(float dt) {

}

void Player::draw(bool drawStateList, bool drawFromState, float relativeTime) {
	PlayerState* state = NULL;

	if(drawStateList) {
		D3DXVECTOR2 lastPosition = Position;
		for(PlayerStateList::iterator itor = StateList.begin(); itor != StateList.end(); ++itor) {
			g_Renderer()->drawLine(lastPosition.x * BLOCK_SIZE+ BLOCK_SIZE/2, lastPosition.y * BLOCK_SIZE+ BLOCK_SIZE/2, 
				itor->Position.x * BLOCK_SIZE + BLOCK_SIZE/2, itor->Position.y * BLOCK_SIZE+ BLOCK_SIZE/2, 1, D3DCOLOR_XRGB(120, 120, 120));
			lastPosition = itor->Position;
		}

		if(StateList.size()) {
			g_Renderer()->drawRect(lastPosition.x * BLOCK_SIZE+ BLOCK_SIZE/2, lastPosition.y * BLOCK_SIZE+ BLOCK_SIZE/2, 2, 2, D3DCOLOR_XRGB(190, 80, 80));
		}
	}

	if(drawFromState && (state = findState(relativeTime)))	{
		g_Renderer()->drawRect(state->Position.x*BLOCK_SIZE, state->Position.y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
	}
	else {
		g_Renderer()->drawRect(Position.x*BLOCK_SIZE, Position.y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
	}
}
