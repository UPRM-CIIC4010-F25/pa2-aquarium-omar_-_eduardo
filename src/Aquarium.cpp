#include "Aquarium.h"
#include <cstdlib>


string AquariumCreatureTypeToString(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return "BiggerFish";
        case AquariumCreatureType::NPCreature:
            return "BaseFish";
        default:
            return "UknownFish";
    }
}

// PlayerCreature Implementation
PlayerCreature::PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 10.0f, 1, sprite){
    m_powerupSprite=nullptr;
    m_normalSprite=sprite;
    m_type = AquariumCreatureType::Player;
}


void PlayerCreature::setDirection(float dx, float dy) {
    m_dx = dx;
    m_dy = dy;
    normalize();
}

void PlayerCreature::move() {
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    this->bounce();
}

void PlayerCreature::reduceDamageDebounce() {
    if (m_damage_debounce > 0) {
        --m_damage_debounce;
    }
}

void PlayerCreature::update() {
    this->reduceDamageDebounce();
    this->move();
    this->updateSizeBoost();
    this->updateSpeedFruit();
}


void PlayerCreature::draw() const {
    
    ofLogVerbose() << "PlayerCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    if (this->m_damage_debounce > 0) {
        ofSetColor(ofColor::red); // Flash red if in damage debounce
    }
    if (m_sprite) {
      m_sprite->draw(m_x, m_y);
    }
    ofSetColor(ofColor::white); // Reset color

}

void PlayerCreature::changeSpeed(int speed) {
    m_speed = speed;
}

void PlayerCreature::loseLife(int debounce) {
    if (m_damage_debounce <= 0) {
        if (m_lives > 0) this->m_lives -= 1;
        m_damage_debounce = debounce; // Set debounce frames
        ofLogNotice() << "Player lost a life! Lives remaining: " << m_lives << std::endl;
    }
    // If in debounce period, do nothing
    if (m_damage_debounce > 0) {
        ofLogVerbose() << "Player is in damage debounce period. Frames left: " << m_damage_debounce << std::endl;
    }
}
void PlayerCreature::activateSizeBoost(){
if(!m_sizeActive){
    m_sizeActive=true;
    m_sizeTimer=m_sizeDuration;
    m_power +=1;
    if(!m_powerupSprite) {
        m_powerupSprite = std::make_shared<GameSprite>("pez_Espada.png", 100, 100);
        }
        this->setSprite(m_powerupSprite);
    m_sizeScale=1.5f;
     setCollisionRadius(m_defaultCollisionRad * m_sizeScale);
    ofLogNotice() << "Grow-Grow Devil Fruit Activated! Power: " << m_power;
    }
}
void PlayerCreature::updateSizeBoost(){
    if(m_sizeActive){
        if(--m_sizeTimer<=0){
            m_sizeActive=false;
            m_sizeScale=1.0f;
            m_power-=1;
          this->setSprite(m_normalSprite);
            ofLogNotice() << "Size Boost Ended. Power: " << m_power;
        }
    }
}
void PlayerCreature::activateSpeedFruit(){
    if(!m_speedFruitActive){
         m_speedFruitActive = true;
        m_speedFruitTime = m_speedFruitDuration;
        m_speedNormal = m_speed;
        m_speed *= 1.5f;
        ofLogNotice() << " Light-Speed Fruit Activated! New Speed: " << m_speed;
    }
}
void PlayerCreature::updateSpeedFruit() {
    if (m_speedFruitActive) {
        if (--m_speedFruitTime <= 0) {
            m_speedFruitActive = false;
            m_speed = m_speedNormal; // Restaura velocidad
            ofLogNotice() << "Speed Boost Ended. Speed: " << m_speed;
        }
    }
}

// NPCreature Implementation
NPCreature::NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 30, 1, sprite) {
    m_dx = (rand() % 3 - 1); // -1, 0, or 1
    m_dy = (rand() % 3 - 1); // -1, 0, or 1
    normalize();

    m_type = AquariumCreatureType::NPCreature;
}

void NPCreature::move() {
    // Simple AI movement logic (random direction)
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }
    bounce();
}

void NPCreature::draw() const {
    ofLogVerbose() << "NPCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    ofSetColor(ofColor::white);
    if (m_sprite) {
        m_sprite->draw(m_x, m_y);
    }
}


