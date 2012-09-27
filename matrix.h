
#ifndef __MATRIX_H
#define __MATRIX_H

struct matrix;

struct matrix *mtxCreate(unsigned width, unsigned height);
struct matrix *mtxCreateI(unsigned size);
struct matrix *mtxCopy(struct matrix *mtx);
struct matrix *mtxFromArray(float array[], unsigned w, unsigned h);
void mtxFree(struct matrix *mtx);
struct matrix *mtxAdd(struct matrix *lhs, struct matrix *rhs);
struct matrix *mtxNeg(struct matrix *mtx);
struct matrix *mtxMul(struct matrix *lhs, struct matrix *rhs);
float mtxDeterminate(struct matrix *mtx);
float mtxGet(struct matrix *mtx, unsigned x, unsigned y);
int mtxSet(struct matrix *mtx, unsigned x, unsigned y, float val);

#endif
