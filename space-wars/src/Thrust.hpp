#include <random>
#include <vector>

class Thrust : public sf::Drawable, public sf::Transformable
{
public:
    Thrust(unsigned int count, unsigned int type) : m_type(type), m_particles(count), m_vertices(sf::PrimitiveType::Points, count), m_fire(false)
    {
    }

    void set_position(sf::Vector2f position)
    {
        m_position = position;
    }
    
    void set_heading(float heading)
    {
        m_heading = heading + 180.0f;
    }

    void set_pose(sf::Vector2f position, float heading)
    {
        set_position(position);
        set_heading(heading);
    }
    
    void increment_heading(float rate = 1.0f)
    {
        m_heading += rate;
        if(m_heading > 360.0f)
            m_heading = m_heading - 360.0f;
    }

    void decrement_heading(float rate = 1.0f)
    {
        m_heading -= rate;
        if(m_heading < 0.0f)
            m_heading = m_heading + 360.0f;
    }

   void fire()
    {
        m_fire = true;
    }

    void coast()
    {
        m_fire = false;
    }

    void update(sf::Time elapsed)
    {
        for (std::size_t i = 0; i < m_particles.size(); ++i)
        {
            // update the particle lifetime
            Particle& p = m_particles[i];
            p.lifetime -= elapsed;

            // if the particle is dead, respawn it
            if ((p.lifetime <= sf::Time::Zero) && m_fire)
                resetParticle(i);

            // update the position of the corresponding vertex
            m_vertices[i].position += p.velocity * elapsed.asSeconds();

            // update the alpha (transparency) of the particle according to its lifetime
            float ratio           = p.lifetime.asSeconds() / m_lifetime.asSeconds();
            if(m_type == Constants::CRAFT_1)
            {
                m_vertices[i].color.r = 250;
                m_vertices[i].color.g = 200;
                m_vertices[i].color.b = 31;
            }
            else if(m_type == Constants::CRAFT_2)
            {
                m_vertices[i].color.r = 50;
                m_vertices[i].color.g = 200;
                m_vertices[i].color.b = 131;
            }
            m_vertices[i].color.a = static_cast<std::uint8_t>(ratio * 255);
        }
    }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        // apply the transform
        states.transform *= getTransform();

        // our particles don't use a texture
        states.texture = nullptr;

        // draw the vertex array
        target.draw(m_vertices, states);
    }

    struct Particle
    {
        sf::Vector2f velocity;
        sf::Time     lifetime;
    };

    void resetParticle(std::size_t index)
    {
        // create random number generator
        static std::random_device rd;
        static std::mt19937       rng(rd());

        // give a random velocity and lifetime to the particle
        const sf::Angle angle       = sf::degrees(std::uniform_real_distribution(m_heading - k_thrust_width, m_heading + k_thrust_width)(rng));
        const float     speed       = std::uniform_real_distribution(50.f, 100.f)(rng);
        m_particles[index].velocity = sf::Vector2f(speed, angle);
        m_particles[index].lifetime = sf::milliseconds(std::uniform_int_distribution(100, 300)(rng));

        // reset the position of the corresponding vertex
        m_vertices[index].position = m_position;
    }

    unsigned int          m_type;
    std::vector<Particle> m_particles;
    sf::VertexArray       m_vertices;
    sf::Time              m_lifetime{sf::seconds(1)};
    sf::Vector2f          m_position;
    float                 m_heading;
    const float           k_thrust_width = 15.0f;
    bool                  m_fire;
};