BiggerFish::BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
    m_dx = (rand() % 3 - 1);
    m_dy = (rand() % 3 - 1);
    normalize();

    setCollisionRadius(60); // Bigger fish have a larger collision radius
    m_value = 5; // Bigger fish have a higher value
    m_type = AquariumCreatureType::BiggerFish;
}

void BiggerFish::move() {
    // Bigger fish might move slower or have different logic
    m_x += m_dx * (m_speed * 0.5); // Moves at half speed
    m_y += m_dy * (m_speed * 0.5);
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }

    bounce();
}

void BiggerFish::draw() const {
    ofLogVerbose() << "BiggerFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    this->m_sprite->draw(this->m_x, this->m_y);
}
GyaradosFish::GyaradosFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
    m_value = 10;
    m_type = AquariumCreatureType::GyaradosFish;
}

void GyaradosFish::move(std::shared_ptr<PlayerCreature> player) {
    float dx = player->getX() - m_x;
    float dy = player->getY() - m_y;
    float length = sqrt(dx*dx + dy*dy);

    
    if (length > 0) { 
         if (dx < 0) {
           this-> m_sprite->setFlipped(true);
        } else {
            this->m_sprite->setFlipped(false);
        }
        m_x += (dx/length) * (m_speed * 1.2f);
        m_y += (dy/length) * (m_speed * 1.2f);
    }
    bounce();
}

void GyaradosFish::draw() const {
    m_sprite->draw(m_x, m_y);
}


// AquariumSpriteManager
AquariumSpriteManager::AquariumSpriteManager(){
    this->m_npc_fish = std::make_shared<GameSprite>("base-fish.png", 70,70);
    this->m_big_fish = std::make_shared<GameSprite>("bigger-fish.png", 120, 120);
    this->m_powerup= std::make_shared<GameSprite>("devil_Fruit.png", 40, 40);
    this->m_speed_fruit= std::make_shared<GameSprite>("kizaru_Fruit.png",40,40);
    this->m_gyarados_fish=std::make_shared<GameSprite>("gyarados.png", 140, 140);
    this->m_angler_fish = std::make_shared<GameSprite>("angler_Fish.png", 90, 90);
}

std::shared_ptr<GameSprite> AquariumSpriteManager::GetSprite(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return std::make_shared<GameSprite>(*this->m_big_fish);
            
        case AquariumCreatureType::NPCreature:
            return std::make_shared<GameSprite>(*this->m_npc_fish);
        case AquariumCreatureType::PowerUp:
            return std::make_shared<GameSprite>(*this->m_powerup);
        case AquariumCreatureType::SpeedFruit:
            return std::make_shared<GameSprite>(*this->m_speed_fruit);
        case AquariumCreatureType::GyaradosFish:
            return std::make_shared<GameSprite>(*this->m_gyarados_fish);
        case AquariumCreatureType::AnglerFish:
            return std::make_shared<GameSprite>(*this->m_angler_fish);
        default:
            return nullptr;
    }
}


// Aquarium Implementation
Aquarium::Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager)
    : m_width(width), m_height(height) {
        m_sprite_manager =  spriteManager;
    }



void Aquarium::addCreature(std::shared_ptr<Creature> creature) {
    creature->setBounds(m_width - 20, m_height - 20);
    m_creatures.push_back(creature);
}

void Aquarium::addAquariumLevel(std::shared_ptr<AquariumLevel> level){
    if(level == nullptr){return;} // guard to not add noise
    this->m_aquariumlevels.push_back(level);
}

void Aquarium::update(std::shared_ptr<PlayerCreature> player) {
    for (auto& creature : m_creatures) {
        if (creature->getType() == AquariumCreatureType::GyaradosFish || creature->getType() == AquariumCreatureType::AnglerFish){
        creature->move(player);
    }else{
        creature->move();
        }
    }
    this->Repopulate();
}

void Aquarium::draw() const {
    for (const auto& creature : m_creatures) {
        creature->draw();
    }
}


