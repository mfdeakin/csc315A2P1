
#include "matrix.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef MTXDEBUG
#define debug(str) fprintf(stderr, str)
#else
int debug(char *str) {
	return 0;
}
#endif

struct matrix {
	float *mtx;
	unsigned width, height;
};

/* Calculates the position in the matrix array of the value */
unsigned matrixPos(struct matrix *mtx, unsigned x, unsigned y)
{
	return mtx->width * y + x;
}

struct matrix *mtxCreate(unsigned width, unsigned height)
{
	if(!width || !height) {
		debug("mtxCreate: Width and height are zero\n");
		return NULL;
	}
	struct matrix *mtx = malloc(sizeof(struct matrix));
	mtx->mtx = malloc(sizeof(float[width][height]));
	memset(mtx->mtx, 0.0f, sizeof(float[width][height]));
	mtx->width = width;
	mtx->height = height;
	return mtx;
}

struct matrix *mtxCreateI(unsigned size)
{
	if(!size) {
		debug("mtxCreateI: size is zero\n");
		return NULL;
	}
	struct matrix *mtx = malloc(sizeof(struct matrix));
	mtx->mtx = malloc(sizeof(float[size][size]));
	memset(mtx->mtx, 0.0f, sizeof(float[size][size]));
	for(int i = 0; i < 9; i += 4)
		mtx->mtx[i] = 1.0f;
	mtx->width = size;
	mtx->height = size;
	return mtx;
}

struct matrix *mtxCopy(struct matrix *mtx)
{
	struct matrix *copy = mtxCreate(mtx->width, mtx->height);
	int i;
	for(i = 0; i < (mtx->width * mtx->height); i++)
		copy->mtx[i] = mtx->mtx[i];
	return copy;
}

void mtxFree(struct matrix *mtx)
{
	free(mtx->mtx);
	free(mtx);
}

struct matrix *mtxAdd(struct matrix *lhs, struct matrix *rhs)
{
	if(lhs->width != rhs->width || lhs->height != rhs->height) {
		debug("mtxAdd: lhs and rhs are not valid for adding\n");
		return NULL;
	}
	struct matrix *mtx = mtxCreate(lhs->width, lhs->height);
	int i;
	for(i = 0; i < (lhs->width * lhs->height); i++)
		mtx->mtx[i] = lhs->mtx[i] + rhs->mtx[i];
	return mtx;
}

struct matrix *mtxSub(struct matrix *lhs, struct matrix *rhs)
{
	if(lhs->width != rhs->width || lhs->height != rhs->height) {
		debug("mtxSub: lhs and rhs are not valid for subtracting\n");
		return NULL;
	}
	struct matrix *mtx = mtxCreate(lhs->width, lhs->height);
	int i;
	for(i = 0; i < (lhs->width * lhs->height); i++)
		mtx->mtx[i] = lhs->mtx[i] - rhs->mtx[i];
	return mtx;
}

struct matrix *mtxNeg(struct matrix *mtx)
{
	struct matrix *ret = mtxCreate(mtx->width, mtx->height);
	int i;
	for(i = 0; i < (mtx->width * mtx->height); i++)
		ret->mtx[i] = -mtx->mtx[i];
	return ret;
}

struct matrix *mtxMul(struct matrix *lhs, struct matrix *rhs)
{
	if(lhs->width != rhs->height) {
		debug("mtxMul: lhs and rhs cannot be multiplied\n");
		return NULL;
	}
	struct matrix *mtx = mtxCreate(rhs->width, lhs->height);
	unsigned i;
	unsigned x = 0, y = 0;
	for(i = 0; i < (mtx->width * mtx->height); i++, x++) {
		if(x == mtx->width) {
			x = 0;
			y++;
		}
		unsigned xyPos, lOff, rOff;
		lOff = matrixPos(lhs, 0, y);
		rOff = matrixPos(rhs, x, 0);
		for(xyPos = 0; xyPos < lhs->width; xyPos++)
			mtx->mtx[i] += lhs->mtx[lOff + xyPos] *
				rhs->mtx[rOff + xyPos * rhs->width];
	}
	return mtx;
}

float mtxDeterminate(struct matrix *mtx)
{
	if(mtx->width != mtx->height) {
		debug("mtxDeterminate: Not a square matrix\n");
		return 0;
	}
	if(mtx->width == 1)
		return mtx->mtx[0];
	if(mtx->width == 2)
		return mtx->mtx[0] * mtx->mtx[3] - mtx->mtx[1] * mtx->mtx[2];
	if(mtx->width == 3) {
		float det = 0;
		int diag;
		/* Positive diagonals */
		int pindices[3][3] = {{0, 4, 8}, {1, 5, 6}, {2, 3, 7}};
		for(diag = 0; diag < 3; diag++) {
			float cur = 1.0f;
			int i;
			for(i = 0; i < 3; i++)
				cur *= mtx->mtx[pindices[diag][i]];
			det += cur;
		}
		/* Negative diagonals */
		int nindices[3][3] = {{0, 5, 7}, {1, 3, 8}, {2, 4, 6}};
		for(diag = 0; diag < mtx->width; diag++) {
			float cur = 1.0f;
			int i;
			for(i = 0; i < 3; i++)
				cur *= mtx->mtx[nindices[diag][i]];
			det += cur;
		}
		return det;
	}
	debug("mtxDeterminate: Not implemented above 3x3\n");
	return 0.0f;
}

float mtxGet(struct matrix *mtx, unsigned x, unsigned y)
{
	if(x >= mtx->width || y >= mtx->height) {
		debug("mtxGet: Invalid index\n");
		return 0.0;
	}
	unsigned pos = matrixPos(mtx, x, y);
	return mtx->mtx[pos];
}

int mtxSet(struct matrix *mtx, unsigned x, unsigned y, float val)
{
	if(x >= mtx->width || y >= mtx->height) {
		debug("mtxSet: Invalid index\n");
		return -1;
	}
	unsigned pos = matrixPos(mtx, x, y);
	mtx->mtx[pos] = val;
	return 0;
}
