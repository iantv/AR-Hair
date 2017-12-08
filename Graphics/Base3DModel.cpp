#include "Base3DModel.h"
#include <QDebug>

#define uint unsigned int

Base3DModel::Base3DModel() : _count(0) {

}

Base3DModel::Base3DModel(const char *fpath) : Base3DModel::Base3DModel() {
    if (!loadModelFromFile(fpath))
        qDebug() << "I couldn't open " << fpath;
}

Base3DModel::~Base3DModel() {
}

const GLfloat *Base3DModel::data() {
    return _data.constData();
}

int Base3DModel::sizeData() const {
    return _data.size();
}

int Base3DModel::vertexCount() const {
    return _data.size() / _count;
}

bool Base3DModel::loadModelFromFile(const char *fpath) {
    QVector<unsigned int> vIdx, uvIdx, nIdx;
    QVector<vec3> tv;
    QVector<vec2> tuv;
    QVector<vec3> tn;

    FILE *file = fopen(fpath, "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
    }
    printf("%s opened\n", fpath);
    while (true) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) break;
        if (strcmp(lineHeader, "o") == 0) {
            char object[128];
            fscanf(file, "%s", object);
        } else if (strcmp(lineHeader, "v") == 0) {
            vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            tv << vertex;
        } else if (strcmp(lineHeader, "vt") == 0) {
            vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            tuv << uv;
        } else if (strcmp(lineHeader, "vn") == 0) {
            vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            tn << normal;
        } else if (strcmp(lineHeader, "f") == 0) {
            this->parseFacet(file, &vIdx, &uvIdx, &nIdx);
        } else if (strcmp(lineHeader, "s") == 0) { // Just skip line started by 's' symbol
            char str[128];
            fscanf(file, "%s", str);
            if (strcmp(str, "off") == 0) {
                // ...
            } else {
                // ...
            }
        } else if (lineHeader[0] == '#') {
            this->skipComments(file, lineHeader);
        } else {
            printf("Unknown: %s", lineHeader);
        }
    }
    for (unsigned int i = 0; i < (unsigned int)vIdx.size(); i++) {
        this->setAttribsAndFillData(!vIdx.size(), !i, 3, &tv, vIdx[i] - 1);
        this->setAttribsAndFillData(!uvIdx.size(), !i, 2, &tuv, uvIdx[i] - 1);
        this->setAttribsAndFillData(!nIdx.size(), !i, 3, &tn, nIdx[i] - 1);
    }
    return true;
}

void Base3DModel::skipComments(FILE *file, char *lineHeader) {
    if (strcmp(lineHeader, "#") == 0) {
        char sym = '#';
        int t = 1;
        while ((sym != '\n') && (sym != EOF) && (t == 1)) {
            t = fscanf(file, "%c", &sym);
        }
    }
}

void Base3DModel::parseFacet(FILE *file, QVector<uint> *vInd, QVector<uint> *uvInd, QVector<uint> *nInd) {
    char str[128];
    unsigned int tvIdx[3] = { 0 }, tuvIdx[3] = { 0 }, tnIdx[3] = { 0 };
    bool vt = false, vn = false;
    for (int i = 0; i < 3; i++) {
        fscanf(file, "%s", str);
        for (int i = 0; i < strlen(str); i++) {
            if (str[i] == '\0') break;
            if (str[i] == '/') {
                if (str[i + 1] == '/') {
                    vn = true;
                    break;
                } else {
                    if (vt == true) {
                        vn = true;
                        break;
                    } else {
                        vt = true;
                    }
                }
            }
        }
        if (vt == false && vn == false) sscanf(str, "%d", &tvIdx[i]);
        else if (vt == false && vn == true) sscanf(str, "%d//%d", &tvIdx[i], &tnIdx[i]);
        else if (vt == true && vn == false) sscanf(str, "%d/%d", &tvIdx[i], &tuvIdx[i]);
        else sscanf(str, "%d/%d/%d", &tvIdx[i], &tuvIdx[i], &tnIdx[i]);
    }
    for (int i = 0; i < 3; i++) {
        *vInd << tvIdx[i];
        if (vt) *uvInd << tuvIdx[i];
        if (vn) *nInd << tnIdx[i];
    }
}

void Base3DModel::setAttribsAndFillData(bool vecIsEmpty, bool isFirstIter, GLint sizeAttr, void *tdata, unsigned int indx) {
    if (!vecIsEmpty) {
        if (isFirstIter) {
            this->setAttribs(sizeAttr);
        }
        if (sizeAttr == 2) {
            this->fillData((QVector<vec2> *)tdata, indx);
        } else {
            this->fillData((QVector<vec3> *)tdata, indx);
        }
    }
}

void Base3DModel::setAttribs(GLint size) {
    _indx << _data.size();
    _size << size;
    _count += size;
}

void Base3DModel::fillData(QVector<vec2> *tdata, unsigned int indx)
{
    _data << (*tdata)[indx].x;
    _data << (*tdata)[indx].y;
}

void Base3DModel::fillData(QVector<vec3> *tdata, unsigned int indx)
{
    _data << (*tdata)[indx].x;
    _data << (*tdata)[indx].y;
    _data << (*tdata)[indx].z;
}
