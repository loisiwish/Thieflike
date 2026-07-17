#include "powers_renderer.hpp"

#include "../items/powers.hpp"

#include <algorithm>
#include <random>

namespace gameplay_renderer {
	namespace {
		std::vector<PowerChoiceOption> buildAvailablePowerChoices(const Player& player) {
			std::vector<PowerChoiceOption> allChoices;
			allChoices.emplace_back(Power_Lifesteal().getName(), Power_Lifesteal().getDescription(), player.getPowerLevelByName(Power_Lifesteal().getName()), Power_Lifesteal().getMaxLevel());
			allChoices.emplace_back(Power_Telekinesis().getName(), Power_Telekinesis().getDescription(), player.getPowerLevelByName(Power_Telekinesis().getName()), Power_Telekinesis().getMaxLevel());
			allChoices.emplace_back(Power_Thorns().getName(), Power_Thorns().getDescription(), player.getPowerLevelByName(Power_Thorns().getName()), Power_Thorns().getMaxLevel());
			allChoices.emplace_back(Power_PoisonnedWeapon().getName(), Power_PoisonnedWeapon().getDescription(), player.getPowerLevelByName(Power_PoisonnedWeapon().getName()), Power_PoisonnedWeapon().getMaxLevel());
			allChoices.emplace_back(Power_Sniper().getName(), Power_Sniper().getDescription(), player.getPowerLevelByName(Power_Sniper().getName()), Power_Sniper().getMaxLevel());

			std::vector<PowerChoiceOption> available;
			for (const PowerChoiceOption& option : allChoices) {
				if (option.level < option.maxLevel) {
					available.push_back(option);
				}
			}
			return available;
		}
	}

	void openPowerSelection(GameLoopContext& ctx) {
		if (ctx.stage == nullptr) {
			return;
		}

		std::vector<PowerChoiceOption> available = buildAvailablePowerChoices(ctx.stage->getPlayer());
		if (available.empty()) {
			ctx.powerSelectionOpen = false;
			ctx.pendingPowerChoices = 0;
			ctx.powerChoices.clear();
			return;
		}

		static thread_local std::mt19937 rng(std::random_device{}());
		std::shuffle(available.begin(), available.end(), rng);

		const std::size_t choiceCount = std::min<std::size_t>(3, available.size());
		ctx.powerChoices.assign(available.begin(), available.begin() + static_cast<std::vector<PowerChoiceOption>::difference_type>(choiceCount));
		ctx.powerSelectionIndex = 0;
		ctx.powerSelectionOpen = true;
		ctx.inventoryScreenOpen = false;
	}

	bool handlePowerSelectionKey(GameLoopContext& ctx, sf::Keyboard::Key key) {
		if (!ctx.powerSelectionOpen) {
			return false;
		}

		if (ctx.powerChoices.empty()) {
			ctx.powerSelectionOpen = false;
			ctx.pendingPowerChoices = 0;
			return true;
		}

		if (key == sf::Keyboard::Left || key == sf::Keyboard::Up) {
			--ctx.powerSelectionIndex;
			if (ctx.powerSelectionIndex < 0) {
				ctx.powerSelectionIndex = static_cast<int>(ctx.powerChoices.size()) - 1;
			}
			return true;
		}

		if (key == sf::Keyboard::Right || key == sf::Keyboard::Down) {
			++ctx.powerSelectionIndex;
			if (ctx.powerSelectionIndex >= static_cast<int>(ctx.powerChoices.size())) {
				ctx.powerSelectionIndex = 0;
			}
			return true;
		}

		int selectedChoice = -1;
		if (key == sf::Keyboard::Num1 || key == sf::Keyboard::Numpad1) {
			selectedChoice = 0;
		} else if (key == sf::Keyboard::Num2 || key == sf::Keyboard::Numpad2) {
			selectedChoice = 1;
		} else if (key == sf::Keyboard::Num3 || key == sf::Keyboard::Numpad3) {
			selectedChoice = 2;
		} else if (key == sf::Keyboard::Enter || key == sf::Keyboard::Space) {
			selectedChoice = ctx.powerSelectionIndex;
		}

		if (selectedChoice < 0 || selectedChoice >= static_cast<int>(ctx.powerChoices.size())) {
			return true;
		}

		Player& player = ctx.stage->getPlayer();
		const PowerChoiceOption& choice = ctx.powerChoices[static_cast<std::size_t>(selectedChoice)];
		if (player.applyPowerByName(choice.name)) {
			ctx.popupNotifications.emplace_back(
				"Power Up: " + choice.name + " (" +
					std::to_string(player.getPowerLevelByName(choice.name)) + "/" +
					std::to_string(choice.maxLevel) + ")",
				sf::Color(120, 220, 255),
				2.8f);
			if (ctx.popupNotifications.size() > 4) {
				ctx.popupNotifications.erase(ctx.popupNotifications.begin());
			}
		}

		if (ctx.pendingPowerChoices > 0) {
			--ctx.pendingPowerChoices;
		}
		if (ctx.pendingPowerChoices > 0) {
			openPowerSelection(ctx);
		} else {
			ctx.powerSelectionOpen = false;
			ctx.powerChoices.clear();
		}

		return true;
	}

