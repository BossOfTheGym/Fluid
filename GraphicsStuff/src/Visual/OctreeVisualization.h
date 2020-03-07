#pragma once

#include <DataStructures/SVO.h>

#include <Render/Attributes.h>
#include <glWrap/Buffer/Buffer.h>
#include <glWrap/VertexArray/VertexArray.h>

#include <vector>


namespace vis
{
	using math::Vec3;
	using math::Float;

	struct Voxels
	{
		gl::VertexArray voxelArray{};
		gl::Buffer      voxelData{};

		Float voxelSize{};
	};

	template<class Value>
	Voxels fromSVO(const octree::SparseVoxelOctree<Value>& svo)
	{
		using Data = std::vector<Vec3>;

		Voxels voxels{};

		//buffer
		Data data;

		data.reserve(svo.size());
		for (auto& [hash, value] : svo)
		{
			data.push_back(svo.voxel(hash));
		}

		voxels.voxelData.createBuffer();
		voxels.voxelData.namedBufferData(sizeof(Vec3) * data.size(), data.data(), gl::BufferUsage::StaticDraw);
		
		//array
		voxels.voxelArray.genArray();
		voxels.voxelArray.bind();
		voxels.voxelArray.info() = gl::VertexArray::DrawInfo
		{
			  gl::DrawMode::Points
			, 0
			, static_cast<GLsizei>(data.size())
			, gl::IndicesType::None
			, nullptr
		};
		voxels.voxelArray.setAttribPointerInBuffer(
			  voxels.voxelData
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
		voxels.voxelArray.unbind();

		//size
		voxels.voxelSize = svo.spaceDelta().x;

		return voxels;
	}

	template<class Value, class Filter>
	Voxels fromFVO(const octree::FullVoxelOctree<Value>& fvo, Filter filter)
	{
		using Data = std::vector<Vec3>;
		using Hash = typename octree::FullVoxelOctree<Value>::Hash;

		Voxels voxels{};

		//buffer
		Data data;

		data.reserve(6LL * fvo.split() * fvo.split());
		for (Hash i = 0; i < fvo.size(); i++)
		{
			if (filter(fvo[i]))
			{
				data.push_back(fvo.voxel(i));
			}
		}

		voxels.voxelData.createBuffer();
		voxels.voxelData.namedBufferData(sizeof(Vec3) * data.size(), data.data(), gl::BufferUsage::StaticDraw);

		//array
		voxels.voxelArray.genArray();
		voxels.voxelArray.bind();
		voxels.voxelArray.info() = gl::VertexArray::DrawInfo
		{
			  gl::DrawMode::Points
			, 0
			, static_cast<GLsizei>(data.size())
			, gl::IndicesType::None
			, nullptr
		};
		voxels.voxelArray.setAttribPointerInBuffer(
			  voxels.voxelData
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
		voxels.voxelArray.unbind();

		//size
		voxels.voxelSize = fvo.spaceDelta().x;

		return voxels;
	}
}
