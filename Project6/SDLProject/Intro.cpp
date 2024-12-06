#include "Intro.h"
#include "Utility.h"

#define LEVEL_WIDTH 11
#define LEVEL_HEIGHT 15

constexpr char SPRITESHEET_FILEPATH[] = "assets/death_knight.png",
           ENEMY_FILEPATH[]       = "assets/main_tiles.png",
           FONT_FILEPATH[] = "assets/font1.png",
           RED_BOY_IMAGE_FILEPATH[] = "assets/introduction.png",
           NEWS_FILEPATH[] = "assets/20241102_NEWS_sm.png";


unsigned int Intro_DATA[] = {};


Intro::Intro() {}

Intro::Intro(int currentPlayerLives, bool ifCollectFirstClue, bool ifCollectSecondClue, bool ifCollectThirdClue) : Scene(currentPlayerLives, ifCollectFirstClue, ifCollectSecondClue, ifCollectThirdClue) {}

Intro::~Intro()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}


void Intro::initialise()
{
    m_game_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/main_tiles.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, Intro_DATA, map_texture_id, 1.0f, 9, 2);
    m_game_state.player = new Entity();
    m_game_state.enemies = new Entity[ENEMY_COUNT];
    
    GLuint red_boy_image_id = Utility::load_texture(RED_BOY_IMAGE_FILEPATH);
    GLuint news_id = Utility::load_texture(NEWS_FILEPATH);
    m_game_state.extraImages = new Entity[2];
    
    m_game_state.extraImages[0] =  Entity(red_boy_image_id, 1.0f, 1.0f, 1.0f, DIEDSCREEN);
    m_game_state.extraImages[1] =  Entity(news_id, 1.0f, 1.0f, 1.0f, DIEDSCREEN);
    
    for (int i = 0; i < 2; i++)
    {
        m_game_state.extraImages[i].set_scale(glm::vec3(5.0f, 5.0f, 0.0f));
        m_game_state.extraImages[i].set_position(glm::vec3(5.0f, -4.0f, 0.0f));
    }
    
    m_game_state.extraImages[0].set_scale(glm::vec3(5.0f, 5.0f, 0.0f));
    m_game_state.extraImages[0].set_position(glm::vec3(7.0f, -4.0f, 0.0f));
    m_game_state.extraImages[1].set_scale(glm::vec3(4.0f, 4.0f, 0.0f));
    m_game_state.extraImages[1].set_position(glm::vec3(2.3f, -4.0f, 0.0f));
    

    
    /**
     BGM and SFX
     */
    
}

void Intro::update(float delta_time){
    m_game_state.extraImages[0].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map, 1);
    m_game_state.extraImages[1].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map, 1);
}

void Intro::render(ShaderProgram *program)
{
    GLuint g_font_texture_id = Utility::load_texture(FONT_FILEPATH);
    Utility::draw_text(program, g_font_texture_id, "Introduction", 0.7f, 0.005f, glm::vec3(0.5f, -1.0f, 0.0f));
    m_game_state.extraImages[0].render(program);
    m_game_state.extraImages[1].render(program);
    Utility::draw_text(program, g_font_texture_id, "Press enter to start", 0.3f, 0.01f, glm::vec3(2.0f, -6.5f, 0.0f));
}


void Intro::launch(){
    m_game_state.next_scene_id = 7;
}

