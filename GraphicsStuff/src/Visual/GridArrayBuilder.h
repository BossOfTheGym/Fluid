#pragma once

#include <Render/Attributes.h>
#include <glWrap/Buffer/Buffer.h>
#include <glWrap/VertexArray/VertexArray.h>
#include <Math/MathLib.h>

#include <vector>


namespace vis
{
	using math::Vec3;
	using math::Float;

	struct Cells
	{
		gl::VertexArray cellsArray{};
		gl::Buffer      cellsBuffer{};

		Vec3 cellSize{};
	};

	template<class Grid>
	Cells buildCellData(Grid&& grid)
	{
		using Data = std::vector<Vec3>;

		// buffer
		Data data;
		data.reserve(grid.size());
		for (auto& [hash, _] : grid)
		{
			data.push_back(grid.center(hash));
		}

		gl::Buffer cellsBuffer;
		cellsBuffer.createBuffer();
		cellsBuffer.namedBufferData(sizeof(Vec3) * data.size(), data.data(), gl::BufferUsage::StaticDraw);
			
		// array
		gl::VertexArray cellsArray;
		cellsArray.genArray();
		cellsArray.bind();
		cellsArray.info() = gl::VertexArray::DrawInfo
		{
			  gl::DrawMode::Points
			, 0
			, static_cast<GLsizei>(data.size())
			, gl::IndicesType::None
			, nullptr
		};
		cellsArray.setAttribPointerInBuffer(
			  cellsBuffer
			, gl::VertexArray::PointerInfo
			{
				  static_cast<GLuint>(VertexAttributes::Position)
				, gl::AttributeSize::Three
				, gl::DataType::Float 
				, gl::GLBool::True
				, 0
				, nullptr
			}
		);
		cellsArray.unbind();

		// size
		Vec3 cellSize = grid.getCellSize();

		return Cells{std::move(cellsArray), std::move(cellsBuffer), cellSize};
	}
}
