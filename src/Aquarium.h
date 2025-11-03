#define NOMINMAX // To avoid min/max macro conflict on Windows

#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include "Core.h"


enum class AquariumCreatureType {
    Player,
    NPCreature,
    BiggerFish,
    PowerUp,
    SpeedFruit,
    GyaradosFish,
    AnglerFish
};

string AquariumCreatureTypeToString(AquariumCreatureType t);

class AquariumLevelPopulationNode{
    public:
        AquariumLevelPopulationNode() = default;
        AquariumLevelPopulationNode(AquariumCreatureType creature_type, int population) {
            this->creatureType = creature_type;
            this->population = population;
            this->currentPopulation = 0;
        };
        AquariumCreatureType creatureType;
        int population;
        int currentPopulation;
};
class Aquarium;
class AquariumLevel;
class AquariumLevel : public GameLevel {
    public:
        AquariumLevel(int levelNumber, int targetScore)
        : GameLevel(levelNumber), m_level_score(0), m_targetScore(targetScore){};
        void ConsumePopulation(AquariumCreatureType creature, int power);
        bool isCompleted() override;
        void populationReset();
        void levelReset(){m_level_score=0;this->populationReset();}
        virtual std::vector<AquariumCreatureType> Repopulate() = 0;
        virtual void initialize();
        virtual void update(float deltaTime, std::shared_ptr<PlayerCreature> player);
        virtual void spawnWave(std::shared_ptr<Aquarium> aquarium);
        int getCurrentWave() const { return m_currentWave; }
        int getMaxWaves() const { return m_maxWaves; }
        virtual std::string getLevelDescription() const = 0;

        float getWaveTimer() const{return m_waveTimer;}
        float getTimeBetweenWaves() const{return m_timeBetweenWaves;}
        int getLevelScore() const{return m_level_score;}
        int getTargetScore() const{return m_targetScore;}
        void forceAdvanceWave() {
            m_waveTimer = 0.0f;
            m_currentWave++;
            ofLogNotice() << "Nueva wave -> " << m_currentWave;
        }
        void forceFinishLevel() {
        m_levelCompleted = true;
        }

    protected:
        std::vector<std::shared_ptr<AquariumLevelPopulationNode>> m_levelPopulation;
        int m_level_score;
        int m_targetScore;
        int m_currentWave;
        int m_maxWaves;
        float m_timeBetweenWaves;
        float m_waveTimer;
        bool m_levelCompleted;
        virtual void setupWavePattern() = 0;
        virtual std::vector<AquariumCreatureType> getWaveCreatures(int waveNumber) = 0;

};


class PlayerCreature : public Creature {
public:

    PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move();
    void draw() const;
    void update();
    void changeSpeed(int speed);
    void setLives(int lives) { m_lives = lives; }
    void setDirection(float dx, float dy);
    float isXDirectionActive() { return m_dx != 0; }
    float isYDirectionActive() {return m_dy != 0; }
    float getDx() { return m_dx; }
    float getDy() { return m_dy; }

    int getScore()const { return m_score; }
    int getLives() const { return m_lives; }
    int getPower() const { return m_power; }
    
    void addToScore(int amount, int weight=1) { m_score += amount * weight; }
    void loseLife(int debounce);
    void increasePower(int value) { m_power += value; }
    void reduceDamageDebounce();
    void activateSizeBoost();
    void updateSizeBoost();
    void activateSpeedFruit();
    void updateSpeedFruit();
    
private:
    int m_score = 0;
    int m_lives = 3;
    int m_power = 1; // mark current power lvl
    int m_damage_debounce = 0; // frames to wait after eating
    bool m_sizeActive=false;
    float m_sizeScale=1.0;
    int m_sizeTimer=0;
    const int m_sizeDuration= 15*60;
    float m_defaultCollisionRad=25.0;
    std::shared_ptr<GameSprite> m_powerupSprite;
    std::shared_ptr<GameSprite> m_normalSprite;
    bool m_speedFruitActive=false;
    int m_speedFruitTime=0;
    const int m_speedFruitDuration=7*60;
    int m_speedNormal=0;
};
class NPCreature : public Creature {
public:
    NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    AquariumCreatureType GetType() {return this->m_creatureType;}
    void move() override;
    void draw() const override;
protected:
    AquariumCreatureType m_creatureType;

};

class BiggerFish : public NPCreature {
public:
    BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move() override;
    void draw() const override;
};
class GyaradosFish : public NPCreature {
    public:
    GyaradosFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move(std::shared_ptr<PlayerCreature> player) override;
    void draw() const override;
};
class PowerUp : public Creature{
public:
    PowerUp(float x, float y, std::shared_ptr<GameSprite> sprite)
        : Creature(x, y, 0, 20.0f, 0, sprite)
    {
        m_type = AquariumCreatureType::PowerUp;
    }

    void move() override {};
    void draw() const override {m_sprite->draw(m_x, m_y);}
};
class SpeedFruit : public Creature{
public:
    SpeedFruit(float x, float y, std::shared_ptr<GameSprite> sprite)
        : Creature(x, y, 0, 20.0f, 0, sprite){
        m_type= AquariumCreatureType::SpeedFruit;
    }
    void move() override {}
    void draw() const override {m_sprite->draw(m_x, m_y);}
};
class AnglerFish : public NPCreature{
    public:
    AnglerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
        : NPCreature(x, y, speed, sprite) {
        m_value = 5;
        m_type = AquariumCreatureType::AnglerFish;
    }

