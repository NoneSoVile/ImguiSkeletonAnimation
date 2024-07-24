#include "ModelGL.h"
#include "Model.h"
#include "ModelObj.h"
#include "Common.h"
#include "GLErrorDef.h"

#define OFFSET(n) ((char *)NULL + (n))
#define LOGI printf

ModelGL::ModelGL()
:model_vboID(0), model_iboID(0),mBufferInited(false),
 mBufferInvalidate(true),
    mIndexCount(0)
{
    model = NULL;
    vao = 0;
}

ModelGL::~ModelGL()
{
    glDeleteBuffers(1, &model_vboID);
    glDeleteBuffers(1, &model_iboID);
    glDeleteBuffers(1, &vao);
}

ModelGL * ModelGL::CreateFromObj(uint8_t *data, float scale, bool computeNormals,
                                 bool computeTangents,
                                 bool initBuffers,
                                 bool flipNormals) {
	Model* model = ModelObj::CreateFromObj(data, scale, computeNormals, computeTangents, flipNormals);
    if (!model) {
        printf("CreateFromObj failed\n");
        return NULL;
    }
		

	ModelGL* modelGL = new ModelGL;
	modelGL->model = model;
	modelGL->computeCenter();
    if (initBuffers) {
        printf("enter modelGL->initBuffers() \n");
        modelGL->initBuffers();
    }
	    

	return modelGL;
}

ModelGL* ModelGL::CreateFromObjWithPoints(uint8_t* data){
    Model* model = ModelObj::CreateFromObjWithPoints(data);
    if (!model)
        return NULL;

    ModelGL* modelGL = new ModelGL;
    modelGL->model = model;
    modelGL->computeCenter();
    modelGL->initBuffers();

    return modelGL;
}

ModelGL* ModelGL::CreateFromPreprocessed(uint8_t* data) {
	Model* model = Model::CreateFromPreprocessed(data);
	if (!model)
		return NULL;

	ModelGL* modelGL = new ModelGL;
	modelGL->model = model;
	modelGL->computeCenter();
	modelGL->initBuffers();

	return modelGL;
}

ModelGL* ModelGL::CreateFromModel(Model* model) {
	if (!model)
		return NULL;

	ModelGL* modelGL = new ModelGL;
	modelGL->model = model;
	modelGL->computeCenter();
	modelGL->initBuffers();

	return modelGL;
}

void ModelGL::computeCenter()
{
    model->getBoundingBox(m_minExtent, m_maxExtent);

    m_radius = (m_maxExtent - m_minExtent) / 2.0f;
    m_center = m_minExtent + m_radius;
}

void ModelGL::initBuffers()
{
    if(!mBufferInvalidate)
        return;

    glGenVertexArrays(1, &vao);  
    glGenBuffers(1, &model_vboID);
    glGenBuffers(1, &model_iboID);
	NvModelPrimType::Enum prim;
	mIndexCount = model->getCompiledIndexCount(prim);
    CHECK_GL;
    //checkGlError("bindbuffer vao");
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, model_vboID);
    glBufferData(GL_ARRAY_BUFFER, model->getCompiledVertexCount() * model->getCompiledVertexSize() * sizeof(float), model->getCompiledVertices(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_GL;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexCount * sizeof(uint32_t), model->getCompiledIndices(prim), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    CHECK_GL;
    mBufferInvalidate = false;
    mBufferInited = true;
    printf(" ModelGL::initBuffers()\n");
    printf("mIndexCount = %d\n", mIndexCount);
}

void ModelGL::rebindModel(Model* model_arg)
{
    model = model_arg;
    if (!mBufferInited) {
        initBuffers();
        return;
    }


    NvModelPrimType::Enum prim;
    mIndexCount = model->getCompiledIndexCount(prim);
    CHECK_GL;
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, model_vboID);
    glBufferData(GL_ARRAY_BUFFER, model->getCompiledVertexCount() * model->getCompiledVertexSize() * sizeof(float), model->getCompiledVertices(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_GL;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_iboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexCount * sizeof(uint32_t), model->getCompiledIndices(prim), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    CHECK_GL;
    mBufferInvalidate = false;
    mBufferInited = true;
    LOGI(" ModelGL::rebindModel()\n");
    printf("mIndexCount = %d\n", mIndexCount);
}


void ModelGL::bindBuffers()
{
    if(!mBufferInited){
        initBuffers();
        mBufferInited = true;
    }
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, model_vboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_iboID);
}

void ModelGL::unbindBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ModelGL::drawPoints(GLint positionHandle){

    glBindBuffer(GL_ARRAY_BUFFER, model_vboID);
    glVertexAttribPointer(positionHandle, model->getPositionSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), 0);
    glEnableVertexAttribArray(positionHandle);
    glDrawArrays(GL_POINTS , 0, model->getCompiledVertexCount());
    glDisableVertexAttribArray(positionHandle);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void ModelGL::drawElements(GLint positionHandle)
{
    bindBuffers();
    glVertexAttribPointer(positionHandle, model->getPositionSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), 0);
    glEnableVertexAttribArray(positionHandle);
	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(positionHandle);
    unbindBuffers();
}

