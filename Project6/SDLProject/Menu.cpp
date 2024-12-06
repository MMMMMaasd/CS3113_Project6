#include "Menu.h"
#include "Utility.h"

#define LEVEL_WIDTH 11
#define LEVEL_HEIGHT 15

constexpr char SPRITESHEET_FILEPATH[] = "assets/death_knight.png",
           ENEMY_FILEPATH[]       = "assets/main_tiles.png",
           FONT_FILEPATH[] = "assets/font1.png",
           RED_BOY_IMAGE_FILEPATH[] = "assets/red_boy_launch.png";

unsigned int MENU_DATA[] = {};


Menu::Menu() {}

Menu::~Menu()
{
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}


void Menu::initialise()
{
    m_game_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/main_tiles.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, MENU_DATA, map_texture_id, 1.0f, 9, 2);
    m_game_state.player = new Entity();
    m_game_state.enemies = new Entity[ENEMY_COUNT];
    
    GLuint red_boy_image_id = Utility::load_texture(RED_BOY_IMAGE_FILEPATH);
    m_game_state.extraImages = new Entity(red_boy_image_id, 1.0f, 1.0f, 1.0f, DIEDSCREEN);
    m_game_state.extraImages->set_scale(glm::vec3(5.0f, 5.0f, 0.0f));
    m_game_state.extraImages->set_position(glm::vec3(5.0f, -4.0f, 0.0f));
 

    
    /**
     BGM and SFX
     */
    
}

void Menu::update(float delta_time){
    m_game_state.extraImages->update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map, 1);
    
}

void Menu::render(ShaderProgram *program)
{
    GLuint g_font_texture_id = Utility::load_texture(FONT_FILEPATH);
    m_game_state.extraImages->render(program);
    Utility::draw_text(program, g_font_texture_id, "A Strange Case", 0.7f, 0.005f, glm::vec3(0.5f, -1.7f, 0.0f));
    Utility::draw_text(program, g_font_texture_id,  "of the", 0.7f, 0.05f, glm::vec3(3.0f, -2.6f, 0.0f));
    Utility::draw_text(program, g_font_texture_id, "Boy ", 1.0f, 0.01f, glm::vec3(1.4f, -3.5f, 0.0f));
    Utility::draw_text(program, g_font_texture_id, "in Red", 0.7f, 0.01f, glm::vec3(4.6f, -3.5f, 0.0f));
    Utility::draw_text(program, g_font_texture_id, "Press enter to start", 0.3f, 0.01f, glm::vec3(2.0f, -6.0f, 0.0f));
}

void Menu::launch(){
    m_game_state.next_scene_id = 5;
}