	void drawPowerSelection(GameLoopContext& ctx) {
		if (!ctx.powerSelectionOpen || ctx.window == nullptr || !ctx.uiFontLoaded) {
			return;
		}

		const sf::Vector2u windowSize = ctx.window->getSize();
		sf::RectangleShape overlay;
		overlay.setSize(sf::Vector2f(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)));
		overlay.setFillColor(sf::Color(8, 12, 20, 220));
		ctx.window->draw(overlay);

		sf::Text title("Choose A Power", ctx.uiFont, 44);
		title.setFillColor(sf::Color(240, 240, 255));
		title.setPosition(60.f, 30.f);
		ctx.window->draw(title);

		const float cardWidth = (static_cast<float>(windowSize.x) - 120.f - 40.f) / 3.f;
		const float cardHeight = std::max(220.f, static_cast<float>(windowSize.y) - 180.f);
		const float startX = 60.f;
		const float cardY = 130.f;

		for (std::size_t i = 0; i < ctx.powerChoices.size(); ++i) {
			const PowerChoiceOption& choice = ctx.powerChoices[i];
			const float cardX = startX + static_cast<float>(i) * (cardWidth + 20.f);
			const bool selected = static_cast<int>(i) == ctx.powerSelectionIndex;

			sf::RectangleShape card(sf::Vector2f(cardWidth, cardHeight));
			card.setPosition(cardX, cardY);
			card.setFillColor(selected ? sf::Color(28, 50, 84, 245) : sf::Color(18, 24, 36, 235));
			card.setOutlineThickness(2.f);
			card.setOutlineColor(selected ? sf::Color(120, 220, 255) : sf::Color(70, 90, 120));
			ctx.window->draw(card);

			sf::Text indexText(std::to_string(i + 1), ctx.uiFont, 20);
			indexText.setFillColor(sf::Color(180, 200, 220));
			indexText.setPosition(cardX + 12.f, cardY + 10.f);
			ctx.window->draw(indexText);

			sf::Text nameText(choice.name, ctx.uiFont, 28);
			nameText.setFillColor(sf::Color::White);
			nameText.setPosition(cardX + 12.f, cardY + 38.f);
			ctx.window->draw(nameText);

			sf::Text levelText("Level " + std::to_string(choice.level) + " / " + std::to_string(choice.maxLevel), ctx.uiFont, 18);
			levelText.setFillColor(sf::Color(255, 215, 140));
			levelText.setPosition(cardX + 12.f, cardY + 78.f);
			ctx.window->draw(levelText);

			sf::Text description(wrapTextToWidth(choice.description, ctx.uiFont, 18, cardWidth - 24.f), ctx.uiFont, 18);
			description.setFillColor(sf::Color(210, 220, 240));
			description.setPosition(cardX + 12.f, cardY + 110.f);
			ctx.window->draw(description);
		}
	}
}
