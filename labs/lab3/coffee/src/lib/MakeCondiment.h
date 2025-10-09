#ifndef MAKECONDIMENT_H
#define MAKECONDIMENT_H

#include <memory>
#include <utility>

template <typename Condiment, typename... Args>
auto MakeCondiment(const Args&... args)
{
	return [=](auto&& b) {
		return std::make_unique<Condiment>(std::forward<decltype(b)>(b), args...);
	};
}

template <typename Component, typename Decorator>
auto operator<<(Component&& component, const Decorator& decorate)
{
	return decorate(std::forward<Component>(component));
}

#endif /* MAKECONDIMENT_H */
