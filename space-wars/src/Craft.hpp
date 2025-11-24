#include <SFML/System/Angle.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <random>


class Craft : public sf::Drawable, public sf::Transformable
{
public:
    Craft(int craft_type) : m_type(craft_type), m_craft(sf::PrimitiveType::Triangles, 6)
    {
    }

    void set_position(float x, float y)
    {
        m_position = {x, y};
    }
    
    void set_position(sf::Vector2f position)
    {
        m_position = position;
    }
    
    void set_heading(sf::Angle heading)
    {
        m_heading = heading + sf::degrees(90.0f);
    }
    
    void set_pose(sf::Vector2f position, sf::Angle heading)
    {
        set_position(position);
        set_heading(heading);
    }
    
    void set_pose(float x, float y, sf::Angle heading)
    {
        set_position({x, y});
        set_heading(heading);
    }
        
    void increment_heading(float rate = 1.0f)
    {
        m_heading += sf::degrees(rate);
        if(m_heading > sf::degrees(360.0f))
            m_heading = m_heading - sf::degrees(360.0f);
    }

    void decrement_heading(float rate = 1.0f)
    {
        m_heading -= sf::degrees(rate);
        if(m_heading < sf::degrees(0.0f))
            m_heading = m_heading + sf::degrees(360.0f);
    }

    void update()
    {
        if(m_type == Constants::CRAFT_1)
            draw_craft_1();
        else if(m_type == Constants::CRAFT_2)
            draw_craft_2();
        else
            draw_unknown();
    }

    void draw_craft_1()
    {
        // define the position of the triangle's points
        m_craft[0].position = sf::Vector2f(100.f, 240.f);
        m_craft[1].position = sf::Vector2f(0.f, 300.f);
        m_craft[2].position = sf::Vector2f(100.f, 0.f);
        
        // define the color of the triangle's points
        m_craft[0].color = sf::Color::Green;
        m_craft[1].color = sf::Color::Blue;
        m_craft[2].color = sf::Color::Green;       

        // define the position of the triangle's points
        m_craft[3].position = sf::Vector2f(100.f, 240.f);
        m_craft[4].position = sf::Vector2f(200.f, 300.f);
        m_craft[5].position = sf::Vector2f(100.f, 0.f);
        
        // define the color of the triangle's points
        m_craft[3].color = sf::Color::Green;
        m_craft[4].color = sf::Color::Blue;
        m_craft[5].color = sf::Color::Green;       

        setOrigin({100.0f, 200.0f});
        setScale({0.125f, 0.125f});
        setPosition(m_position);
        setRotation(m_heading);
    }
    
    void draw_craft_2()
    {
        // define the position of the triangle's points
        m_craft[0].position = sf::Vector2f(100.f, 240.f);
        m_craft[1].position = sf::Vector2f(0.f, 300.f);
        m_craft[2].position = sf::Vector2f(100.f, 0.f);
        
        // define the color of the triangle's points
        m_craft[0].color = sf::Color::Red;
        m_craft[1].color = sf::Color::Blue;
        m_craft[2].color = sf::Color::Red;       

        // define the position of the triangle's points
        m_craft[3].position = sf::Vector2f(100.f, 240.f);
        m_craft[4].position = sf::Vector2f(200.f, 300.f);
        m_craft[5].position = sf::Vector2f(100.f, 0.f);
        
        // define the color of the triangle's points
        m_craft[3].color = sf::Color::Red;
        m_craft[4].color = sf::Color::Blue;
        m_craft[5].color = sf::Color::Red;       

        setOrigin({100.0f, 200.0f});
        setScale({0.125f, 0.125f});
        setPosition(m_position);
        setRotation(m_heading);
    }
    
    void draw_unknown()
    {
        // define the position of the triangle's points
        m_craft[0].position = sf::Vector2f(100.f, 100.f);
        m_craft[1].position = sf::Vector2f(0.f, 200.f);
        m_craft[2].position = sf::Vector2f(100.f, 0.f);
        
        // define the color of the triangle's points
        m_craft[0].color = sf::Color::Yellow;
        m_craft[1].color = sf::Color::Yellow;
        m_craft[2].color = sf::Color::Yellow;       

        // define the position of the triangle's points
        m_craft[3].position = sf::Vector2f(100.f, 100.f);
        m_craft[4].position = sf::Vector2f(200.f, 200.f);
        m_craft[5].position = sf::Vector2f(100.f, 0.f);
        
        // define the color of the triangle's points
        m_craft[3].color = sf::Color::Yellow;
        m_craft[4].color = sf::Color::Yellow;
        m_craft[5].color = sf::Color::Yellow;       

        setOrigin({100.0f, 200.0f});
        setScale({0.125f, 0.125f});
        setPosition(m_position);
        setRotation(m_heading);
    }
    
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        // apply the transform
        states.transform *= getTransform();

        // our particles don't use a texture
        states.texture = nullptr;

        // draw the vertex array
        target.draw(m_craft, states);
    }

    int                   m_type;
    sf::VertexArray       m_craft;
    sf::Vector2f          m_position;
    sf::Angle             m_heading;
};