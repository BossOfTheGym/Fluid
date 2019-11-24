#ifndef OCTREE_VISUALIZATION_H
#define OCTREE_VISUALIZATION_H

#include "SVO.h"

#include <Render/Attributes.h>
#include <glWrap/VertexArray/VertexArray.h>

#include <vector>


namespace vis
{
	struct Voxels
	{
		VertexArray voxelArray{};
		Buffer      voxelData{};

		float voxelSize{};
	};

	template<class Value>
	Voxels fromSVO(const SparseVoxelOctree<Value>& svo)
	{
		using Data = std::vector<Vec3>;

		Voxels voxels{};

		//buffer
		Data data;

		data.reserve(svo.size());
		for (auto&[hash, value] : svo)
		{
			data.push_back(svo.voxel(hash));
		}

		voxels.voxelData.createBuffer();
		voxels.voxelData.namedBufferData(sizeof(Vec3) * data.size(), data.data(), BufferUsage::StaticDraw);
		
		//array
		voxels.voxelArray.genArray();
		voxels.voxelArray.bind();
		voxels.voxelArray.info() = VertexArray::DrawInfo
		{
			  DrawMode::Points
			, 0
			, static_cast<GLsizei>(svo.size())
			, IndicesType::None
			, nullptr
		};
		voxels.voxelArray.setAttribPointerInBuffer(
			  voxels.voxelData
			, VertexArray::PointerInfo
			{
				  static_cast<GLuint>(VertexAttributes::Position)
				, AttributeSize::Three
			    , DataType::Float 
			    , GLBool::True
			    , 0
			    , nullptr
			}
		);
		voxels.voxelArray.unbind();

		//size
		voxels.voxelSize = svo.spaceDelta().x;

		return voxels;
	}

	// TODO : rework, should take criteria except value
	template<class Value, class Filter>
	Voxels fromFVO(const FullVoxelOctree<Value>& fvo, Filter filter)
	{
		using Data = std::vector<Vec3>;
		using Hash = typename FullVoxelOctree<Value>::Hash;

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
		voxels.voxelData.namedBufferData(sizeof(Vec3) * data.size(), data.data(), BufferUsage::StaticDraw);

		//array
		voxels.voxelArray.genArray();
		voxels.voxelArray.bind();
		voxels.voxelArray.info() = VertexArray::DrawInfo
		{
			  DrawMode::Points
			, 0
			, static_cast<GLsizei>(data.size())
			, IndicesType::None
			, nullptr
		};
		voxels.voxelArray.setAttribPointerInBuffer(
			  voxels.voxelData
			, VertexArray::PointerInfo
			{
				static_cast<GLuint>(VertexAttributes::Position)
				, AttributeSize::Three
			    , DataType::Float 
			    , GLBool::True
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


#endif