#pragma once
#include "statemachine.h"
#include "display.h"
#include "tetris.h"
#include "snake.h"
#include "jump.h"
#include "event.h"


class Display;
class GameSM : public StateMachine<Event*>
{
public:
	GameSM(Display * display);
	void processStateMaschine(Event *event);
private:
	// states
	void stateDefault(Event* event);
	void stateTetris(Event* event);
	void stateSnake(Event* event);
	void stateJump(Event* event);
	void stateSettingsMenu(Event* event);
	void stateGameOver(Event* event);
	void stateSpeedMenu(Event* event);
	void stateLanguageMenu(Event* event);
	void stateLoadEffect(Event* event);
	void stateHighscoreMenu(Event* event);
	void stateResetMenu(Event*event);

	class MenuItem{
	public:
		void init(char num, char initial){
			num_ = num;
			value_ = initial;
		}
		void init(char num){
			num_ = num;
		}

		enum Button{
			NO_BTN = 0,
			UP_BTN = 1,
			DOWN_BTN = 2,
		};

		Button advance(Event* event);
		static Button advance(Event *event, char &item, const char num, const char min = 0);
		char value_ = 0;

	private:
		char num_ = 0;
	};

	enum ProcessCriterum{
		EVER = 1 << 0,
		TIMER1 = 1 << 1,
		TIMER2 = 1 << 2,
		PCINT = 1 << 3
	};

	enum Language{
		EN = 0,
		DE = 1,
	};

	byte process_criterium_;
	unsigned long process_timer1_;
	unsigned long process_timer2_;

	Display * display_;
	Game * game_; // pointer to the current game

	Language language_ = EN;
	byte speed_ = 0;
	State load_following_state_; // state load effect need a pointer to the state after its
};