void Aquarium::removeCreature(std::shared_ptr<Creature> creature) {
    auto it = std::find(m_creatures.begin(), m_creatures.end(), creature);
    if (it != m_creatures.end()) {
        if(creature->getType()==AquariumCreatureType::NPCreature ||
            creature->getType() == AquariumCreatureType::BiggerFish){
        ofLogVerbose() << "removing creature " << endl;
        int selectLvl = this->currentLevel % this->m_aquariumlevels.size();
        this->m_aquariumlevels.at(selectLvl)->ConsumePopulation(creature->getType(), creature->getValue());
            }
        m_creatures.erase(it);
    }
}

void Aquarium::clearCreatures() {
    m_creatures.clear();
}

std::shared_ptr<Creature> Aquarium::getCreatureAt(int index) {
    if (index < 0 || size_t(index) >= m_creatures.size()) {
        return nullptr;
    }
    return m_creatures[index];
}



void Aquarium::SpawnCreature(AquariumCreatureType type) {
    int x = rand() % this->getWidth();
    int y = rand() % this->getHeight();
    int speed = 1 + rand() % 25; // Speed between 1 and 25

    switch (type) {
        case AquariumCreatureType::NPCreature:
            this->addCreature(std::make_shared<NPCreature>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::NPCreature)));
            break;
        case AquariumCreatureType::BiggerFish:
            this->addCreature(std::make_shared<BiggerFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::BiggerFish)));
            break;
        case AquariumCreatureType::PowerUp:
            this->addCreature(std::make_shared<PowerUp>(x, y, this->m_sprite_manager->GetSprite(AquariumCreatureType::PowerUp)));
            break;
        case AquariumCreatureType::SpeedFruit:
            this->addCreature(std::make_shared<SpeedFruit>(x, y, this->m_sprite_manager->GetSprite(AquariumCreatureType::SpeedFruit)));
            break;
        case AquariumCreatureType::GyaradosFish:
            this->addCreature(std::make_shared<GyaradosFish>(x, y, speed, this->m_sprite_manager->GetSprite(type)));
            break;
        case AquariumCreatureType::AnglerFish:
            this->addCreature(std::make_shared<AnglerFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::AnglerFish)));
             break;
        default:
            ofLogError() << "Unknown creature type to spawn!";
            break;
    }

}


// repopulation will be called from the levl class
// it will compose into aquarium so eating eats frm the pool of NPCs in the lvl class
// once lvl criteria met, we move to new lvl through inner signal asking for new lvl
// which will mean incrementing the buffer and pointing to a new lvl index
// En Aquarium.cpp - ACTUALIZA el método Repopulate:

void Aquarium::Repopulate() {
    ofLogVerbose("entering phase repopulation");
    
    int selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
    ofLogVerbose() << "the current index: " << selectedLevelIdx << endl;
    std::shared_ptr<AquariumLevel> level = this->m_aquariumlevels.at(selectedLevelIdx);

    
    level->update(1.0f/60.0f, this->m_player); 

    
    if(level->getCurrentWave() < level->getMaxWaves() && 
       level->m_waveTimer >= level->m_timeBetweenWaves) {
        level->spawnWave(shared_from_this()); 
    }

    if(level->isCompleted()){
        ofLogNotice() << "Level " << selectedLevelIdx << " completed! Moving to next level.";
        level->levelReset();
        this->currentLevel += 1;
        selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
        level = this->m_aquariumlevels.at(selectedLevelIdx);
        level->initialize(); 
        this->clearCreatures();
        
        
        ofLogNotice() << level->getLevelDescription();
    }

    
    std::vector<AquariumCreatureType> toRespawn = level->Repopulate();
    ofLogVerbose() << "amount to repopulate : " << toRespawn.size() << endl;
    
    for(AquariumCreatureType newCreatureType : toRespawn){
        this->SpawnCreature(newCreatureType);
    }
}


// Aquarium collision detection
std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player) {
    if (!aquarium || !player) return nullptr;
    
    for (int i = 0; i < aquarium->getCreatureCount(); ++i) {
        std::shared_ptr<Creature> npc = aquarium->getCreatureAt(i);
        if (npc && checkCollision(player, npc)) {
            return std::make_shared<GameEvent>(GameEventType::COLLISION, player, npc);
        }
    }
    return nullptr;
};

//  Imlementation of the AquariumScene

