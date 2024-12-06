#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Util.h"
#include "Entity.h"
#include "Map.h"

/**
    Notice that the game's state is now part of the Scene class, not the main file.
*/
struct GameState
{
    // ————— GAME OBJECTS ————— //
    Map *map;
    Entity *player;
    Entity *enemies;
    Entity *clues;
    Entity *diedScreen;
    Entity *extraImages;
    // ————— AUDIO ————— //
    Mix_Music *bgm;
    Mix_Chunk *jump_sfx;
    Mix_Chunk *monster_walk;
    Mix_Chunk *monster_colloid;
    Mix_Chunk *player_killed;
    Mix_Chunk *player_walk;
    Mix_Chunk *enemy_laugh;
    
    // ————— POINTERS TO OTHER SCENES ————— //
    int next_scene_id;
};


class Scene {
protected:
    GameState m_game_state;
    int playerLives = 3;
    bool ifCollectedFirstClue = false;
    bool ifCollectedSecondClue = false;
    bool ifCollectedThirdClue = false;
    bool ifCollectedActiveClue = false;
    bool ifPlayerDied = false;
public:
    // ————— ATTRIBUTES ————— //
    int m_number_of_enemies = 1;
    
    Scene() : playerLives(3), ifCollectedFirstClue(false), ifCollectedSecondClue(false), ifCollectedThirdClue(false){}

    Scene(int current_lives, bool ifFirst, bool ifSecond, bool ifThird) : playerLives(current_lives), ifCollectedFirstClue(ifFirst), ifCollectedSecondClue(ifSecond), ifCollectedThirdClue(ifThird){}
    
    // ————— METHODS ————— //
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program) = 0;
    // Setter to start the game //
    virtual void launch(){}
    
    // ————— GETTERS ————— //
    GameState const get_state() const { return m_game_state;             }
    int const get_number_of_enemies() const { return m_number_of_enemies; }
    int const get_scene_id() const {return m_game_state.next_scene_id; }
    int const get_current_lives() const {return playerLives; }
    bool const get_if_first_clue_collected() const { return ifCollectedFirstClue; }
    bool const get_if_second_clue_collected() const { return ifCollectedSecondClue; }
    bool const get_if_third_clue_collected() const { return ifCollectedThirdClue; }
    bool const get_if_player_died() const { return ifPlayerDied; }
    bool const get_if_active_clue() const { return ifCollectedActiveClue; }
    
    void set_current_lives(int new_lives) {playerLives = new_lives; }
    void collect_first_clue() { ifCollectedFirstClue = true; }
    void collect_second_clue() { ifCollectedSecondClue = true; }
    void collect_third_clue() { ifCollectedThirdClue = true; }
    void set_first_clue_status(bool status) { ifCollectedFirstClue = status; }
    void set_second_clue_status(bool status) { ifCollectedSecondClue = status; }
    void set_third_clue_status(bool status) { ifCollectedThirdClue = status; }
    void player_died() { ifPlayerDied = true; }
    void collect_active() { ifCollectedActiveClue = true; }
    
};
