#ifndef YF_MODEL_H
#define YF_MODEL_H


#include <vector>
#include "NvMath.h"
using std::vector;

/// \file
/// 3D API-compatible geometric object loader and optimizer

/// Primitive type mask.
struct NvModelPrimType {
    NvModelPrimType() {}
    enum Enum {
        NONE = 0x0, ///< Not set
        POINTS = 0x1, ///< single-vertex points
        EDGES = 0x2, ///< two vertex edges
        TRIANGLES = 0x4, ///< three-vertex triangles
        TRIANGLES_WITH_ADJACENCY = 0x8, ///< triangles with adjancency info
        ALL = 0xf ///< mask of all values
    };
};

/// Non-rendering geometry model.
/// Graphics-API-agnostic geometric model class, including model loading from
/// OBJ file data, optimization, bounding volumes and rescaling.  
/// Can compute addition items such as facet normals and tangents as needed.
class Model {
protected:
    Model();

public:

    /// Create a model from OBJ data
	/// \param[in] data pointer to OBJ file data
	/// \param[in] scale the target radius to which we want the model scaled, or <0 if no scaling should be done
	/// \param[in] computeNormals indicate whether per-vertex normals should be estimated and added
	/// \param[in] computeTangents indicate whether per-vertex tangent vectors should be estimated and added
	/// \return a pointer to the new model
	static Model* CreateFromObj(uint8_t* data, float scale, bool computeNormals = false, bool computeTangents = false);

	/// Create a model from a preprocessed "NVM" file, which is much faster and more efficient to load than OBJ
	/// \param[in] data pointer to NVM file data
	/// \return a pointer to the new model
	static Model* CreateFromPreprocessed(uint8_t* data);

	static Model* CreateFromData(const float* verts, uint32_t vertCount, uint32_t vertSize,
                                 const uint32_t* indices, uint32_t indexCount,
                                 uint32_t posSize, int32_t posOffset,
                                 int32_t normOffset = -1,
                                 uint32_t uvSize = 0, int32_t uvOffset = -1,
                                 int32_t tanOffset = -1,
                                 uint32_t colorSize = 0, int32_t colorOffset = -1);

    virtual ~Model();

    ///@{
    /// Vertex data size queries.
    /// \return the number of elements per vertex for the given attribute (e.g. xyz positions are 3)
    int32_t getPositionSize() const;
    int32_t getNormalSize() const;
    int32_t getTexCoordSize() const;
    int32_t getTangentSize() const;
    int32_t getColorSize() const;
    ///@}

    /// Get the array of compiled vertices.
    /// The array of the optimized, compiled vertices for rendering
    /// \return the pointer to the start of the first vertex
    const float* getCompiledVertices() const;

    /// Get the array of compiled indices for the given prim type.
    /// Computes and returns the array of indices with the cached primitive type
    /// \param[out] prim the primitive type for which indices have been generated
    /// \return pointer to the array of indices
    const uint32_t* getCompiledIndices( NvModelPrimType::Enum& prim) const;

    ///@{
    /// Get the offset within the vertex of each attrib.
    /// \return the offset (in number of floats) of each attrib from the base of the vertex
    int32_t getCompiledPositionOffset() const;
    int32_t getCompiledNormalOffset() const;
    int32_t getCompiledTexCoordOffset() const;
    int32_t getCompiledTangentOffset() const;
    int32_t getCompiledColorOffset() const;
    ///@}

    /// Get the size of a compiled vertex.
    /// \return the size of the merged vertex (in number of floats)
    int32_t getCompiledVertexSize() const;

    /// Get the count of vertices in the compiled array.
    /// \return the vertex count in the compiled (renderable) array
    int32_t getCompiledVertexCount() const;

    /// The rendering index count.
    /// \param[out] prim the primitive type of the array whose length was returned
    /// \return the number of indices in the given array
    int32_t getCompiledIndexCount( NvModelPrimType::Enum& prim) const;

	void getBoundingBox(MB::vec3f &minVal, MB::vec3f &maxVal) {
		minVal = m_minExtent;
		maxVal = m_maxExtent;
	}

	/// Write a loaded model to NVM format (Windows Only!)
	/// \param[in] filename the name/path to the file to be created
	/// \return true on success and false on failure
	bool WritePreprocessedModel(const char* filename);

	/// Load a preprocessed NVM model from preloaded data
	/// \param[in] data pointer to the model data
	/// \return true on success and false on failure
	bool LoadPreprocessedModel(uint8_t* data);

	/// Load model data into a model object (assumed to be triangle list)
	/// \param[in] verts pointer to an array of floats containing all vertex data
	/// \param[in] vertCount vertex count
	/// \param[in] vertSize size of vertex in floats
	/// \param[in] indices pointer to an array of 32-bit indices
	/// \param[in] indexCount index count
	/// \param[in] posSize size of each vertex position in floats
	/// \param[in] posOffset offset of the positions in floats within a vertex
	/// \param[in] normOffset offset of the normals in floats within a vertex (normals are assumed to be 3-vec)
	/// \param[in] uvSize size of each vertex UV in floats
	/// \param[in] uvOffset offset of the UVs in floats within a vertex
	/// \param[in] tanOffset offset of the tangents in floats within a vertex (tangents are assumed to be 3-vec)
	/// \param[in] colorSize size of each vertex color in floats
	/// \param[in] colorOffset offset of the colors in floats within a vertex
	/// \return true on success and false on failure
	bool LoadFromData(const float* verts, uint32_t vertCount, uint32_t vertSize,
		const uint32_t* indices, uint32_t indexCount,
		uint32_t posSize, int32_t posOffset,
		int32_t normOffset = -1,
		uint32_t uvSize = 0, int32_t uvOffset = -1,
		int32_t tanOffset = -1,
		uint32_t colorSize = 0, int32_t colorOffset = -1);

    virtual void computeBoundingBox(MB::vec3f& minVal, MB::vec3f& maxVal);

    virtual vector<float>& getPositions();

public:
    //data structures optimized for rendering, compiled model
    uint32_t* _indices;
    float* _vertices;
    int32_t _pOffset;
    int32_t _nOffset;
    int32_t _tcOffset;
    int32_t _sTanOffset;
    int32_t _cOffset;
    int32_t _posSize;
    int32_t _tcSize;
    int32_t _cSize;
    int32_t _vtxSize;

	int32_t _vertexCount;
	int32_t _indexCount;
	MB::vec3f m_minExtent;
	MB::vec3f m_maxExtent;

    vector<float> _positions;
    std::vector<float> _normals;
    std::vector<float> _texCoords;
    std::vector<float> _sTangents;
    std::vector<float> _colors;


};

#endif
