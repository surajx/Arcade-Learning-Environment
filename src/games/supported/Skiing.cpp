/* *****************************************************************************
 * A.L.E (Arcade Learning Environment)
 * Copyright (c) 2009-2013 by Yavar Naddaf, Joel Veness, Marc G. Bellemare and 
 *   the Reinforcement Learning and Artificial Intelligence Laboratory
 * Released under the GNU General Public License; see License.txt for details. 
 *
 * Based on: Stella  --  "An Atari 2600 VCS Emulator"
 * Copyright (c) 1995-2007 by Bradford W. Mott and the Stella team
 *
 * *****************************************************************************
 */
#include "Skiing.hpp"

#include "../RomUtils.hpp"


SkiingSettings::SkiingSettings() {
    m_reward   = 0;
    m_score    = 0;
    m_terminal = false;
    m_mode = 1;
}


/* create a new instance of the rom */
RomSettings* SkiingSettings::clone() const { 
    
    RomSettings* rval = new SkiingSettings();
    *rval = *this;
    return rval;
}


/* process the latest information from ALE */
void SkiingSettings::step(const System& system) {

    // update the reward
    int centiseconds = getDecimalScore(0xEA, 0xE9, &system);
    int minutes = readRam(&system, 0xE8);
    int score = minutes * 6000 + centiseconds;
    int reward = m_score - score; // negative reward for time
    m_reward = reward;
    m_score = score;

    // update terminal status
    int end_flag = readRam(&system, 0x91);
    m_terminal = end_flag == 0xFF;
}


/* is end of game */
bool SkiingSettings::isTerminal() const {

    return m_terminal;
};


/* get the most recently observed reward */
reward_t SkiingSettings::getReward() const { 

    return m_reward; 
}


/* is an action part of the minimal set? */
bool SkiingSettings::isMinimal(const Action &a) const {

    switch (a) {
        case PLAYER_A_NOOP:
        case PLAYER_A_RIGHT:
        case PLAYER_A_LEFT:
            return true;
        default:
            return false;
    }   
}

bool SkiingSettings::isLegal(const Action &a) const {
  switch (a) {
    // Don't allow pressing 'fire'
    case PLAYER_A_FIRE:
    case PLAYER_A_UPFIRE:
    case PLAYER_A_DOWNFIRE:
    case PLAYER_A_LEFTFIRE:
    case PLAYER_A_RIGHTFIRE:
    case PLAYER_A_UPLEFTFIRE:
    case PLAYER_A_UPRIGHTFIRE:
    case PLAYER_A_DOWNLEFTFIRE:
    case PLAYER_A_DOWNRIGHTFIRE:
      return false;
    default:
      return true;
  }
}

/* reset the state of the game */
void SkiingSettings::reset(System& system, StellaEnvironment& environment) {
    
    m_reward   = 0;
    m_score    = 0;
    m_terminal = false;
    setMode(m_mode,system,environment);
}
        
/* saves the state of the rom settings */
void SkiingSettings::saveState(Serializer & ser) {
  ser.putInt(m_reward);
  ser.putInt(m_score);
  ser.putBool(m_terminal);
}

// loads the state of the rom settings
void SkiingSettings::loadState(Deserializer & ser) {
  m_reward = ser.getInt();
  m_score = ser.getInt();
  m_terminal = ser.getBool();
}

ActionVect SkiingSettings::getStartingActions() {
    ActionVect startingActions;
    for (int i=0; i<16; i++)
        startingActions.push_back(PLAYER_A_DOWN);
    return startingActions;
}




//Returns a list of mode that the game can be played in.
ModeVect SkiingSettings::getAvailableModes(){
    ModeVect modes(10);
    for(unsigned i=0;i<10;i++){
        modes[i]=i+1;
    }
    return modes;
}

//Set the mode of the game. The given mode must be one returned by the previous function. 
void SkiingSettings::setMode(mode_t m,System &system, StellaEnvironment& environment){
    if(m>=1 && m<=10){
        m_mode = m;
        //open the mode selection screen
        environment.pressSelect(2);
        //Read the mode we are currently in
        unsigned char mode = readRam(&system,0xEB);
        //press select until the correct mode is reached
        while(mode!=m_mode){
            environment.pressSelect(2);
            mode = readRam(&system,0xEB);
        }
        //reset the environment to apply changes.
        environment.soft_reset();
    }else{
        throw std::runtime_error("This mode doesn't currently exist for this game");
    }

}
