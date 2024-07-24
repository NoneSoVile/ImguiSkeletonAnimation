#ifndef YF_MODEL_OBJ_H
#define YF_MODEL_OBJ_H

#include "Model.h"
#include <vector>
#include <map>
#include <string>
#include "NvMath.h"
using std::vector;
/// \file
/// GL-compatible geometric object loader and optimizer

/// Non-rendering geometry model.
/// Graphics-API-agnostic geometric model class, including model loading from
/// OBJ file data, optimization, bounding volumes and rescaling.  
/// Can compute addition items such as facet normals and tangents as needed.
class ModelObj : public Model {
public:

    /// Create a model.
    /// Creates an empty model
     static Model* CreateFromObjFile(const char* fileName, float scale, bool computeNormals, bool computeTangents, bool filpNormals = false);
	static Model* CreateFromObj(uint8_t* data, float scale, bool computeNormals, bool computeTangents, bool filpNormals = false);
	static Model* CreateFromPreprocessed(uint8_t* data);
    static Model* loadObjAsTotal(const char* fileName, float scale, bool computeNormals, bool computeTangents, bool filpNormals);
    bool   loadObjfromfile(FILE* fp);

	//added by leejb
	static Model* CreateFromObjWithPoints(uint8_t* data);

	virtual ~ModelObj() {}

    /// Load raw model from OBJ data.
    /// Loads a model from the given block of raw OBJ-file data
    /// \param[in] fileData a pointer to the in-memory representation of the OBJ file.
    /// This data is not cached locally and can be freed once the function returns
    /// \return true on success and false on failure
	bool loadFromFileData(uint8_t*fileData);

    /// Process a model into rendering-friendly form.
    /// This function takes the raw model data in the internal
    ///  structures, and attempts to bring it to a format directly
    ///  accepted for vertex array style rendering. This means that
    ///  a unique compiled vertex will exist for each unique
    ///  combination of position, normal, tex coords, etc that are
    ///  used in the model. The prim parameter, tells the model
    ///  what type of index list to compile. By default it compiles
    ///  a simple triangle mesh with no connectivity. 
    /// \param[in] prim the desired primitive type that will be used for rendering;
    /// the target of the compilation operation
    void compileModel( NvModelPrimType::Enum prim = NvModelPrimType::TRIANGLES);

    ///  Computes an AABB from the data.
    /// This function returns the points defining the axis-
    /// aligned bounding box containing the model.
    /// \param[out] minVal the returned minimum corner of the AABB
    /// \param[out] maxVal the returned maximum corner of the AABB
    void computeBoundingBox(MB::vec3f &minVal, MB::vec3f &maxVal);

    /// Rescale the model to the desired radius and centers it at the origin.
    /// Rescales object based on bounding box such that the new bounding box
    /// "radius" (half the diagonal of the bounding box) is the given target value and
    /// translates the vertices so that the bounding box is centered at the origin
    /// \param[in] radius the desired target radius of the geometry
    void rescale( float radius);

    /// Compute tangent vectors in the "S" UV direction.
    /// This function computes tangents in the s direction on
    /// the model. It operates on the raw data, so it should only
    /// be used before compiling a model into a HW friendly form.
    /// This can cause model expansion, since it can keep vertices
    /// from being shared.  Thus it should be used only when the results
    /// are required by the rendering method
    void computeTangents();

    /// Compute per-vertex normals.
    /// This function computes vertex normals for a model
    /// which did not have them. It computes them on the raw
    /// data, so it should be done before compiling the model
    /// into a HW friendly format.
	void computeNormals(bool filpNormals);

    /// Remove zero-area/length primitives.
    /// Removes primitives that will add nothing to the rendered result
    void removeDegeneratePrims();

	///@{
	/// Vertex data existence queries.
	/// \return true if the given vertex attribute exists in the primitive and false if not
	bool hasNormals() const { return _normals.size() > 0; }
	bool hasTexCoords() const { return _texCoords.size() > 0; }
	bool hasTangents() const { return _sTangents.size() > 0; }
	bool hasColors() const { return _colors.size() > 0; }
	///@}
    std::vector<float> getPositionsByObjectName(std::string& name);
	std::vector<float> getTexCoordsByObjectName(std::string& name);
	std::vector<float> getNormalsByObjectName(std::string& name);
    std::vector<int> getPositionIndexByObjectName(std::string& name);
    std::vector< vector<int> >& getFaceList();
    std::vector<std::string>& getSubObjectList();
    int getSubObjectCount();
    vector<Model*>& getSubObjectModelList();

    virtual vector<float>& getPositions();
    void createObjByName(std::string objName);
protected:
	ModelObj() { }

    //data structures for model data, not optimized for rendering



	std::map<std::string, std::vector<float> >   _positionsByObjectName;
	std::map<std::string, std::vector<float> >   _texCoordsByObjectName;
	std::map<std::string, std::vector<float> >   _normalsByObjectName;
    std::map<std::string, std::vector<int> >     _pIndexByObjectName;
    std::map<std::string, std::vector<int> >     _tIndexByObjectName;
    std::map<std::string, std::vector<int> >     _nIndexByObjectName;
    std::vector< vector<int> >_pIndexListInObject;  //group by object 
    std::vector< vector<int> >_tIndexListInObject;  //group by object 
    std::vector< vector<int> >_nIndexListInObject;  //group by object 
    std::vector< vector<float> >_positionsInObject;  //group by object 
    std::vector< vector<float> >_normalsInObject;  //group by object 
    std::vector< vector<float> >_texCoordsInObject;  //group by object 
    std::vector<std::string> _objectsList;
    std::string _currentObjectName;
    vector<Model*> _subObjectModelList;

    std::vector<uint32_t> _pIndex;
    std::vector<uint32_t> _nIndex;
    std::vector<uint32_t> _tIndex;
    std::vector<uint32_t> _tanIndex;
    std::vector<uint32_t> _cIndex;
};

#endif
