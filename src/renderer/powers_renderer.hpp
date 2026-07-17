#pragma once

#include "gameplay_renderer.hpp"

namespace gameplay_renderer {
	bool handlePowerSelectionKey(GameLoopContext& ctx, sf::Keyboard::Key key);
	void openPowerSelection(GameLoopContext& ctx);
	void drawPowerSelection(GameLoopContext& ctx);
}
