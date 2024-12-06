#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 9

int diedCounter_b = 20;

constexpr char SPRITESHEET_FILEPATH[] = "assets/Full Sheet Boy.png",
           ENEMY_FILEPATH[]       = "assets/redshrikemod1.png",
           CLUE_BOARD_1_FILEPATH[] = "assets/levelb_clue1.png",
           CLUE_BOARD_2_FILEPATH[] = "assets/levelb_clue2.png",
           CLUE_BOARD_3_FILEPATH[] = "assets/levelb_clue3.png",
           CLUE_BOARD_4_FILEPATH[] = "assets/levelb_clue4.png",
           CONVERSATION_FILEPATH[] = "assets/levelb_conversation.png",
           DIED_SCREEN_FILEPATH[] = "assets/gameLose.png",
           FONT_FILEPATH[] = "assets/font1.png";

unsigned int LEVELB_DATA[] =
{
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    26, 26, 26, 26, 26, 26, 23, 4, 4, 21, 26, 26, 26, 26,
    3, 3, 30, 24, 20, 24, 20, 39, 40, 20, 24, 24, 24, 24,
    3, 3, 30, 24, 20, 24, 20, 45, 46, 20, 24, 24, 24, 24,
    3, 3, 30, 0, 1, 2, 33, 26, 26, 26, 26, 25, 24, 24,
    3, 3, 30, 6, 7, 8, 30, 5, 30, 30, 24, 24, 24, 24,
    3, 3, 30, 12, 13, 14, 30, 30, 30, 30, 24, 24, 24, 24,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
};

LevelB::LevelB() {}

LevelB::LevelB(int currentPlayerLives, bool ifCollectFirstClue, bool ifCollectSecondClue, bool ifCollectThirdClue) : Scene(currentPlayerLives, ifCollectFirstClue, ifCollectSecondClue, ifCollectThirdClue) {}

LevelB::~LevelB()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelB::initialise()
{
    m_game_state.next_scene_id = -1;
    
    GLuint map_texture_id = Utility::load_texture("assets/pastoral-tileset-db32.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id, 1.0f, 6, 8);
    
    // Clues in this level
    GLuint clue_1_id = Utility::load_texture(CLUE_BOARD_1_FILEPATH);
    GLuint clue_2_id = Utility::load_texture(CLUE_BOARD_2_FILEPATH);
    GLuint clue_3_id = Utility::load_texture(CLUE_BOARD_3_FILEPATH);
    GLuint clue_4_id = Utility::load_texture(CLUE_BOARD_4_FILEPATH);
    GLuint conversation_id = Utility::load_texture(CONVERSATION_FILEPATH);
    GLuint died_screen_id = Utility::load_texture(DIED_SCREEN_FILEPATH);
    
    m_game_state.clues = new Entity[5];
    
    m_game_state.clues[0] =  Entity(clue_2_id, 1.0f, 1.0f, 1.0f, CLUE);
    m_game_state.clues[1] =  Entity(clue_1_id, 1.0f, 1.0f, 1.0f, CLUE);
    m_game_state.clues[2] =  Entity(clue_4_id, 1.0f, 1.0f, 1.0f, CLUE);
    // Conversation with AI
    m_game_state.clues[3] =  Entity(conversation_id, 1.0f, 1.0f, 1.0f, CLUE);
    m_game_state.clues[4] =  Entity(clue_3_id, 1.0f, 1.0f, 1.0f, CLUE);
    m_game_state.diedScreen = new Entity(died_screen_id, 1.0f, 1.0f, 1.0f, DIEDSCREEN);
    m_game_state.diedScreen->set_scale(glm::vec3(5.0f, 5.0f, 0.0f));
    m_game_state.diedScreen->set_position(glm::vec3(5.0f, -4.0f, 0.0f));
    
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
        
    m_game_state.player->set_position(glm::vec3(1.0f, -2.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(5.0f);
    
    /**
    Enemies' stuff */
    
    int ai_walking_animation[3][3] =
    {
        { 0, 1, 2},  //move right
        { 3, 4, 5}, // move down
        { 6, 7, 8}, // move up
    };
    
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
    m_game_state.enemies[i] =  Entity(enemy_texture_id, 0.5f, 0.5f, 1.0f, ENEMY, WARTAUR, IDLE, ai_walking_animation, true);
    }


    m_game_state.enemies[0].set_position(glm::vec3(4.0f, -5.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_game_state.enemies[0].set_width(0.5f);
    m_game_state.enemies[0].set_height(0.5f);
    
    
    
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/Jump Sound Effect (High Quality).wav");
    m_game_state.player_walk = Mix_LoadWAV("assets/stepdirt_5.wav");
    m_game_state.player_killed = Mix_LoadWAV("assets/10._damage_grunt_male.wav");
    Mix_VolumeChunk(m_game_state.player_walk, MIX_MAX_VOLUME / 30);
}

void LevelB::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map, 1);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map, 1);
    }
    
    /*
    if(m_game_state.player->get_collided_enemy()){
        m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
        m_game_state.enemies[0].set_position(glm::vec3(15.0f, 0.0f, 0.0f));
        set_current_lives(get_current_lives() - 1);
        m_game_state.player->set_enemy_collide_status(false);
    }
    */
    
    for (int i = 0; i < 5; i++)
    {
        m_game_state.clues[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map, 1);
    }
    
    std::cout << "start" << std::endl;
    std::cout << m_game_state.player->get_position().x << std::endl;
    std::cout << m_game_state.player->get_position().y << std::endl;
    std::cout << "yes" << std::endl;
    
    if (ifCollectedSecondClue && m_game_state.player->get_position().y >= -5.25f && m_game_state.player->get_position().y <= -4.5f && m_game_state.player->get_position().x >= 8.2 && m_game_state.player->get_position().x < 8.5) m_game_state.next_scene_id = 2;
    
    if (ifCollectedSecondClue && m_game_state.player->get_position().x >= 10.3f) m_game_state.next_scene_id = 4;
    
    m_game_state.diedScreen->update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map, 1);
    
    if(m_game_state.player->get_collided_enemy() && m_game_state.enemies[0].if_attack_in_second_level){
        player_died();
    }
}
 
