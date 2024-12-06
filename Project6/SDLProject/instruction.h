#include "Scene.h"

class Instruction : public Scene {
public:
    int ENEMY_COUNT = 1;
    
    Instruction();
    Instruction(int currentPlayerLives, bool ifCollectFirstClue, bool ifCollectSecondClue, bool ifCollectThirdClue);
    
    ~Instruction();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    
    void launch() override;
};
