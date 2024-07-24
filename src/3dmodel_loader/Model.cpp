//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------

#include "Model.h"
#include "ModelObj.h"
#include "NvMath.h"

using namespace MB;

struct NvModelFileHeader {
	uint8_t _magic[4];
	uint32_t _headerSize; // includes magic
	uint32_t _version;
	uint32_t _vertexCount;
	uint32_t _indexCount;
	uint32_t _vertexSize; // size of each vert IN BYTES!
	uint32_t _indexSize; // size of each index IN BYTES!
	int32_t _pOffset;
	int32_t _nOffset;
	int32_t _tcOffset;
	int32_t _sTanOffset;
	int32_t _cOffset;
	int32_t _posSize;
	int32_t _tcSize;
	int32_t _cSize;
	float _minExtent[3];
	float _maxExtent[3];
	// ptr + headersize = vertex base
	// ptr + headersize + vertexCount * vertexSize = index base
};


Model* Model::CreateFromObj(uint8_t* data, float scale, bool computeNormals, bool computeTangents)  {
	return ModelObj::CreateFromObj(data, scale, computeNormals, computeTangents);
}

Model* Model::CreateFromPreprocessed(uint8_t* data) {
	Model* model = new Model;
	if (!model->LoadPreprocessedModel(data))
		return NULL;

	return model;
}

//
//
////////////////////////////////////////////////////////////
Model::Model() : 
	_pOffset(-1), _nOffset(-1), _tcOffset(-1), _sTanOffset(-1), _cOffset(-1), 
	_posSize(0), _tcSize(0), _cSize(0),
	_vtxSize(0),
	_vertexCount(0), _indexCount(0),_vertices(NULL),_indices(NULL)
{
}

//
//
//////////////////////////////////////////////////////////////////////
Model::~Model() {
    //dynamic allocations presently all handled via stl
	if (_vertices != NULL) {
		delete[] _vertices;
	}

	if (_indices != NULL) {
		delete[] _indices;
	}
}

//
//
////////////////////////////////////////////////////////////
int32_t Model::getPositionSize() const {
	return _posSize;
}

//
//
////////////////////////////////////////////////////////////
int32_t Model::getNormalSize() const {
	return 3;
}

//
//
////////////////////////////////////////////////////////////
int32_t Model::getTexCoordSize() const {
	return _tcSize;
}

//
//
////////////////////////////////////////////////////////////
int32_t Model::getTangentSize() const {
	return 3;
}

//
//
////////////////////////////////////////////////////////////
int32_t Model::getColorSize() const {
	return _cSize;
}

bool Model::LoadPreprocessedModel(uint8_t* data) {
	if (!data)
		return false;

	NvModelFileHeader* hdr = (NvModelFileHeader*)data;

	if (hdr->_magic[0] != 'N' || hdr->_magic[1] != 'V' ||
		hdr->_magic[2] != 'M' || hdr->_magic[3] != 'B' ||
		hdr->_headerSize != sizeof(NvModelFileHeader) ||
		hdr->_version != 1 ||
		hdr->_indexSize != sizeof(uint32_t))
		return false;

	_vertexCount = hdr->_vertexCount;
	_indexCount = hdr->_indexCount;
	_vtxSize = hdr->_vertexSize / sizeof(float);
	_pOffset = hdr->_pOffset;
	_nOffset = hdr->_nOffset;
	_tcOffset = hdr->_tcOffset;
	_sTanOffset = hdr->_sTanOffset;
	_cOffset = hdr->_cOffset;
	_posSize = hdr->_posSize;
	_tcSize = hdr->_tcSize;
	_cSize = hdr->_cSize;

	for (int i = 0; i < 3; i++)
		m_minExtent[i] = hdr->_minExtent[i];
	for (int i = 0; i < 3; i++)
		m_maxExtent[i] = hdr->_maxExtent[i];

	_vertices = new float[_vertexCount * _vtxSize];
	_indices = new uint32_t[_indexCount];

	memcpy(_vertices, data + sizeof(NvModelFileHeader),
		_vertexCount * _vtxSize * sizeof(float));

	memcpy(_indices, data + sizeof(NvModelFileHeader) +
		_vertexCount * _vtxSize * sizeof(float),
		_indexCount * sizeof(uint32_t));

	return true;

}

Model* Model::CreateFromData(const float* verts, uint32_t vertCount, uint32_t vertSize,
                             const uint32_t* indices, uint32_t indexCount,
                             uint32_t posSize, int32_t posOffset,
                             int32_t normOffset,
                             uint32_t uvSize, int32_t uvOffset,
                             int32_t tanOffset,
                             uint32_t colorSize, int32_t colorOffset) {
	Model* model = new Model;
	if (!model->LoadFromData(verts, vertCount, vertSize,
		indices, indexCount,
		posSize, posOffset,
		normOffset,
		uvSize, uvOffset,
		tanOffset,
		colorSize, colorOffset))
		return NULL;

	return model;
}