  void move(std::shared_ptr<PlayerCreature> player) {
        float dx = player->getX() - m_x;
        float dy = player->getY() - m_y;
        float distancia = sqrt(dx*dx + dy*dy);

        if (distancia < 150) {  
            m_dx = -dx/distancia;
            m_dy = -dy/distancia;
            m_x += m_dx* (m_speed*1.6f);
            m_y += m_dy* (m_speed*1.6f);
        } else {
        m_x += m_dx * m_speed;
        m_y += m_dy * m_speed;
        }
         if (m_dx < 0) {
            this->m_sprite->setFlipped(true);
        } else {
            this->m_sprite->setFlipped(false);
        }
        bounce();
    }
    void draw() const override {
        m_sprite->draw(m_x, m_y);
    }
};

class AquariumSpriteManager {
    public:
        AquariumSpriteManager();
        ~AquariumSpriteManager() = default;
        std::shared_ptr<GameSprite>GetSprite(AquariumCreatureType t);
    private:
        std::shared_ptr<GameSprite> m_npc_fish;
        std::shared_ptr<GameSprite> m_big_fish;
        std::shared_ptr<GameSprite> m_powerup;
        std::shared_ptr<GameSprite> m_speed_fruit;
        std::shared_ptr<GameSprite> m_gyarados_fish;
        std::shared_ptr<GameSprite> m_angler_fish;
};


class Aquarium :public std::enable_shared_from_this<Aquarium>{
public:
    Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager);
    void addCreature(std::shared_ptr<Creature> creature);
    void addAquariumLevel(std::shared_ptr<AquariumLevel> level);
    void removeCreature(std::shared_ptr<Creature> creature);
    void clearCreatures();
    void update(std::shared_ptr<PlayerCreature> player);
    void draw() const;
    void setBounds(int w, int h) { m_width = w; m_height = h; }
    void setMaxPopulation(int n) { m_maxPopulation = n; }
    void Repopulate(std::shared_ptr<PlayerCreature> player);
    void SpawnCreature(AquariumCreatureType type);
    
    std::shared_ptr<Creature> getCreatureAt(int index);
    int getCreatureCount() const { return m_creatures.size(); }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getCurrentLevelIndex() const{return currentLevel;}
    int getLevelCount() const{return m_aquariumlevels.size();}
    std::shared_ptr<AquariumLevel> getLevel(int index) const{return m_aquariumlevels.at(index); }

private:
    int m_maxPopulation = 0;
    int m_width;
    int m_height;
    int currentLevel = 0;
    std::vector<std::shared_ptr<Creature>> m_creatures;
    std::vector<std::shared_ptr<Creature>> m_next_creatures;
    std::vector<std::shared_ptr<AquariumLevel>> m_aquariumlevels;
    std::shared_ptr<AquariumSpriteManager> m_sprite_manager;
};


std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player);


class AquariumGameScene : public GameScene {
    public:
        AquariumGameScene(std::shared_ptr<PlayerCreature> player, std::shared_ptr<Aquarium> aquarium, string name)
        : m_player(std::move(player)) , m_aquarium(std::move(aquarium)), m_name(name){}
        std::shared_ptr<GameEvent> GetLastEvent(){return m_lastEvent;}
        void SetLastEvent(std::shared_ptr<GameEvent> event){this->m_lastEvent = event;}
        std::shared_ptr<PlayerCreature> GetPlayer(){return this->m_player;}
        std::shared_ptr<Aquarium> GetAquarium(){return this->m_aquarium;}
        string GetName()override {return this->m_name;}
        void Update() override;
        void Draw() override;
    private:
        void paintAquariumHUD();
        std::shared_ptr<PlayerCreature> m_player;
        std::shared_ptr<Aquarium> m_aquarium;
        std::shared_ptr<GameEvent> m_lastEvent;
        string m_name;
        AwaitFrames updateControl{5};
};


class Level_0 : public AquariumLevel  {
    public:
        Level_0(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 14));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::AnglerFish, 4));
        }

        std::vector<AquariumCreatureType> Repopulate() override;

        void spawnWave(std::shared_ptr<Aquarium> aquarium) override;
        void setupWavePattern() override;
        std::vector<AquariumCreatureType> getWaveCreatures(int waveNumber) override;
        std::string getLevelDescription() const override;
};

class Level_1 : public AquariumLevel  {
    public:
        Level_1(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 9));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 5));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::AnglerFish, 3));
        }
    
        
        std::vector<AquariumCreatureType> Repopulate() override;

        void spawnWave(std::shared_ptr<Aquarium> aquarium) override;
        void setupWavePattern() override;
        std::vector<AquariumCreatureType> getWaveCreatures(int waveNumber) override;
        std::string getLevelDescription() const override;

};
class Level_2 : public AquariumLevel  {
    public:
        Level_2(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 6));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 8));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::GyaradosFish, 6));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::AnglerFish, 6));
        }

        std::vector<AquariumCreatureType> Repopulate() override;

        void spawnWave(std::shared_ptr<Aquarium> aquarium) override;
        void setupWavePattern() override;
        std::vector<AquariumCreatureType> getWaveCreatures(int waveNumber) override;
        std::string getLevelDescription() const override;
};
