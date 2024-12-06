#include "Scene.h"

class LevelDied : public Scene {
public:
    int ENEMY_COUNT = 1;
    
    LevelDied();
    
    LevelDied(int currentPlayerLives, bool ifCollectFirstClue, bool ifCollectSecondClue, bool ifCollectThirdClue);
    
    ~LevelDied();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