void LevelB::render(ShaderProgram *program)
{
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    
    if (!(m_game_state.enemies[0].get_position().y >= -5.0f && m_game_state.enemies[0].get_position().y <= -4.5f && m_game_state.enemies[0].get_position().x >= 8.2 && m_game_state.enemies[0].get_position().x < 8.5)){
        for (int i = 0; i < m_number_of_enemies; i++)
                m_game_state.enemies[i].render(program);
    }else{
        for (int i = 0; i < m_number_of_enemies; i++)
            m_game_state.enemies[i].deactivate();
    }
    


    
    if(m_game_state.player->get_if_interact() && m_game_state.player->get_position().y >= -4.25 && m_game_state.player->get_position().y <= -3.99f && m_game_state.player->get_position().x >= 1.7f && m_game_state.player->get_position().x <= 2.3f){
        m_game_state.clues[0].render(program);
        m_game_state.player->if_reading_clue = true;
        collect_second_clue();
        
    }
    
    if(m_game_state.player->get_if_interact() && m_game_state.player->get_position().y >= -3.66f && m_game_state.player->get_position().y <= -3.0f && m_game_state.player->get_position().x >= 4.3f && m_game_state.player->get_position().x <= 4.31f){
        m_game_state.clues[1].render(program);
        m_game_state.player->if_reading_clue = true;
    }
    
    if(m_game_state.player->get_if_interact() && m_game_state.player->get_position().y >= -5.25f && m_game_state.player->get_position().y <= -5.00f && m_game_state.player->get_position().x >= 4.8f && m_game_state.player->get_position().x <= 5.22f){
        m_game_state.clues[2].render(program);
        m_game_state.player->if_reading_clue = true;
    }
    
    if(m_game_state.player->if_start_conversation && !m_game_state.player->if_finish_conversation){
        m_game_state.clues[3].render(program);
    }
    
    if(m_game_state.player->get_if_interact() && m_game_state.player->get_position().y >= -5.25f && m_game_state.player->get_position().y <= -4.75f && m_game_state.player->get_position().x >= 7.5f && m_game_state.player->get_position().x <= 8.1f){
        m_game_state.clues[4].render(program);
        m_game_state.player->if_reading_clue = true;
    }
    
    if(!ifCollectedSecondClue && (m_game_state.player->get_position().x >= 10.3f || (m_game_state.player->get_position().y >= -5.0f && m_game_state.player->get_position().y <= -4.5f && m_game_state.player->get_position().x >= 8.3 && m_game_state.player->get_position().x < 8.5))){
        GLuint g_font_texture_id = Utility::load_texture(FONT_FILEPATH);
        Utility::draw_text(program, g_font_texture_id, "You must collect clue in this level", 0.2f, 0.005f, glm::vec3(0.8f, -0.8f, 0.0f));
    }
    
    if(ifPlayerDied){
        m_game_state.diedScreen->render(program);
        if(diedCounter_b > 0){
            Mix_PlayChannel(-1,  m_game_state.player_killed, 0);
        }
        diedCounter_b -=1 ;
    }
    
}
  
