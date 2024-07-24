#ifndef YF_GLMODEL_H_
#define YF_GLMODEL_H_
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "NvMath.h"

/// \file
/// OpenGL geometric model handing and rendering

class Model;

/// Drawable geometric model using GL VBOs.  Supports loading from OBJ file data.
/// Contains positions as well as optional normals, UVs, and tangent vectors
class ModelGL 
{
public:
	~ModelGL();

	/// Create a renderable model from OBJ data
	/// \param[in] data pointer to OBJ file data
	/// \param[in] scale the target radius to which we want the model scaled, or <0 if no scaling should be done
	/// \param[in] computeNormals indicate whether per-vertex normals should be estimated and added
	/// \param[in] computeTangents indicate whether per-vertex tangent vectors should be estimated and added
	/// \return a pointer to the new model
	static ModelGL *
    CreateFromObj(uint8_t *data, float scale, bool computeNormals, bool computeTangents,
                  bool initBuffers = false, bool flipNormals = false);

	/// Create a renderable model from a preprocessed "NVM" file, which is much faster and more efficient to load than OBJ
	/// \param[in] data pointer to NVM file data
	/// \return a pointer to the new model
	static ModelGL* CreateFromPreprocessed(uint8_t* data);

	/// Create a renderable model from an unoptimized NvModel object
	/// \param[in] model the unoptimized model to be converted
	/// \return a pointer to the new model
	static ModelGL* CreateFromModel(Model* model);

	//ADDED BY LEEJB
    static ModelGL* CreateFromObjWithPoints(uint8_t* data);
    void drawPoints(GLint positionHandle);

    /// Draw the model using the current shader (positions)
    /// Binds the vertex position array to the given attribute array index and draws the
    /// model with the currently bound shader.
    /// \param[in] positionHandle the vertex attribute array index that represents position in the current shader
    void drawElements(GLint positionHandle);

    /// Draw the model using the current shader (positions and normals)
    /// Binds the vertex position and normal arrays to the given attribute array indices and draws the
    /// model with the currently bound shader.
    /// \param[in] positionHandle the vertex attribute array index that represents position in the current shader
    /// \param[in] normalHandle the vertex attribute array index that represents normals in the current shader
    void drawElements(GLint positionHandle, GLint normalHandle);


	void drawElementsWithTexCoord(GLint positionHandle, GLint texHandle);

    /// Draw the model using the current shader (positions, UVs and normals)
    /// Binds the vertex position, UV and normal arrays to the given attribute array indices and draws the
    /// model with the currently bound shader.
    /// \param[in] positionHandle the vertex attribute array index that represents position in the current shader
    /// \param[in] normalHandle the vertex attribute array index that represents normals in the current shader
    /// \param[in] texcoordHandle the vertex attribute array index that represents UVs in the current shader
    void drawElements(GLint positionHandle, GLint normalHandle, GLint texcoordHandle, bool usePoints = false, bool useLines = false);

    /// Draw the model using the current shader (positions, UVs, normals and tangents)
    /// Binds the vertex position, UV, normal and tangent arrays to the given attribute array indices and draws the
    /// model with the currently bound shader.
    /// \param[in] positionHandle the vertex attribute array index that represents position in the current shader
    /// \param[in] normalHandle the vertex attribute array index that represents normals in the current shader
    /// \param[in] texcoordHandle the vertex attribute array index that represents UVs in the current shader
    /// \param[in] tangentHandle the vertex attribute array index that represents tangents in the current shader
    void drawElements(GLint positionHandle, GLint normalHandle, GLint texcoordHandle, GLint tangentHandle);

    /// Get the low-level geometry data.
    /// Returns the underlying geometry model data instance
    /// \return a pointer to the #NvModel instance that holds the client-memory data
    Model *getModel();

    /// Updates the center point of the model
    void computeCenter();

    MB::vec3f m_center; ///< The computed center of the bounding box of the app

    MB::vec3f GetMinExt()
    {
        return m_minExtent;
    }

    MB::vec3f GetMaxExt()
    {
        return m_maxExtent;
    }

    void bindBuffers();
    void unbindBuffers();
	void initBuffers();
    void rebindModel(Model* model);

public:
	ModelGL();


	Model *model;
	MB::vec3f m_minExtent, m_maxExtent, m_radius;	
	GLuint vao, model_vboID, model_iboID;
	uint32_t mIndexCount;
	bool mBufferInited;
	bool mBufferInvalidate;
};


#endif 
