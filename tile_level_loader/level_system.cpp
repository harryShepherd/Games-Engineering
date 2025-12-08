#include "level_system.hpp"
#include <fstream>
#include <iostream>

std::unique_ptr<LevelSystem::Tile[]> LevelSystem::m_tiles;
int LevelSystem::m_width;
int LevelSystem::m_height;
sf::Vector2f LevelSystem::m_offset(0.0f, 0.0f);
sf::Vector2f LevelSystem::m_start_position;

float LevelSystem::m_tile_size(0.0f);
std::vector<std::unique_ptr<sf::RectangleShape>> LevelSystem::m_sprites;

std::map<LevelSystem::Tile, sf::Color> LevelSystem::m_colors{
    {WALL, sf::Color::White},
    {END, sf::Color::Red}
};

int LevelSystem::get_height() { return m_height; }
int LevelSystem::get_width() { return m_width; }

sf::Color LevelSystem::get_color(LevelSystem::Tile t)
{
    auto tileColor = m_colors.find(t);
    if(tileColor == m_colors.end())
    {
        m_colors[t] = sf::Color::Transparent;
    }

    return m_colors[t];
}

void LevelSystem::set_color(LevelSystem::Tile t, sf::Color c)
{
    m_colors[t] = c;
}

void LevelSystem::load_level(const std::string &path, float tile_size)
{
    m_tile_size = tile_size;
    int w = 0, h = 0;
    std::string buffer;

    // Load file to buffer
    std::ifstream file(path);
    if(file.good())
    {
        file.seekg(0, std::ios::end);
        buffer.resize(file.tellg());
        file.seekg(0);
        file.read(&buffer[0], buffer.size());
        file.close();
    }
    else
    {
        throw std::string("Couldn't open level file: " + path);
    }

    int x = 0;

    std::vector<Tile> temp_tiles;
    for (int i = 0; i < buffer.size(); ++i)
    {
        const char c = buffer[i];

        switch(c)
        {
            case 'w':
                temp_tiles.push_back(WALL);
                break;
            case 's':
                temp_tiles.push_back(START);
                m_start_position = get_tile_pos({x, h});
                break;
            case 'e':
                temp_tiles.push_back(END);
                break;
            case ' ':
                temp_tiles.push_back(EMPTY);
                break;
            case '+':
                temp_tiles.push_back(WAYPOINT);
                break;
            case 'n':
                temp_tiles.push_back(ENEMY);
                break;
            case '\n':
                if (w == 0) { w = i; } // if width isnt set yet
                x = 0;
                h++;
                break;
            default:
                return;
        }
        x++;
    }

    if (temp_tiles.size() != (w * h))
    {
        throw std::string("Mismatch in level size: " + path);
    }

    m_tiles = std::make_unique<Tile[]>(w * h);
    m_width = w;
    m_height = h;

    std::copy(temp_tiles.begin(), temp_tiles.end(), &m_tiles[0]);
    build_sprites();
}

void LevelSystem::build_sprites()
{
    m_sprites.clear();

    for (int y = 0; y < LevelSystem::get_height(); ++y)
    {
        for (int x = 0; x < LevelSystem::get_width(); ++x)
        {
            std::unique_ptr<sf::RectangleShape> sprite = std::make_unique<sf::RectangleShape>();
            
            sprite->setPosition(get_tile_pos({x, y}));
            sprite->setSize(sf::Vector2f(m_tile_size, m_tile_size));
            sprite->setFillColor(get_color(get_tile({x, y})));
            
            m_sprites.push_back(std::move(sprite));
        }
    }
}

sf::Vector2f LevelSystem::get_tile_pos(sf::Vector2i pos)
{
    return (sf::Vector2f(pos.x, pos.y) * m_tile_size);
}

LevelSystem::Tile LevelSystem::get_tile(sf::Vector2i pos)
{
    if ((pos.x >= m_width || pos.y >= m_height) || (pos.x < 0 || pos.y < 0))
    {
        return EMPTY;
    }

    Tile tile = m_tiles[(pos.y * m_width) + pos.x];
    return tile;
}

LevelSystem::Tile LevelSystem::get_tile_at(sf::Vector2f v)
{
    auto a = v - m_offset;
    if (a.x < 0 || a.y < 0)
    {
        return EMPTY;
    }

    return get_tile(sf::Vector2i((v - m_offset) / m_tile_size));
}

