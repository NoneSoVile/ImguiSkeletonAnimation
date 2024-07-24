/*
 * Copyright (c) YF Technologies Co., Ltd. All rights reserved.
 *
 * AVM GL Error Definition
 *
 * @author Ben Chan
 * @since 2022-09-24
 */

#ifndef AVM_GL_ERROR_DEF_H
#define AVM_GL_ERROR_DEF_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#if 1
#define CHECK_GL \
    do { \
        GLint _glerr_ = glGetError();\
        if (_glerr_) \
            printf("%s: gl error (0x%x) line(%d)\n", __func__, _glerr_, __LINE__); \
    } while(0)
#else
#define CHECK_GL
#endif
#endif //AVM_GL_ERROR_DEF_H