void AquariumGameScene::Update(){
    std::shared_ptr<GameEvent> event;

    this->m_player->update();

    if (this->updateControl.tick()) {
        event = DetectAquariumCollisions(this->m_aquarium, this->m_player);
        if (event != nullptr && event->isCollisionEvent()) {
            if (event->creatureB->getType() == AquariumCreatureType::PowerUp) {
            m_player->activateSizeBoost();
            m_aquarium->removeCreature(event->creatureB);
             return;
            }
        if (event->creatureB->getType() == AquariumCreatureType::SpeedFruit) {
            m_player->activateSpeedFruit();
            m_aquarium->removeCreature(event->creatureB);
             return;
                }
            ofLogVerbose() << "Collision detected between player and NPC!" << std::endl;
            float newDx = -m_player->getDx();
            float newDy= -m_player->getDy();
            m_player->setDirection(newDx,newDy);
            if(event->creatureB != nullptr){
                event->print();
                if(this->m_player->getPower() < event->creatureB->getValue()){
                    ofLogNotice() << "Player is too weak to eat the creature!" << std::endl;
                    this->m_player->loseLife(3*60); // 3 frames debounce, 3 seconds at 60fps
                    if(this->m_player->getLives() <= 0){
                        this->m_lastEvent = std::make_shared<GameEvent>(GameEventType::GAME_OVER, this->m_player, nullptr);
                        return;
                    }
                }
                else{
                    this->m_aquarium->removeCreature(event->creatureB);
                    this->m_player->addToScore(1, event->creatureB->getValue());
                    if (this->m_player->getScore() % 20 == 0) {
                        this->m_aquarium->SpawnCreature(AquariumCreatureType::PowerUp);
                            ofLogNotice() << "A Grow-Grow Devil Fruit appear! ";
                            }
                    if (this->m_player->getScore() % 15 == 0) {
                         this->m_aquarium->SpawnCreature(AquariumCreatureType::SpeedFruit);
                            ofLogNotice() << "A Light-Speed Devil Fruit appeared!";
    }
                        if (this->m_player->getScore() % 30 == 0 && this->m_player->getScore() > 0) {
                            this->m_player->increasePower(1);
           
                    ofLogNotice() << "Player grew stronger! New Power: " << this->m_player->getPower() << endl;
                    }
                    
                }
                
                

            } else {
                ofLogError() << "Error: creatureB is null in collision event." << std::endl;
            }
        }
        this->m_aquarium->update(this->m_player);
    }

}

void AquariumGameScene::Draw() {
    this->m_player->draw();
    this->m_aquarium->draw();
    this->paintAquariumHUD();

}




void AquariumGameScene::paintAquariumHUD(){
    float panelWidth = ofGetWindowWidth() - 150;
    ofDrawBitmapString("Score: " + std::to_string(this->m_player->getScore()), panelWidth, 20);
    ofDrawBitmapString("Power: " + std::to_string(this->m_player->getPower()), panelWidth, 30);
    ofDrawBitmapString("Lives: " + std::to_string(this->m_player->getLives()), panelWidth, 40);
    
    
    if(!this->m_aquarium->m_aquariumlevels.empty()) {
        int currentLevelIdx = this->m_aquarium->currentLevel % this->m_aquarium->m_aquariumlevels.size();
        auto currentLevel = this->m_aquarium->m_aquariumlevels.at(currentLevelIdx);
        
        ofDrawBitmapString("Level: " + std::to_string(this->m_aquarium->currentLevel + 1), panelWidth, 60);
        ofDrawBitmapString("Wave: " + std::to_string(currentLevel->getCurrentWave() + 1) + 
                          "/" + std::to_string(currentLevel->getMaxWaves()), panelWidth, 70);
        ofDrawBitmapString("Level Score: " + std::to_string(currentLevel->m_level_score) + 
                          "/" + std::to_string(currentLevel->m_targetScore), panelWidth, 80);
        
        
        ofDrawBitmapString(currentLevel->getLevelDescription(), 20, ofGetWindowHeight() - 20);
    }
    
    for (int i = 0; i < this->m_player->getLives(); ++i) {
        ofSetColor(ofColor::red);
        ofDrawCircle(panelWidth + i * 20, 50, 5);
    }
    ofSetColor(ofColor::white);
}

