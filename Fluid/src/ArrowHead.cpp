#include "ArrowHead.h"

#include <cmath>


ArrowHeadBuilder::ArrowHeadBuilder(  uint32_t split
	, Float lengthFront
	, Float lengthBack
	, Float widthMiddle
	, Float widthBack
	, Float heightBack
	, Float sigma
) 
	: m_split(split)
	, m_lengthFront(lengthFront)
	, m_lengthBack(lengthBack)
	, m_widthMiddle(widthMiddle)
	, m_widthBack(widthBack)
	, m_heightBack(heightBack)
	, m_sigma(sigma)
{}

VertexArray ArrowHeadBuilder::buildShape()
{
	const auto dx = m_widthBack / m_split;
	const auto x0 = -m_widthBack / 2;
	const auto xn = +m_widthBack / 2;

	const auto h = m_heightBack;
	const auto s = m_sigma;

	//compute points of the back profile
	std::vector<Vec3> backProfile;

	backProfile.push_back(Vec3(x0, 0.0f, 0.0f));
	for (int32_t i = 1; i < m_split; i++)
	{
		const auto x = x0 + i * dx;

		backProfile.push_back(Vec3(x, h * std::exp(-x * x / s), 0.0f));
	}
	backProfile.push_back(Vec3(xn, 0.0f, 0.0f));

	//compute points of the middle profile
	const auto coef = m_widthMiddle / m_widthBack;

	std::vector<Vec3>  middleProfile(backProfile);
	for (auto& point : middleProfile)
	{
		point *= coef;
		point.z = m_lengthBack;
	}

	//initialize mesh data
	std::vector<Vec3> meshData;

	//construct back part
	for (int32_t i = 0; i < static_cast<int32_t>(backProfile.size()) - 1; i++)
	{
		auto& b1 = backProfile[i];
		auto& b2 = backProfile[i + 1];

		meshData.push_back(b1);
		meshData.push_back(b2);
		meshData.push_back(Vec3(0.0f));
	}

	//construct bottom part
	meshData.push_back(Vec3(-m_widthMiddle / 2, 0.0f, m_lengthBack));
	meshData.push_back(Vec3(+m_widthMiddle / 2, 0.0f, m_lengthBack));
	meshData.push_back(Vec3(0.0f, 0.0f, m_lengthBack + m_lengthFront));

	meshData.push_back(Vec3(-m_widthMiddle / 2, 0.0f, m_lengthBack));
	meshData.push_back(Vec3(-m_widthBack / 2, 0.0f, 0.0));
	meshData.push_back(Vec3(+m_widthBack / 2, 0.0f, 0.0));

	meshData.push_back(Vec3(-m_widthMiddle / 2, 0.0f, m_lengthBack));
	meshData.push_back(Vec3(+m_widthMiddle / 2, 0.0f, m_lengthBack));
	meshData.push_back(Vec3(+m_widthBack / 2, 0.0f, 0.0));

	//construct top part
	for (int32_t i = 0; i < static_cast<int32_t>(backProfile.size()) - 1; i++)
	{
		auto& b1 = backProfile[i];
		auto& b2 = backProfile[i + 1];
		auto& m1 = middleProfile[i];
		auto& m2 = middleProfile[i + 1];

		meshData.push_back(b1);
		meshData.push_back(b2);
		meshData.push_back(m1);

		meshData.push_back(m1);
		meshData.push_back(m2);
		meshData.push_back(b2);
	}

	for (int32_t i = 0; i < static_cast<int32_t>(middleProfile.size()) - 1; i++)
	{
		auto& m1 = middleProfile[i];
		auto& m2 = middleProfile[i + 1];

		meshData.push_back(m1);
		meshData.push_back(m2);
		meshData.push_back(Vec3(0.0, 0.0, m_lengthBack + m_lengthFront));
	}

	//normalize data
	Float xMin{}, xMax{};
	Float yMin{}, yMax{};
	Float zMin{}, zMax{};
	for (auto& point : meshData)
	{
		xMin = std::min(point.x, xMin);
		xMax = std::max(point.x, xMax);
		yMin = std::min(point.y, xMin);
		yMax = std::max(point.y, xMax);
		zMin = std::min(point.z, xMin);
		zMax = std::max(point.z, xMax);
	}

	Float delta = std::max(std::max(xMax - xMin, yMax - yMin), zMax - zMin) / 2;
	for (auto& point : meshData)
	{
		point.x = (point.x - (xMin + xMax) / 2) / delta;
		point.y = (point.y - (yMin + yMax) / 2) / delta;
		point.z = (point.z - (zMin + zMax) / 2) / delta;
	}

	//construst vertex array
	Buffer vertices{};
	vertices.createBuffer();
	vertices.namedBufferData(sizeof(Vec3) * meshData.size(), meshData.data(), BufferUsage::StaticDraw);

	VertexArray result;
	result.info() = VertexArray::DrawInfo
	{
		  DrawMode::Triangles
		, 0
		, static_cast<GLsizei>(meshData.size())
	};
	result.createArray();
	result.bind();
	result.setAttribPointerInBuffer(
		vertices
		, VertexArray::PointerInfo
		{
			  static_cast<GLuint>(VertexAttributes::Position),
			  AttributeSize::Three,
			  DataType::Float,
			  GLBool::False,
			  0,
			  nullptr
		}
	);
	result.unbind();

	return result;
}