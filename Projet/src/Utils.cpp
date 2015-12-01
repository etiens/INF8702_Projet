
#include <glload/gl_all.h>
#include <GL/glu.h>

#include "Utils.hpp"

int printOglError(std::string file, int line)
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n",
			     file.c_str(), line, gluErrorString(glErr));
        retCode = 1;
    }
    return retCode;
}
