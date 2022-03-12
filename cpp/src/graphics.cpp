#include <boost/shared_ptr.hpp>
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "engine.h"
#include "graphics.h"
#include "common.h"
#include "config.h"

using namespace std;

sf::Font mainFont;

sf::RenderWindow* setupGraphics()
{
    if (!mainFont.loadFromFile("Andale_Mono.ttf"))
        throw runtime_error("Can't load font");

    sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Coinfight Client", sf::Style::Close | sf::Style::Titlebar);

    // const uint8_t pixels[] =
    //     {255, 0, 0, 255,   0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,
    //     0, 0, 0, 0,   255, 0, 0, 255,   0, 0, 0, 0,   0, 0, 0, 0,
    //     0, 0, 0, 0,   0, 0, 0, 0,   255, 0, 0, 255,   0, 0, 0, 0,
    //     0, 0, 0, 0,   0, 0, 0, 0,   0, 0, 0, 0,   255, 0, 0, 255
    //     };
    // sf::Vector2u size(4, 4);
    // sf::Vector2u hotspot(0,0);

    // sf::Cursor cursor;
    // if (cursor.loadFromPixels(
    //     pixels,
    //     size,
    //     hotspot
    // ))
    //     window->setMouseCursor(cursor);
    
    return window;
}

void drawEntity(sf::RenderWindow *window, boost::shared_ptr<Entity> entity, CameraState camera)
{
    vector2i drawPos = gamePosToScreenPos(camera, vector2i(entity->pos));
    sf::Color primaryColor = entity->getPrimaryColor();
    float drawRotation = -entity->getRotation();

    if (boost::shared_ptr<GoldPile> goldPile = boost::dynamic_pointer_cast<GoldPile, Entity>(entity))
    {
        int size = ceil(sqrt(goldPile->gold.getInt() / 30.0)) + 1;
        if (size > 1)
        {
            sf::CircleShape triangle(size, 3);
            triangle.setOrigin(triangle.getRadius(), triangle.getRadius());
            triangle.setFillColor(primaryColor);
            triangle.setPosition(drawPos.x, drawPos.y);
            window->draw(triangle);
        }
    }
    else if (boost::shared_ptr<Prime> castedEntity = boost::dynamic_pointer_cast<Prime, Entity>(entity))
    {
        sf::ConvexShape oneSide;
        oneSide.setPointCount(3);

        oneSide.setFillColor(primaryColor);
        oneSide.setPosition(drawPos.x, drawPos.y);
        oneSide.setRotation(radToDeg(drawRotation));

        // draw two triangles
        oneSide.setPoint(1, sf::Vector2f(12, 0));
        oneSide.setPoint(0, sf::Vector2f(-4, 0));
        oneSide.setPoint(2, sf::Vector2f(-12, 8));
        window->draw(oneSide);
        oneSide.setPoint(2, sf::Vector2f(-12, -8));
        window->draw(oneSide);
    }
    else if (boost::shared_ptr<Gateway> castedEntity = boost::dynamic_pointer_cast<Gateway, Entity>(entity))
    {
        sf::RectangleShape rect(sf::Vector2f(20, 20));
        rect.setOrigin(10, 10);
        rect.setFillColor(primaryColor);
        rect.setPosition(drawPos.x, drawPos.y);
        window->draw(rect);
    }
    else
    {
        throw runtime_error("No drawing code implemented for " + entity->getTypeName() + ".");
    }
}

void drawOutputStrings(sf::RenderWindow *window, vector<sf::String> strings)
{
    for (uint i=0; i<strings.size(); i++)
    {
        sf::Text text(strings[i], mainFont, 16);
        text.setFillColor(sf::Color(150, 150, 150));

        float width = text.getLocalBounds().width;
        int x, y;
        x = window->getSize().x - width - 10;
        y = 6 + i * 20;
        text.setPosition(sf::Vector2f(x, y));

        window->draw(text);
    }
}

void drawSelectableCursor(sf::RenderWindow *window, vector2i mousePos)
{
    sf::Transform mouseTransform;
    mouseTransform.translate(mousePos.x, mousePos.y);

    sf::CircleShape circle(10);
    circle.setOrigin(5, 5);
    circle.setOutlineColor(sf::Color::Green);
    circle.setFillColor(sf::Color::Transparent);
    circle.setOutlineThickness(2);

    window->draw(circle, mouseTransform);
}

void drawCursor(sf::RenderWindow *window, UI ui)
{
    vector2i mousePos = vector2i(sf::Mouse::getPosition(*window).x, sf::Mouse::getPosition(*window).y);

    switch (ui.cursorState)
    {
        case UI::CursorState::Normal:
            window->setMouseCursorVisible(true);
            break;
        case UI::CursorState::Selectable:
            window->setMouseCursorVisible(false);
            drawSelectableCursor(window, mousePos);
            break;
    }
}

void display(sf::RenderWindow *window, Game *game, UI ui, int playerIdOrNegativeOne)
{
    window->clear();

    for (unsigned int i = 0; i < game->entities.size(); i++)
    {
        if (game->entities[i])
            drawEntity(window, game->entities[i], ui.camera);
    }

    vector<sf::String> outputStrings;

    if (playerIdOrNegativeOne >= 0)
    {
        uint playerId = playerIdOrNegativeOne;
        outputStrings.push_back(game->players[playerId].credit.getDollarString());

        drawOutputStrings(window, outputStrings);
    }

    drawCursor(window, ui);
    
    window->display();
}