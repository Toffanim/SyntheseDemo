#ifndef _LINK_H_
#define _LINK_H_

#include "../PMat/pmat.h"

typedef struct _link{
	PMat *M1, *M2;
	double k, z, l0, s;
	void (*algo)(struct _link*);
	void (*draw)(struct _link*);
}Link;

void ressort(Link *link, double k);
void gravite(Link *link, double g);

void connectL(PMat *M1, Link *link, PMat *M2);
/*
void algo_ressort(Link *link);
*/
#endif
