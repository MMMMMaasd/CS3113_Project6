#include "LevelA.h"
#include "Utility.h"


#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 9

constexpr char SPRITESHEET_FILEPATH[] = "assets/Full Sheet Boy.png",
           PLATFORM_FILEPATH[]    = "assets/pastoral-tileset-db32.png",
           ENEMY_FILEPATH[]       = "assets/rotting_zombie-NESW.png",
           CLUE_BOARD_1_FILEPATH[] = "assets/clue1.png",
           CLUE_BOARD_2_FILEPATH[] = "assets/clue2.png",
           CLUE_BOARD_3_FILEPATH[] = "assets/clue3.png",
           CLUE_BOARD_4_FILEPATH[] = "assets/clue4.png",
           CLUE_BOARD_5_FILEPATH[] = "assets/clue5.png",
           DIED_SCREEN_FILEPATH[] = "assets/gameLose.png",
           FONT_FILEPATH[] = "assets/font1.png";

int diedCounter = 20;

unsigned int LEVELA_DATA[] =
{
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    3, 3, 4, 18, 18, 19, 18, 18, 18, 21, 26, 26, 26, 26,
    3, 3, 18, 4, 21, 26, 23, 36, 38, 20, 24, 18, 3, 3,
    38, 36, 37, 38, 20, 19, 20, 36, 38, 20, 26, 23, 24, 3,
    38, 36, 37, 38, 20, 18, 20, 31, 32, 20, 19, 20, 18, 3,
    3, 3, 18, 5, 20, 24, 20, 5, 18, 20, 26, 35, 4, 3,
    3, 3, 3, 3, 20, 4, 33, 26, 26, 35, 18, 24, 4, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
};

LevelA::LevelA(int currentPlayerLives, bool ifCollectFirstClue, bool ifCollectSecondClue, bool ifCollectThirdClue) : Scene(currentPlayerLives, ifCollectFirstClue, ifCollectSecondClue, ifCollectThirdClue) {}
 
