#include "game_sm.h"


#undef TRANSITION
#define TRANSITION(s) {\
	setState(STATE_CAST(&GameSM::s)); \
	process_criterium_ = 0;\
	process(ON_ENTRY); \
	}

GameSM::GameSM(Display *display, byte speed)
   :StateMachine(STATE_CAST(&GameSM::stateDefault)), display_(display), speed_(speed), process_timer_process_time_ (0)
{
	process(ON_ENTRY);
}

void GameSM::processStateMaschine(byte event)
{
	if((process_criterium_ & EVER) || (process_criterium_ & ProcessCriterum::PCINT && event & CHANGE)){
		this->process(event);
	}

	else if (process_criterium_ & ProcessCriterum::TIMER && process_timer_process_time_ && process_timer_process_time_ < millis()){
		event |= TIMEOUT;
		process_timer_process_time_ = 0;
		this->process(event);
	}
	display_->text1_->setShiftSpeed(5);
	display_->text2_->setShiftSpeed(5);
}

byte GameSM::MenuItem::advance(byte event, char& item, const char num, const char min) {

	switch (event & INPUT_MASK) {
	case BTN_LEFT:
		if (--item < min)
			item = num-1;
		break;
	case BTN_RIGHT:
		if (++item >= num)
			item = min;
		break;
	case BTN_DOWN:
		return 1;
	case BTN_ROTATE:
		return 2;
	}
	return 0;
}


void GameSM::stateDefault(byte event)
{

	const char * texts[2][2] = {{"Tetris","Setting"}, {"Tetris","Einstellungen"}};
	static MenuItem item;
	if(event & ON_ENTRY){
		process_criterium_ |= PCINT;
		item.init(2,0);
		display_->loadMenuConfiguration();
	}

	else if(event & INPUT_MASK && event & CHANGE){
		byte advance_output = item.advance(event);

		if(advance_output == 1){
			switch(item.value_){
			case 0:
				TRANSITION(stateTetris);
				break;
			case 1:
				TRANSITION(stateSettingsMenu);
				break;
			}
			return;
		}
		else if(advance_output == 0){
			display_->update();
			return;
		}
	}

	display_->text1_->setText(texts[language_][item.value_]);
}

void GameSM::stateTetris(byte event)
{
	static bool btn_down_state	= false;
	static unsigned long step_interval;
	if(event & ON_ENTRY){
		display_->text1_->clear();
		display_->text2_->clear();
		if(game_ != nullptr){
			delete game_ ;
			game_ = nullptr;
		}
		game_ = new Game(display_);
		game_->reset();
		game_->begin();
		btn_down_state = false;
		process_criterium_ |= PCINT | TIMER;
		step_interval = 1000;
		process_timer_process_time_ = millis() + step_interval;
		return;
	}

	if(event & CHANGE){
		if(event & INPUT_MASK){
			if(event & BTN_ROTATE){
				game_->rotate();
			}

			if(event & BTN_LEFT){
				game_->left();
			}

			else if(event & BTN_RIGHT){
				game_->right();
			}

			if(event & BTN_DOWN){
				if(btn_down_state == false){
					process_timer_process_time_ = step_interval = 100;
					btn_down_state = true;
					goto step;
				}
			}
		}
		if(btn_down_state == true && !(event % BTN_DOWN)){
			process_timer_process_time_ = step_interval = 1000;
			btn_down_state = false;
		}
	}
	if(event & TIMEOUT){
step:
		if(game_->step()){ // game ends
			TRANSITION(stateShowResult);
			return;
		}
		if(!(event & BTN_DOWN))
			step_interval = 1000;
		else if(event & BTN_DOWN)
			step_interval = 100;
		process_timer_process_time_ =	millis() + step_interval;
	}
}

void GameSM::stateShowResult(byte event){
	if(event & ON_ENTRY){
		display_->clear();
		process_criterium_ |= PCINT;
		if(game_ != nullptr){
			int points;
			points = game_->getPoints();
			delete game_;
			game_ = nullptr;

			display_->text2_->setOffset(0);
			display_->text2_->setOperationCols(1,6);
			display_->text2_->setOperationRows(1,7);
			display_->text2_->setNumber(points);
			display_->text1_->setOperationRows(9,15);
			display_->text1_->setOperationCols(0,7);
			display_->text1_->setOffset(8);
			display_->text1_->setText("Game Over");
			display_->setRow(0,0xFF);
			display_->setRow(8,0xFF);
			display_->setColumn(0,0xFF);
			display_->setColumn(7,0xFF);
		}
	}
	if(event & CHANGE && event & INPUT_MASK){
		TRANSITION(stateDefault);
		return;
	}
}

void GameSM::stateSettingsMenu(byte event)
{
	static MenuItem item;
	const byte menu_text[2][2] = {

	};
	if(event & ON_ENTRY){
		item.init(2,0);
	}
	else if(event & ON_ENTRY && event & INPUT_MASK){
		if(item.advance(event)){

		}
	}
}

byte GameSM::MenuItem::advance(byte event)
{
	advance(event,value_,num_);
}
