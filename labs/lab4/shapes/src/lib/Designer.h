#ifndef DESIGNER_H
#define DESIGNER_H

#include "lib/Shape.h"
#include "lib/ShapeFactory.h"

class IDesigner
{
public:
	virtual ~IDesigner() = default;
	virtual PictureDraft CreateDraft(std::istream& strm) = 0;
};

class Designer : public IDesigner
{
public:
	Designer(std::unique_ptr<IShapeFactory> factory)
		: m_factory(std::move(factory))
	{
	}

	PictureDraft CreateDraft(std::istream& strm) override
	{
		std::vector<std::unique_ptr<Shape>> shapes;
		std::string line;

		while (std::getline(strm, line))
		{
			if (!line.empty())
			{
				try
				{
					auto shape = m_factory->CreateShape(line);
					shapes.push_back(std::move(shape));
				}
				catch (const std::exception& e)
				{
					throw std::runtime_error("Failed to create shape: " + std::string(e.what()));
				}
			}
		}

		return PictureDraft(std::move(shapes));
	}

private:
	std::unique_ptr<IShapeFactory> m_factory;
};
#endif /* DESIGNER_H */
