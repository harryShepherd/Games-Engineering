#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>

class LevelSystem
{
public:
    enum Tile
    {
        EMPTY,
        START,
        END,
        WALL,
        ENEMY,
        WAYPOINT
    };

    static void load_level(const std::string &file_path, float tile_size);
    static void render(sf::RenderWindow &win);
    static sf::Color get_color(Tile t);
    static void set_color(Tile t, sf::Color c);
    static Tile get_tile(sf::Vector2i pos);
    static sf::Vector2f get_tile_pos(sf::Vector2i pos);
    static Tile get_tile_at(sf::Vector2f pos);
    static int get_height();
    static int get_width();
    static sf::Vector2f get_start_pos();
    static std::vector<sf::Vector2i> find_tiles(Tile t);
    static std::vector<sf::Vector2i> get_tiles_list(Tile type);
    static std::vector<std::vector<sf::Vector2i>> get_groups(Tile type);
    static bool in_group(const sf::Vector2i &pos, const std::vector<sf::Vector2i> &group);

protected:
    static std::unique_ptr<Tile[]> m_tiles;
    static int m_width;
    static int m_height;
    static sf::Vector2f m_offset;
    static float m_tile_size;
    static std::map<Tile, sf::Color> m_colors;
    static sf::Vector2f m_start_position;
    static std::vector<std::unique_ptr<sf::RectangleShape>> m_sprites;
    static void build_sprites();
    static void m_get_group(Tile type, const sf::Vector2i &pos, const std::vector<sf::Vector2i> &tile_list, std::vector<sf::Vector2i> &group, bool vert);
};