void ModelGL::drawElementsWithTexCoord(GLint positionHandle, GLint texcoordHandle){
    bindBuffers();
    int vtxSize = model->getCompiledVertexSize();
    int posSize = model->getPositionSize();
    int coordOFFSET = model->getCompiledTexCoordOffset();
    glEnableVertexAttribArray(positionHandle);
    glVertexAttribPointer(positionHandle, model->getPositionSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), 0);



    if (texcoordHandle >= 0) {
        glEnableVertexAttribArray(texcoordHandle);
        glVertexAttribPointer(texcoordHandle, model->getTexCoordSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), OFFSET(model->getCompiledTexCoordOffset()*sizeof(float)));

    }

    glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(positionHandle);

    if (texcoordHandle >= 0)
        glDisableVertexAttribArray(texcoordHandle);
    unbindBuffers();
}

void ModelGL::drawElements(GLint positionHandle, GLint normalHandle)
{
    bindBuffers();
    glVertexAttribPointer(positionHandle, model->getPositionSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), 0);
    glEnableVertexAttribArray(positionHandle);
    
    if (normalHandle >= 0) {
        glVertexAttribPointer(normalHandle, model->getNormalSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), OFFSET(model->getCompiledNormalOffset()*sizeof(float)));
        glEnableVertexAttribArray(normalHandle);
    }
    
	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(positionHandle);
    if (normalHandle >= 0)
        glDisableVertexAttribArray(normalHandle);
    unbindBuffers();
}

void ModelGL::drawElements(GLint positionHandle, GLint normalHandle, GLint texcoordHandle, bool usePoints, bool useLines)
{
    bindBuffers();
    glBindBuffer(GL_ARRAY_BUFFER, model_vboID);
    //checkGlError("glBindBuffer");
    if (positionHandle >= 0) {
        glEnableVertexAttribArray(positionHandle);
        glVertexAttribPointer(positionHandle, model->getPositionSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), 0);
        
    }


    if (normalHandle >= 0) {
        glEnableVertexAttribArray(normalHandle);
        glVertexAttribPointer(normalHandle, model->getNormalSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), OFFSET(model->getCompiledNormalOffset()*sizeof(float)));
        
    }

    if (texcoordHandle >= 0) {
        glEnableVertexAttribArray(texcoordHandle);
        glVertexAttribPointer(texcoordHandle, model->getTexCoordSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), OFFSET(model->getCompiledTexCoordOffset()*sizeof(float)));
       
    }
    //checkGlError("glVertexAttribPointer");
    
    if (useLines) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);
    }
    else if (usePoints) {
        glDrawElements(GL_POINTS, mIndexCount, GL_UNSIGNED_INT, 0);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);
    }

    //checkGlError("glDrawElements");
    if (positionHandle >= 0)
        glDisableVertexAttribArray(positionHandle);
    if (normalHandle >= 0)
        glDisableVertexAttribArray(normalHandle);
    if (texcoordHandle >= 0)
        glDisableVertexAttribArray(texcoordHandle);
    unbindBuffers();
   
}

void ModelGL::drawElements(GLint positionHandle, GLint normalHandle, GLint texcoordHandle, GLint tangentHandle)
{
    bindBuffers();
    glVertexAttribPointer(positionHandle, model->getPositionSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), 0);
    glEnableVertexAttribArray(positionHandle);

    if (normalHandle >= 0) {
        glVertexAttribPointer(normalHandle, model->getNormalSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), OFFSET(model->getCompiledNormalOffset()*sizeof(float)));
        glEnableVertexAttribArray(normalHandle);
    }

    if (texcoordHandle >= 0) {
        glVertexAttribPointer(texcoordHandle, model->getTexCoordSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), OFFSET(model->getCompiledTexCoordOffset()*sizeof(float)));
        glEnableVertexAttribArray(texcoordHandle);
    }

    if (tangentHandle >= 0) {
        glVertexAttribPointer(tangentHandle,  model->getTangentSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), OFFSET(model->getCompiledTangentOffset()*sizeof(float)));
        glEnableVertexAttribArray(tangentHandle);
    }
	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(positionHandle);
    if (normalHandle >= 0)
        glDisableVertexAttribArray(normalHandle);
    if (texcoordHandle >= 0)
        glDisableVertexAttribArray(texcoordHandle);
    if (tangentHandle >= 0)
        glDisableVertexAttribArray(tangentHandle);
    unbindBuffers();
}

Model * ModelGL::getModel()
{
    return model;
}
