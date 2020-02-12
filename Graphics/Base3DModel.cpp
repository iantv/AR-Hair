#include "Base3DModel.h"
#include <QDebug>
#include <QVector>

#define _CRT_SECURE_NO_WARNINGS
#define uint unsigned int

Base3DModel::Base3DModel() : _count(0) {

}

Base3DModel::Base3DModel(const char *fpath) : Base3DModel::Base3DModel() {
    if (!loadModelFromFile(fpath))
        qDebug() << "I couldn't open " << fpath;
}

Base3DModel::~Base3DModel() {
    _indx.clear();
    _size.clear();
    _data.clear();
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

    errno_t err;
    FILE *file;
    err = fopen_s(&file, fpath, "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
    }
    printf("%s opening\n", fpath);
    while (true) {
        char lineHeader[128];
        int res = fscanf_s(file, "%s", lineHeader);
        if (res == EOF) break;
        if (strcmp(lineHeader, "o") == 0) {
            char object[128];
            fscanf_s(file, "%s", object);
        } else if (strcmp(lineHeader, "v") == 0) {
            vec3 vertex;
            fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            tv << vertex;
        } else if (strcmp(lineHeader, "vt") == 0) {
            vec2 uv;
            fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
            tuv << uv;
        } else if (strcmp(lineHeader, "vn") == 0) {
            vec3 normal;
            fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            tn << normal;
        } else if (strcmp(lineHeader, "f") == 0) {
            this->parseFacet(file, vIdx, uvIdx, nIdx);
        } else if (strcmp(lineHeader, "s") == 0) { // Just skip line started by 's' symbol
            char str[128];
            fscanf_s(file, "%s", str);
            if (strcmp(str, "off") == 0) {
                // ...
            } else {
                // ...
            }
        } else if (lineHeader[0] == '#') {
            this->skipComments(file, lineHeader);
        } else if (strcmp(lineHeader, "usemtl") == 0) {
            char str[128];
            fscanf_s(file, "%s", str);
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
            t = fscanf_s(file, "%c", &sym);
        }
    }
}

void Base3DModel::parseFacet(FILE *file, QVector<uint> &vInd, QVector<uint> &uvInd, QVector<uint> &nInd) {
    char str[128];
    fgets(str, 128, file);
    for (char *t = str;*t >= ' ';) {
        ++t;
        QVector<uint> triple(3, 0);
        for (int i = 0; i < 3; ++i) {
            char *p = t;
            for (; isdigit(*p); ++p) {}
            if (p > t)
                triple[i] = atoi(t);
            t = p;
            if (*t == '/') ++t; else break;
        }
        //printf("%d %d %d\n", triple[0], triple[1], triple[2]);
        vInd << triple[0];
        uvInd << triple[1];
        nInd << triple[2];
    }

    if (vInd.size() == 4) {
        vInd << vInd[0] << vInd[2];
        uvInd << uvInd[0] << uvInd[2];
        nInd << nInd[0] << nInd[2];
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
