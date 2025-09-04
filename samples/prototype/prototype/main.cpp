#include <memory>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <unordered_map>

using namespace std;

class CShape
{
public:
	virtual unique_ptr<CShape> Clone()const = 0;
	virtual ~CShape() = default;
};

class CRectangle : public CShape
{
public:
	CRectangle(double w, double h)
		:m_width(w), m_height(h) {}
	double GetWidth()const
	{
		return m_width;
	}

	double GetHeight()const
	{
		return m_height;
	}

	unique_ptr<CShape> Clone() const override
	{
		return make_unique<CRectangle>(*this);
	}
private:
	double m_width, m_height;

};

class CCircle : public CShape
{
public:
	CCircle(double r) :m_radius(r) {}
	double GetRadius()const
	{
		return m_radius;
	}

	unique_ptr<CShape> Clone() const override
	{
		return make_unique<CCircle>(*this);
	}
private:
	double m_radius;
};

typedef vector<shared_ptr<CShape>> Shapes;

class CShapeGroup : public CShape
{
public:
	CShapeGroup() = default;
	CShapeGroup(const CShapeGroup & group)
	{
		for (auto & shape : group.m_shapes)
		{
			m_shapes.push_back(shape->Clone());
		}
	}

	void AddShape(unique_ptr<CShape> && shape)
	{
		m_shapes.push_back(move(shape));
	}

	size_t GetShapeCount()const
	{
		return m_shapes.size();
	}

	shared_ptr<CShape> GetShape(size_t index)const
	{
		return m_shapes.at(index);
	}
	unique_ptr<CShape> Clone() const override
	{
		return make_unique<CShapeGroup>(*this);
	}
private:
	Shapes m_shapes;
};

class CShapeRegistry
{
public:
	void RegisterShapePrototype(const string& id, const CShape& prototype)
	{
		m_prototypes.emplace(id, prototype.Clone());
	}
	unique_ptr<CShape> CreateShape(const string& id)
	{
		return m_prototypes.at(id)->Clone();
	}
private:
	unordered_map<string, unique_ptr<CShape>> m_prototypes;
};

template <typename Base, typename CloneInterface, typename Impl>
class CPrototypeImpl : public Base
{
public:
	unique_ptr<CloneInterface> Clone()const override
	{
		return make_unique<Impl>(*static_cast<Impl const*>(this));
	}
};

class CPoint : public CPrototypeImpl<CShape, CShape, CPoint>
{
public:
	CPoint() = default;
	CPoint(double x, double y)
		: m_x(x), m_y(y)
	{
	}
private:
	double m_x = 0.0;
	double m_y = 0.0;
};

int main()
{
	CShapeRegistry registry;
	registry.RegisterShapePrototype("circle", CCircle(42));
	registry.RegisterShapePrototype("rectangle", CRectangle(40, 30));
	registry.RegisterShapePrototype("point", CPoint(40, 30));
	CShapeGroup group;
	group.AddShape(make_unique<CCircle>(10));
	group.AddShape(make_unique<CRectangle>(32, 32));
	registry.RegisterShapePrototype("group", group);

	auto someShape = registry.CreateShape("group");
	return 0;
}
