#include "pch.h"
#include "GameFramework.h"
#include "Global.h"
#include "Player.h"
#include "Monster.h"
#include "Item.h"
#include "Bar.h"

GameFramework::GameFramework(sf::RenderWindow* window)
{	
	// 윈도우창 생성
	_window = window;

    _isInGame = false;
    _isOpenInventory = false;
    _isOpenEquipment = false;
    _dragItem = nullptr;
    _avatar = nullptr;

    // UI
    _barBackground.setPosition(470, 870);
    _barBackground.setSize(sf::Vector2f(110, 75));
    _barBackground.setFillColor(sf::Color::White);
    _hpBar = new Bar({475, 900}, {100, 15}, UIType::HpBar);
    _expBar = new Bar({ 475, 925 }, { 100, 15 }, UIType::ExpBar);

    _map = g_dataManager->GetTilemap();

    _texture.loadFromFile("Resource/Tile0.png");
    _sprite.setTexture(_texture);
}

GameFramework::~GameFramework()
{
}

void GameFramework::Update()
{
    sf::Event event;
    while (_window->pollEvent(event))
    {
        // 창 닫기 버튼 누르면 종료
        if (event.type == sf::Event::Closed)
            _window->close();
    }

    // 처음 클릭했을 때
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && _isOpenInventory)
        {
            sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

            for (auto& item : _inventory.GetItems())
            {
                if (item && item->GetInInventorySprite().getGlobalBounds().contains(mousePos))
                {
                    _dragItem = item;
                    _dragItem->SetIsClick(true);
                    break;
                }
            }

            if (_equipment.GetCurrentWeapon() && _equipment.GetEquipmentSprite().getGlobalBounds().contains(mousePos))
            {
                _dragItem = _equipment.GetCurrentWeapon();
                _dragItem->SetIsClick(true);
            }
        }
    }

    // 클릭중
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && _dragItem)
    {
        sf::Vector2i currentMousePos = sf::Mouse::getPosition(*_window);
        sf::Vector2f mousePos(static_cast<float>(currentMousePos.x), static_cast<float>(currentMousePos.y));

        _dragItem->SetInInventorySpritePos(mousePos);
    }

    // 클릭을 그만뒀을 때
    if (event.type == sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == sf::Mouse::Left && _isOpenInventory && _dragItem)
        {
            sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

            if (!_inventory.GetInventorySprite().getGlobalBounds().contains(mousePos) && !_equipment.GetEquipmentSprite().getGlobalBounds().contains(mousePos))
            {
                g_network->SendDropItemPacket(_dragItem->GetID());
                if (_dragItem == _equipment.GetCurrentWeapon())
                {
                    _equipment.SetCurrentWeapon(nullptr);
                    g_network->SendChangeWeaponPacket(-1);
                }
            }

            if (_equipment.GetEquipmentSprite().getGlobalBounds().contains(mousePos))
            {
                _equipment.SetCurrentWeapon(_dragItem);
            }

            _dragItem->SetIsClick(false);
            _dragItem = nullptr;
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) && !_avatar)
    {
        strncpy_s(_userName, "abc", sizeof(_userName) - 1);
        g_network->SendLoginPacket(_userName);
    }
       
    if (_avatar && _window->hasFocus())
    {
        if (_avatar->IsCanMove())
        {
            Vector pos = _avatar->GetPos();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                pos.x -= TILE_SIZE;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                pos.x += TILE_SIZE;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                pos.y -= TILE_SIZE;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                pos.y += TILE_SIZE;

            if (IsCanGo(pos))
            {
                _avatar->SetPos(pos);
                g_network->SendMovePacket(_avatar);
            }
        }

        if (_avatar->IsCanAttack())
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                g_network->SendAttackPacket();
        }
        
        if (_equipment.GetCurrentWeapon() /*&& _avatar->IsCanSkill()*/)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                g_network->SendSkillPacket();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
        {
            if (_isOpenInventory)
                _isOpenInventory = false;
            else
                _isOpenInventory = true;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        {
            if (_isOpenEquipment)
                _isOpenEquipment = false;
            else
                _isOpenEquipment = true;
        }
    }

    if (_avatar)
    {
        Vector cameraPos = _avatar->GetPos();

        cameraPos.x = std::max(WINDOW_WIDTH / 2, std::min(cameraPos.x, WORLD_WIDTH * TILE_SIZE - WINDOW_WIDTH / 2));
        cameraPos.y = std::max(WINDOW_HEIGHT / 2, std::min(cameraPos.y, WORLD_HEIGHT * TILE_SIZE - WINDOW_HEIGHT / 2));

        _gameView.setCenter(cameraPos.x, cameraPos.y);
    }
}