void AquariumLevel::initialize() {
    m_level_score = 0;
    m_currentWave = 0;
    m_waveTimer = 0.0f;
    m_levelCompleted = false;
    populationReset();
    setupWavePattern();
}
void AquariumLevel::update(float deltaTime, std::shared_ptr<PlayerCreature> player) {
   
    if (m_levelCompleted) return;

    
    m_waveTimer += deltaTime;

    
    if (m_currentWave < m_maxWaves && m_waveTimer >= m_timeBetweenWaves) {
        
        m_waveTimer = 0.0f;
        m_currentWave++;
    }

    
    if (m_level_score >= m_targetScore) {
        m_levelCompleted = true;
    }
}

void AquariumLevel::spawnWave(std::shared_ptr<Aquarium> aquarium) {
    if (!aquarium) return;
    
    std::vector<AquariumCreatureType> waveCreatures = getWaveCreatures(m_currentWave);
    for (auto creatureType : waveCreatures) {
        aquarium->SpawnCreature(creatureType);
    }
}

void AquariumLevel::populationReset(){
    for(auto node: this->m_levelPopulation){
        node->currentPopulation = 0; // need to reset the population to ensure they are made a new in the next level
    }
}

void AquariumLevel::ConsumePopulation(AquariumCreatureType creatureType, int power){
    for(std::shared_ptr<AquariumLevelPopulationNode> node: this->m_levelPopulation){
        ofLogVerbose() << "consuming from this level creatures" << endl;
        if(node->creatureType == creatureType){
            ofLogVerbose() << "-cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            if(node->currentPopulation > 0){
                node->currentPopulation -= 1;
                m_level_score += power;
                

             
            
            ofLogVerbose() << "+cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            if(m_level_score >= m_targetScore) {
                    m_levelCompleted = true;
                }
            }
            return;
        }
    }
}   


bool AquariumLevel::isCompleted(){
    return m_levelCompleted || m_level_score >= m_targetScore;
}





std::vector<AquariumCreatureType> Level_0::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    if (m_currentWave >= m_maxWaves) {
        for(std::shared_ptr<AquariumLevelPopulationNode> node : m_levelPopulation){
            int delta = node->population - node->currentPopulation;
            if(delta > 0){
                for(int i = 0; i < delta; i++){
                    toRepopulate.push_back(node->creatureType);
                }
                node->currentPopulation += delta;
            }
        }
    }
    
    return toRepopulate;
}

void Level_0::setupWavePattern() {
    m_maxWaves = 3;
    m_timeBetweenWaves = 12.0f;
}

std::vector<AquariumCreatureType> Level_0::getWaveCreatures(int waveNumber) {
    std::vector<AquariumCreatureType> waveCreatures;
    
    switch(waveNumber) {
        case 0: 
            for(int i = 0; i < 4; i++) {
                waveCreatures.push_back(AquariumCreatureType::NPCreature);
            }
            break;
        case 1: 
            for(int i = 0; i < 6; i++) {
                waveCreatures.push_back(AquariumCreatureType::NPCreature);
            }
            break;
        case 2: 
            for(int i = 0; i < 4; i++) {
                waveCreatures.push_back(AquariumCreatureType::NPCreature);
            }
            waveCreatures.push_back(AquariumCreatureType::BiggerFish);
            break;
    }
    
    return waveCreatures;
}

std::string Level_0::getLevelDescription() const {
    return "Nivel 1: Ecosistema Basico - Peces Dorados Pacificos";
}



std::vector<AquariumCreatureType> Level_1::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    
    if (m_currentWave >= m_maxWaves) {
        for(std::shared_ptr<AquariumLevelPopulationNode> node : m_levelPopulation){
            int delta = node->population - node->currentPopulation;
            if(delta > 0){
                for(int i = 0; i < delta; i++){
                    toRepopulate.push_back(node->creatureType);
                }
                node->currentPopulation += delta;
            }
        }
    }
    
    return toRepopulate;
}

void Level_1::setupWavePattern() {
    m_maxWaves = 4;
    m_timeBetweenWaves = 10.0f;
}

