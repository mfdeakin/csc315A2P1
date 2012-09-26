
#ifndef _MATRIXH_
#define _MATRIXH_

struct matrix;

struct matrix *mtxCreate(unsigned width, unsigned height);
void mtxFree(struct matrix *mtx);
struct matrix *mtxAdd(struct matrix *lhs, struct matrix *rhs);
struct matrix *mtxNeg(struct matrix *mtx);
struct matrix *mtxMul(struct matrix *lhs, struct matrix *rhs);
float mtxDeterminate(struct matrix *mtx);
float mtxGet(struct matrix *mtx, unsigned x, unsigned y);
int mtxSet(struct matrix *mtx, unsigned x, unsigned y, float val);

#endif
