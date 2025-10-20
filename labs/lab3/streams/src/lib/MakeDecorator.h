#ifndef MAKEDECORATOR_H
#define MAKEDECORATOR_H

#include <memory>
#include <utility>

template <typename Decorator, typename... Args>
auto MakeDecorator(const Args&... args)
{
	return [&](auto&& object) {
		return std::make_unique<Decorator>(
			std::forward<decltype(object)>(object), args...);
	};
}

template <typename Component, typename Decorator>
auto operator<<(Component&& component, const Decorator& decorate)
{
	return decorate(std::forward<Component>(component));
}

#endif /* MAKEDECORATOR_H */