bool Model::LoadFromData(const float* verts, uint32_t vertCount, uint32_t vertSize,
                         const uint32_t* indices, uint32_t indexCount,
                         uint32_t posSize, int32_t posOffset,
                         int32_t normOffset,
                         uint32_t uvSize, int32_t uvOffset,
                         int32_t tanOffset,
                         uint32_t colorSize, int32_t colorOffset) {

	_vertexCount = vertCount;
	_indexCount = indexCount;
	_vtxSize = vertSize;
	_pOffset = posOffset;
	_nOffset = normOffset;
	_tcOffset = uvOffset;
	_sTanOffset = tanOffset;
	_cOffset = colorOffset;
	_posSize = posSize;
	_tcSize = uvSize;
	_cSize = colorSize;

	// compute extents
	for (int i = 0; i < 3; i++) {
		m_minExtent[i] = FLT_MAX;
		m_maxExtent[i] = -FLT_MAX;
	}

	const float* pos = verts + posOffset;
	for (int i = 0; i < _vertexCount; i++) {
		float x = pos[0];
		float y = pos[1];
		float z = pos[2];

		for (int j = 0; j < 3; j++) {
			if (m_minExtent[j] > pos[j])
				m_minExtent[j] = pos[j];
			if (m_maxExtent[j] < pos[j])
				m_maxExtent[j] = pos[j];
		}

		pos += vertSize;
	}

	_vertices = new float[_vertexCount * _vtxSize];
	_indices = new uint32_t[_indexCount];

	memcpy(_vertices, verts,
		_vertexCount * _vtxSize * sizeof(float));

	memcpy(_indices, indices ,
		_indexCount * sizeof(uint32_t));

	/*vector<int> index_v;
	index_v.assign(indices, indices + _indexCount);
	std::sort(index_v.begin(), index_v.end());
	index_v.erase(std::unique(index_v.begin(), index_v.end()), index_v.end());

	vector<int>::iterator cur = index_v.begin();
	while ( cur != index_v.end() )
	{
		_positions.push_back(_vertices[(*cur) * 3]);
		_positions.push_back(_vertices[(*cur) *3 + 1]);
		_positions.push_back(_vertices[(*cur) *3 + 2]);
		++cur;
	}*/

	return true;
}

void Model::computeBoundingBox(MB::vec3f& minVal, MB::vec3f& maxVal)
{
	minVal = m_minExtent;
	maxVal = m_maxExtent;
}

vector<float>& Model::getPositions()
{
	return _positions;
}


bool Model::WritePreprocessedModel(const char* filename) {
#ifdef _WIN32
	FILE* fp = NULL;
	
	errno_t err = fopen_s(&fp, filename, "wb");
	if (err || !fp)
		return false;

	NvModelFileHeader hdr;
	hdr._magic[0] = 'N';
	hdr._magic[1] = 'V';
	hdr._magic[2] = 'M';
	hdr._magic[3] = 'B';
	hdr._headerSize = sizeof(NvModelFileHeader);
	hdr._version = 1;
	hdr._vertexCount = _vertexCount;
	hdr._indexCount = _indexCount;
	hdr._vertexSize = _vtxSize * sizeof(float);
	hdr._indexSize = sizeof(uint32_t);
	hdr._pOffset = _pOffset;
	hdr._nOffset = _nOffset;
	hdr._tcOffset = _tcOffset;
	hdr._sTanOffset = _sTanOffset;
	hdr._cOffset = _cOffset;
	hdr._posSize = _posSize;
	hdr._tcSize = _tcSize;
	hdr._cSize = _cSize;

	for (int i = 0; i < 3; i++) {
		hdr._minExtent[i] = m_minExtent[i];
		hdr._maxExtent[i] = m_maxExtent[i];
	}

	fwrite(&hdr, sizeof(NvModelFileHeader), 1, fp);

	fwrite(_vertices, 1, hdr._vertexSize * _vertexCount, fp);
	fwrite(_indices, sizeof(uint32_t), _indexCount, fp);
	
	fclose(fp);

	return true;
#else
	return false;
#endif
}

//
//
////////////////////////////////////////////////////////////
const float* Model::getCompiledVertices() const {
	return _vertices;
}

//
//
////////////////////////////////////////////////////////////
const uint32_t* Model::getCompiledIndices(NvModelPrimType::Enum& prim) const {
	prim = NvModelPrimType::TRIANGLES;
	return _indices;
}

//
//
////////////////////////////////////////////////////////////
int32_t Model::getCompiledPositionOffset() const {
	return _pOffset;
}

//
//
////////////////////////////////////////////////////////////
int32_t Model::getCompiledNormalOffset() const {
	return _nOffset;
}

//
//
////////////////////////////////////////////////////////////
int32_t Model::getCompiledTexCoordOffset() const {
	return _tcOffset;
}

//
//
////////////////////////////////////////////////////////////
int32_t Model::getCompiledTangentOffset() const {
	return _sTanOffset;
}

//
//
////////////////////////////////////////////////////////////
int32_t Model::getCompiledColorOffset() const {
	return _cOffset;
}

// returns the size of the merged vertex in # of floats
//
//
////////////////////////////////////////////////////////////
int32_t Model::getCompiledVertexSize() const {
	return _vtxSize;
}

//
//
////////////////////////////////////////////////////////////
int32_t Model::getCompiledVertexCount() const {
	return _vertexCount;
}

//
//
////////////////////////////////////////////////////////////
int32_t Model::getCompiledIndexCount(NvModelPrimType::Enum& prim) const {
	prim = NvModelPrimType::TRIANGLES;

	return _indexCount;
}
