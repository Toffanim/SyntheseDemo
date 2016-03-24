#ifndef _PMAT_H_
#define _PMAT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>


/* coordonnees spatiales (x,y,z) */
typedef double   G3Xcoord [3];
typedef G3Xcoord G3Xpoint ,G3Xvector;

typedef struct _PMat{ /* 2 types Masse mobile (particule), Masse immobile (point fixe) */ 
	G3Xpoint pos;
	G3Xpoint vit;
	G3Xpoint frc;
	double m;
	void (*algo)(struct _PMat*);
	void (*draw)(struct _PMat*);
}PMat;

void pointFixe(PMat *p, G3Xpoint pos);
void pointMobile(PMat *p, G3Xpoint pos, G3Xpoint vit, double m);
/*
void algo_pfixe(PMat *p);
void algo_particule(PMat *p);
*/
#endif
