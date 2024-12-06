#include "Scene.h"

class Intro : public Scene {
public:
    int ENEMY_COUNT = 1;
    
    Intro();
    Intro(int currentPlayerLives, bool ifCollectFirstClue, bool ifCollectSecondClue, bool ifCollectThirdClue);
    
    ~Intro();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    
    void launch() override;
};
