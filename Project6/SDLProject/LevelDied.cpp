#include "LevelDied.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

int diedCounter_a = 20;

constexpr char SPRITESHEET_FILEPATH[] = "assets/Full Sheet Boy.png",
           ENEMY_FILEPATH[]       = "assets/djin.png",
           CLUE_BOARD_1_FILEPATH[] = "assets/main_tiles.png",
           DIED_SCREEN_FILEPATH[] = "assets/gameLose.png",
           FONT_FILEPATH[] = "assets/font1.png";

            

unsigned int LEVELDIED_DATA[] =
{
    180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180,
    180, 180, 35, 35, 35, 35, 35, 35, 35, 180, 180, 180,
    180, 180, 180, 20, 48, 48, 48, 48, 48, 48, 48, 18, 180, 180,
    180, 180, 180, 20, 48, 48, 48, 48, 48, 48, 48, 18, 180, 180,
    180, 180, 180, 20, 48, 49, 48, 48, 48, 48, 48, 18, 180, 180,
    180, 180, 180, 20, 48, 48, 48, 48, 48, 48, 48, 18, 180, 180,
    180, 180, 180, 20, 48, 48, 48, 48, 48, 48, 48, 18, 180, 180,
    180, 180, 180, 180, 3, 3, 3, 3, 3, 3, 3, 180, 180, 180,
};

LevelDied::LevelDied() {}

LevelDied::LevelDied(int currentPlayerLives, bool ifCollectFirstClue, bool ifCollectSecondClue, bool ifCollectThirdClue) : Scene(currentPlayerLives, ifCollectFirstClue, ifCollectSecondClue, ifCollectThirdClue) {}

LevelDied::~LevelDied()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelDied::initialise()
{
    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("assets/interior.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELDIED_DATA, map_texture_id, 1.0f, 16, 16);
    GLuint died_screen_id = Utility::load_texture(DIED_SCREEN_FILEPATH);
    m_game_state.diedScreen = new Entity(died_screen_id, 1.0f, 1.0f, 1.0f, DIEDSCREEN);
    m_game_state.diedScreen->set_scale(glm::vec3(5.0f, 5.0f, 0.0f));
    m_game_state.diedScreen->set_position(glm::vec3(5.0f, -4.0f, 0.0f));
    
    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
    // Existing
    int player_walking_animation[5][4] =
    {
        { 0, 1, 2, 3 },  // Nothing
        { 4, 5, 6, 6 }, // Move right
        { 8, 9, 10, 11 },  //Move Left
        { 12, 13, 14, 15 },   // Move Down
        { 16, 17, 18, 19 } // Move Up
    };

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    
    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        2.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        3,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        5,                         // animation row amount
        0.40f,                      // width
        0.5f,                       // height
        PLAYER
    );
    
    m_game_state.player->set_position(glm::vec3(5.0f, -5.5f, 0.0f));
    
    // m_game_state.player->set_position(glm::vec3(5.0f, -3.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(6.0f);
    
    /**
    Enemies' stuff */
    int ai_walking_animation[3][4] =
    {
        { 0, 1, 2, 3}, // Use this as animation
        { 4, 5, 6, 7},
        { 8, 9, 10, 11},
    };
    
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
    m_game_state.enemies[i] =  Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, BALMER, KILLER, ai_walking_animation);
    }


    m_game_state.enemies[0].set_position(glm::vec3(8.0f, -5.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.enemies[0].set_width(1.5f);
    m_game_state.enemies[0].set_height(1.5f);
    m_game_state.enemies[0].set_speed(1.5f);
    
    
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/Jump Sound Effect (High Quality).wav");
    m_game_state.player_walk = Mix_LoadWAV("assets/stepwood_2.wav");
    m_game_state.enemy_laugh = Mix_LoadWAV("assets/Creepy Laugh Sound Effect  SOUNDFX.wav");
    m_game_state.player_killed = Mix_LoadWAV("assets/10._damage_grunt_male.wav");
    Mix_VolumeChunk(m_game_state.player_walk, MIX_MAX_VOLUME / 10);
}

void LevelDied::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map, 2);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map, 2);
        if(m_game_state.enemies[i].get_ai_state() == KILLER && !ifPlayerDied){
            Mix_PlayChannel(-1,  m_game_state.enemy_laugh, 0);
        }
    }
    
    m_game_state.diedScreen->update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map, 1);
    
    if(m_game_state.player->get_collided_enemy()){
        player_died();
    }
    
    
    std::cout << "start" << std::endl;
    std::cout << m_game_state.player->get_position().x << std::endl;
    std::cout << m_game_state.player->get_position().y << std::endl;
    std::cout << "stop" << std::endl;
}

void LevelDied::render(ShaderProgram *program)
{
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    for (int i = 0; i < m_number_of_enemies; i++)
            m_game_state.enemies[i].render(program);
    
    GLuint g_font_texture_id = Utility::load_texture(FONT_FILEPATH);
    Utility::draw_text(program, g_font_texture_id, "You are not supposed to get here", 0.25f, 0.005f, glm::vec3(0.65f, -0.8f, 0.0f));
    
    if(ifPlayerDied){
        m_game_state.diedScreen->render(program);
        if(diedCounter_a > 0){
            Mix_PlayChannel(-1,  m_game_state.player_killed, 0);
        }
        diedCounter_a -=1 ;
    }
    
}

