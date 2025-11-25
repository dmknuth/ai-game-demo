#include <random>

class Explosion : public sf::Drawable, public sf::Transformable
{
public:
    Explosion(unsigned int count) : m_particles(count), m_vertices(sf::PrimitiveType::Points, count)
    {
    }

    void set_position(sf::Vector2f position)
    {
        m_position = position;
    }
    
//    void reset_particles()
//    {
//        m_position = position;
//    }
    
    void update(sf::Time elapsed)
    {
        for (std::size_t i = 0; i < m_particles.size(); ++i)
        {
            // update the particle lifetime
            Particle& p = m_particles[i];
            p.lifetime -= elapsed;

            // if the particle is dead, respawn it
            if (p.lifetime <= sf::Time::Zero)
            {
                resetParticle(i);
                m_vertices[i].position = m_position;
            }

            // update the position of the corresponding vertex
            m_vertices[i].position += p.velocity * elapsed.asSeconds();

            // update the alpha (transparency) of the particle according to its lifetime
            float ratio           = p.lifetime.asSeconds() / m_lifetime.asSeconds();
            m_vertices[i].color.r = 250;
            m_vertices[i].color.g = 200;
            m_vertices[i].color.b = 31;
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
        const sf::Angle angle       = sf::degrees(std::uniform_real_distribution(0.0f, 360.0f)(rng));
        const float     speed       = std::uniform_real_distribution(50.f, 100.f)(rng);
        m_particles[index].velocity = sf::Vector2f(speed, angle);
        m_particles[index].lifetime = sf::milliseconds(std::uniform_int_distribution(500, 1500)(rng));

        // reset the position of the corresponding vertex
        m_vertices[index].position = m_position;
    }

    std::vector<Particle> m_particles;
    sf::VertexArray       m_vertices;
    sf::Time              m_lifetime{sf::seconds(1)};
    sf::Vector2f          m_position;
};