void LevelSystem::render(sf::RenderWindow &window)
{
    for (int i = 0; i < m_width * m_height; ++i)
    {
        window.draw(*m_sprites[i]);
    }
}

sf::Vector2f LevelSystem::get_start_pos() { return m_start_position; }

std::vector<sf::Vector2i> LevelSystem::find_tiles(LevelSystem::Tile type)
{
    auto v = std::vector<sf::Vector2i>();
    for (int i = 0; i < m_width * m_height; ++i)
    {
        if (m_tiles[i] == type)
        {
            v.push_back({i % m_width, i / m_width});
        }
    }

    return v;
}

std::vector<sf::Vector2i> LevelSystem::get_tiles_list(Tile type)
{
    std::vector<sf::Vector2i> list;

    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            Tile t = get_tile({x, y});
            if (t != type)
            {
                continue;
            }
            list.push_back({x, y});
        }
    }
    return list;
}

std::vector<std::vector<sf::Vector2i>> LevelSystem::get_groups(Tile type)
{
    std::vector<std::vector<sf::Vector2i>> groups;
    std::vector<sf::Vector2i> tile_list = find_tiles(type);

    while (!tile_list.empty())
    {
        std::vector<sf::Vector2i> group;

        if (tile_list.size() == 1)
        {
            group.push_back(tile_list[0]);
        }
        else
        {
            m_get_group(type, tile_list.front(), tile_list, group, true);
        }

        groups.push_back(group);

        for (sf::Vector2i pos : group)
        {
            int i = 0;
            for (; i < tile_list.size(); ++i)
            {
                if (tile_list[i] == pos) { break; }
            }
            if (i < tile_list.size()) {
                tile_list.erase(tile_list.begin() + i);
            }
        }
    }
    return groups;
}

void LevelSystem::m_get_group(Tile type, const sf::Vector2i &pos, const std::vector<sf::Vector2i> &tile_list, std::vector<sf::Vector2i> &group, bool vert)
{
    if (in_group(pos, group)) { return; }

    group.push_back(pos);

    std::vector<Tile> neighbours = 
    {
        in_group({pos.x-1,pos.y-1},tile_list) ? get_tile({pos.x-1,pos.y-1}) : EMPTY,
        in_group({pos.x,pos.y-1},tile_list) ? get_tile({pos.x,pos.y-1}) : EMPTY,
        in_group({pos.x+1,pos.y-1},tile_list) ? get_tile({pos.x+1,pos.y-1}) : EMPTY,
        in_group({pos.x+1,pos.y},tile_list) ? get_tile({pos.x+1,pos.y}) : EMPTY,
        in_group({pos.x+1,pos.y+1},tile_list) ? get_tile({pos.x+1,pos.y+1}) : EMPTY,
        in_group({pos.x,pos.y+1},tile_list) ? get_tile({pos.x,pos.y+1}) : EMPTY,
        in_group({pos.x-1,pos.y+1},tile_list) ? get_tile({pos.x-1,pos.y+1}) : EMPTY,
        in_group({pos.x-1,pos.y},tile_list) ? get_tile({pos.x-1,pos.y}) : EMPTY
    };

    if (neighbours[3] == type)
    {
        // look right
        m_get_group(type, {pos.x + 1, pos.y}, tile_list, group,
            neighbours[3] == neighbours[4] &&
            neighbours[7] == neighbours[6] && 
            neighbours[3] == neighbours[2] && 
            neighbours[7] == neighbours[0]
        );
    }

    if (neighbours[7] == type)
    {
        // look left
        m_get_group(type, {pos.x - 1, pos.y}, tile_list, group,
            neighbours[3] == neighbours[4] &&
            neighbours[7] == neighbours[6] && 
            neighbours[3] == neighbours[2] && 
            neighbours[7] == neighbours[0]
        );
    }
    if (!vert) { return; }

    if(neighbours[3] == neighbours[4] && neighbours[7] == neighbours[6] && neighbours[5] == type)
    {
        //look down
        m_get_group(type,{pos.x,pos.y+1},tile_list,group,true);
    }

    if(neighbours[3] == neighbours[2] && neighbours[7] == neighbours[0] && neighbours[1] == type)
    {
        //look up
        m_get_group(type,{pos.x,pos.y-1},tile_list,group,true);
    }
}

bool LevelSystem::in_group(const sf::Vector2i &pos, const std::vector<sf::Vector2i> &group)
{
    for (const sf::Vector2i &p : group)
    {
        if (p == pos) { return true; }
        return false;
    }
    return false;
}