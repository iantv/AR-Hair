#ifndef BASE3DMODEL_H
#define BASE3DMODEL_H

#include <qopengl.h>
#include <QVector>

struct vec3 {
    GLfloat x, y, z;
};

struct vec2 {
    GLfloat x, y;
};

enum VBOType {
    VBO_VERTICES = 0,
    VBO_TEXCOORS,
    VBO_NORMALS
};

class Base3DModel
{
public:
    Base3DModel();
    Base3DModel(const char *fpath);
    ~Base3DModel();
    const GLfloat *data();
    int sizeData() const;
    int vertexCount() const;
    int _count;
    QVector <int> _indx;
    QVector <GLint> _size;
private:
    bool loadModelFromFile(const char *fpath);
    void parseFacet(FILE *file, QVector<uint> &vInd, QVector<uint> &uvInd, QVector<uint> &nInd);
    void skipComments(FILE *file, char *lineHeader);
    void setAttribs(GLint size);
    void setAttribsAndFillData(bool vecIsEmpty, bool isFirstIter, GLint sizeAttr, void *tdata, unsigned int indx);
    void fillData(QVector<vec2> *tdata, unsigned int indx);
    void fillData(QVector<vec3> *tdata, unsigned int indx);
    QVector<GLfloat> _data;
};

#endif // BASE3DMODEL_H