void GameFramework::Render()
{
    _window->clear();

    if (_isInGame)
    {
        _window->setView(_gameView);
        sf::Vector2f viewPos = _gameView.getCenter();
        Vector index{ static_cast<int>(viewPos.x) / TILE_SIZE, static_cast<int>(viewPos.y) / TILE_SIZE };

        int viewRange = 11;
        Vector start{ std::max(0, index.x - viewRange), std::max(0, index.y - viewRange) };
        Vector end{ std::min(WORLD_WIDTH - 1, index.x + viewRange), std::min(WORLD_HEIGHT - 1, index.y + viewRange) };

        for (int y = start.y; y <= end.y; ++y)
        {
            for (int x = start.x; x <= end.x; ++x)
            {
                _sprite.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                _window->draw(_sprite);
            }
        }

        for (auto& [id, object] : _objects)
            object->Render(_window);

        if (_avatar)
        {
            _avatar->Render(_window);

            _window->setView(_uiView);

            _window->draw(_barBackground);
            _hpBar->Render(_window);
            _expBar->Render(_window);

            std::string uiText = "Lv " + std::to_string(_avatar->GetLevel()) + "   " + _userName;
            sf::Text text;
            text.setFont(g_dataManager->GetFont());
            text.setString(uiText);
            text.setCharacterSize(25);
            text.setFillColor(sf::Color::Black);
            text.setPosition(475, 870);
            _window->draw(text);
        }  

        if (_isOpenInventory)
            _inventory.Render(_window);

        if (_isOpenEquipment)
            _equipment.Render(_window);
    }
    else
    {

    }

    _window->display();
}

void GameFramework::LoadTile()
{

}

void GameFramework::CreateAvatar(int playerId, int visualId, short x, short y, int hp, int maxHp, long long exp, int level)
{
    _avatar = std::make_shared<Player>();
    _avatar->SetID(playerId);
    _avatar->SetPos(x, y);
    _avatar->SetHP(hp);
    _avatar->SetMaxHP(maxHp);
    _avatar->SetExp(exp);
	_avatar->SetLevel(level);
    _avatar->SetName(_userName);

    _hpBar->SetMaxValue(maxHp);
    _hpBar->SetCurrentValue(hp);
    _expBar->SetMaxValue(pow(2, level - 1) * 100);
    _expBar->SetCurrentValue(exp);
}

void GameFramework::AddCreature(int id, int visualID, const char* name, short x, short y, int hp, int maxHp, long long exp, int level)
{
    CreatureRef creture;
    
    switch (static_cast<ObjectType>(visualID))
    {
    case ObjectType::Player:
        creture = std::make_shared<Player>();
        creture->SetName(name);
        break;
    case ObjectType::Agro:
        creture = std::make_shared<Monster>(static_cast<ObjectType>(visualID));
        break;
    case ObjectType::Peace:
        creture = std::make_shared<Monster>(static_cast<ObjectType>(visualID));
        break;
    }

    creture->SetID(id);
    creture->SetPos(x, y);
    creture->SetHP(hp);
    creture->SetMaxHP(maxHp);
    creture->SetExp(exp);
    creture->SetLevel(level);
    creture->SetName(name);

    _objects[id] = creture;
}

void GameFramework::AddItem(int id, ObjectType type, Vector pos)
{
    ItemRef item;

    switch (type)
    {
    case ObjectType::Sword:
        item = std::make_shared<Item>(type);
        break;
    }

    item->SetID(id);
    item->SetPos(pos);

    _objects[id] = item;
}

void GameFramework::RemoveObject(int id)
{
    _objects.erase(id);
}

bool GameFramework::IsCanGo(Vector pos)
{
    Vector index = pos / TILE_SIZE;
    Vector max{ static_cast<int>(_map[0].size()), static_cast<int>(_map.size()) };

    if (index < Vector{ 0, 0 } || index >= max)
        return false;

    if (_map[index.x][index.y] != 0)
        return false;

    return true;
}

GameObjectRef GameFramework::GetGameObject(int id)
{
    if(_objects.count(id))
        return _objects[id];

    return nullptr;
}