LevelA::~LevelA()
{
    delete [] m_game_state.enemies;
    delete [] m_game_state.clues;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeChunk(m_game_state.monster_walk);
    Mix_FreeChunk(m_game_state.monster_colloid);
    Mix_FreeChunk(m_game_state.player_killed);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelA::initialise()
{
    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("assets/pastoral-tileset-db32.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 6, 8);
     
    // Clues in this level
    GLuint clue_1_id = Utility::load_texture(CLUE_BOARD_1_FILEPATH);
    GLuint clue_2_id = Utility::load_texture(CLUE_BOARD_2_FILEPATH);
    GLuint clue_3_id = Utility::load_texture(CLUE_BOARD_3_FILEPATH);
    GLuint clue_4_id = Utility::load_texture(CLUE_BOARD_4_FILEPATH);
    GLuint clue_5_id = Utility::load_texture(CLUE_BOARD_5_FILEPATH);
    GLuint died_screen_id = Utility::load_texture(DIED_SCREEN_FILEPATH);
    m_game_state.clues = new Entity[5];
    m_game_state.diedScreen = new Entity(died_screen_id, 1.0f, 1.0f, 1.0f, DIEDSCREEN);
    m_game_state.diedScreen->set_scale(glm::vec3(5.0f, 5.0f, 0.0f));
    m_game_state.diedScreen->set_position(glm::vec3(5.0f, -4.0f, 0.0f));
    
    m_game_state.clues[0] =  Entity(clue_1_id, 1.0f, 1.0f, 1.0f, CLUE);
    m_game_state.clues[1] =  Entity(clue_2_id, 1.0f, 1.0f, 1.0f, CLUE);
    m_game_state.clues[2] =  Entity(clue_3_id, 1.0f, 1.0f, 1.0f, CLUE);
    m_game_state.clues[3] =  Entity(clue_4_id, 1.0f, 1.0f, 1.0f, CLUE);
    m_game_state.clues[4] =  Entity(clue_5_id, 1.0f, 1.0f, 1.0f, CLUE);
    
    for (int i = 0; i < 5; i++)
    {
        m_game_state.clues[i].set_scale(glm::vec3(5.0f, 5.0f, 0.0f));
        m_game_state.clues[i].set_position(glm::vec3(5.0f, -4.0f, 0.0f));
    }
 
    
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
        1.0f,                      // speed
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
         
    m_game_state.player->set_position(glm::vec3(1.9f, -7.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(5.5f);
    
    /**
    Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);
    
    int enemy_walking_animation[4][3] =
    {
        { 0, 1, 2 }, // Move Up
        { 3, 4, 5 }, // Move Right
        { 6, 7, 8 }, // Move Down
        { 9, 10, 11 }, // Move Left
    };
    

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
    m_game_state.enemies[i] =  Entity(enemy_texture_id, 0.5f, 0.5f, 1.0f, ENEMY, GUARD, IDLE, enemy_walking_animation);
    }
 

    m_game_state.enemies[0].set_position(glm::vec3(7.0f, -2.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/Jump Sound Effect (High Quality).wav");
    m_game_state.player_walk = Mix_LoadWAV("assets/stepdirt_5.wav");
    m_game_state.monster_walk = Mix_LoadWAV("assets/monster-14.wav");
    m_game_state.monster_colloid = Mix_LoadWAV("assets/monster-16.wav");
    m_game_state.player_killed = Mix_LoadWAV("assets/10._damage_grunt_male.wav");
    
    Mix_VolumeChunk(m_game_state.player_walk, MIX_MAX_VOLUME / 30);
    Mix_VolumeChunk(m_game_state.monster_colloid, MIX_MAX_VOLUME / 10);
}

void LevelA::update(float delta_time)
{
    if(ifPlayerDied){
        return;
    }
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map, 1);
    
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map, 1);
        if(m_game_state.enemies[i].get_ai_state() == CHASING && m_game_state.enemies[i].if_levela_ai_chasing && !ifPlayerDied){
            Mix_PlayChannel(-1,  m_game_state.monster_walk, 0);
        }
    }
    
    for (int i = 0; i < 5; i++)
    {
        m_game_state.clues[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map, 1);
    }
    
    m_game_state.diedScreen->update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map, 1);
    
    /*
    if(m_game_state.player->get_collided_enemy()){
        m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
        m_game_state.enemies[0].set_position(glm::vec3(10.0f, 0.0f, 0.0f));
        set_current_lives(get_current_lives() - 1);
        m_game_state.player->set_enemy_collide_status(false);
    }
     */
    
    if(m_game_state.player->get_collided_enemy()){
        player_died();
    }
    
    std::cout << "start" << std::endl;
    std::cout << m_game_state.player->get_position().x << std::endl;
    std::cout << m_game_state.player->get_position().y << std::endl;
    std::cout << "stop" << std::endl;
    if (m_game_state.player->get_position().x >= 10.3f && ifCollectedFirstClue) m_game_state.next_scene_id = 1;
}
 
void LevelA::render(ShaderProgram *g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < m_number_of_enemies; i++)
            m_game_state.enemies[i].render(g_shader_program);
     
    if(m_game_state.player->get_if_interact() && m_game_state.player->get_position().y >= -6.06f && m_game_state.player->get_position().y <= -5.85f && m_game_state.player->get_position().x >= 1.7f && m_game_state.player->get_position().x <= 1.75f){
        m_game_state.clues[0].render(g_shader_program);
        m_game_state.player->if_reading_clue = true;
    }
    
    if(m_game_state.player->get_if_interact() && m_game_state.player->get_position().y >= -3.25f && m_game_state.player->get_position().y <= -2.75f && m_game_state.player->get_position().x >= 2.65f && m_game_state.player->get_position().x <= 3.49f){
        m_game_state.clues[1].render(g_shader_program);
        m_game_state.player->if_reading_clue = true;
    }
    
    if(m_game_state.player->get_if_interact() && m_game_state.player->get_position().y >= -7.01f && m_game_state.player->get_position().y <= -6.70f && m_game_state.player->get_position().x >= 4.8f && m_game_state.player->get_position().x <= 5.25f){
        m_game_state.clues[2].render(g_shader_program);
        m_game_state.player->if_reading_clue = true;
    }
    
    if(m_game_state.player->get_if_interact() && m_game_state.player->get_position().y >= -5.30f && m_game_state.player->get_position().y <= -4.6f && m_game_state.player->get_position().x >= 7.1f && m_game_state.player->get_position().x <= 7.30f){
        m_game_state.clues[3].render(g_shader_program);
        m_game_state.player->if_reading_clue = true;
        m_game_state.player->got_clue();
        collect_first_clue();
    }
    
    if(!ifCollectedFirstClue && m_game_state.player->get_position().x >= 10.3f){
        GLuint g_font_texture_id = Utility::load_texture(FONT_FILEPATH);
        Utility::draw_text(g_shader_program, g_font_texture_id, "You must collect clue in this level", 0.2f, 0.005f, glm::vec3(0.8f, -0.8f, 0.0f));
    }
    
    if(m_game_state.player->get_if_interact() && m_game_state.player->get_position().y >= -5.18f && m_game_state.player->get_position().y <= -4.8f && m_game_state.player->get_position().x >= 8.7f && m_game_state.player->get_position().x <= 8.8f){
        m_game_state.clues[4].render(g_shader_program);
        m_game_state.player->if_reading_clue = true;
        m_game_state.player->got_clue();
        collect_first_clue();
    }
    
    
    if(ifPlayerDied){
        m_game_state.diedScreen->render(g_shader_program);
        if(diedCounter > 0){
            Mix_PlayChannel(-1,  m_game_state.player_killed, 0);
        }
        Mix_PlayChannel(-1,  m_game_state.monster_colloid, 0);
        diedCounter -=1 ;
        
    }
    
    
}


