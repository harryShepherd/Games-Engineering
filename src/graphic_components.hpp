class ShapeComponent : public Component {
public:
    ShapeComponent() = delete;
    explicit ShapeComponent(Entity* const p);

    void update(const float& dt) override;
    void render() override;

    sf::Shape& get_shape() const;

    template <typename T, typename... Targs>
    void set_shape(Targs... params) {
        _shape = std::make_shared<T>(params...);
    }
protected:
    std::shared_ptr<sf::Shape> _shape;
};