std::vector<AquariumCreatureType> Level_1::getWaveCreatures(int waveNumber) {
    std::vector<AquariumCreatureType> waveCreatures;
    
    switch(waveNumber) {
        case 0: 
            for(int i = 0; i < 4; i++) {
                waveCreatures.push_back(AquariumCreatureType::NPCreature);
            }
            waveCreatures.push_back(AquariumCreatureType::BiggerFish);
            break;
        case 1: 
            for(int i = 0; i < 3; i++) {
                waveCreatures.push_back(AquariumCreatureType::NPCreature);
            }
            for(int i = 0; i < 2; i++) {
                waveCreatures.push_back(AquariumCreatureType::BiggerFish);
            }
            break;
        case 2: 
            for(int i = 0; i < 2; i++) {
                waveCreatures.push_back(AquariumCreatureType::NPCreature);
            }
            for(int i = 0; i < 3; i++) {
                waveCreatures.push_back(AquariumCreatureType::BiggerFish);
            }
            waveCreatures.push_back(AquariumCreatureType::GyaradosFish);
            break;
        case 3: 
            for(int i = 0; i < 2; i++) {
                waveCreatures.push_back(AquariumCreatureType::BiggerFish);
            }
            for(int i = 0; i < 2; i++) {
                waveCreatures.push_back(AquariumCreatureType::GyaradosFish);
            }
            break;
    }
    
    return waveCreatures;
}

std::string Level_1::getLevelDescription() const {
    return "Nivel 2: Arrecife de Coral - Aparecen Gyarados!";
}



std::vector<AquariumCreatureType> Level_2::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    
    if (m_currentWave >= m_maxWaves) {
        for(std::shared_ptr<AquariumLevelPopulationNode> node : m_levelPopulation){
            int delta = node->population - node->currentPopulation;
            if(delta > 0){
                for(int i = 0; i < delta; i++){
                    toRepopulate.push_back(node->creatureType);
                }
                node->currentPopulation += delta;
            }
        }
    }
    
    return toRepopulate;
}

void Level_2::setupWavePattern() {
    m_maxWaves = 5;
    m_timeBetweenWaves = 8.0f;
}

std::vector<AquariumCreatureType> Level_2::getWaveCreatures(int waveNumber) {
    std::vector<AquariumCreatureType> waveCreatures;
    
    switch(waveNumber) {
        case 0: 
            for(int i = 0; i < 3; i++) {
                waveCreatures.push_back(AquariumCreatureType::NPCreature);
            }
            waveCreatures.push_back(AquariumCreatureType::BiggerFish);
            waveCreatures.push_back(AquariumCreatureType::AnglerFish);
            break;
        case 1: 
            for(int i = 0; i < 2; i++) {
                waveCreatures.push_back(AquariumCreatureType::NPCreature);
            }
            for(int i = 0; i < 2; i++) {
                waveCreatures.push_back(AquariumCreatureType::BiggerFish);
            }
            waveCreatures.push_back(AquariumCreatureType::GyaradosFish);
            break;
        case 2: 
            waveCreatures.push_back(AquariumCreatureType::NPCreature);
            for(int i = 0; i < 2; i++) {
                waveCreatures.push_back(AquariumCreatureType::BiggerFish);
            }
            for(int i = 0; i < 2; i++) {
                waveCreatures.push_back(AquariumCreatureType::AnglerFish);
            }
            break;
        case 3: 
            for(int i = 0; i < 2; i++) {
                waveCreatures.push_back(AquariumCreatureType::BiggerFish);
            }
            for(int i = 0; i < 2; i++) {
                waveCreatures.push_back(AquariumCreatureType::GyaradosFish);
            }
            for(int i = 0; i < 2; i++) {
                waveCreatures.push_back(AquariumCreatureType::AnglerFish);
            }
            break;
        case 4: 
            for(int i = 0; i < 3; i++) {
                waveCreatures.push_back(AquariumCreatureType::GyaradosFish);
            }
            for(int i = 0; i < 3; i++) {
                waveCreatures.push_back(AquariumCreatureType::AnglerFish);
            }
            break;
    }
    
    return waveCreatures;
}

std::string Level_2::getLevelDescription() const {
    return "Nivel 3: Oceano Profundo - Peligros y Maravillas!";
}