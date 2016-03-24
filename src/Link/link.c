#include "link.h"

#define EPS 1.E-6

double G3Xdist(G3Xpoint p1, G3Xpoint p2){
	return sqrt(pow(p2[0]-p1[0], 2)+pow(p2[1]-p1[1], 2)+pow(p2[2]-p1[2], 2));
}

void G3Xsetvct(G3Xvector res, G3Xvector v1, G3Xvector v2){
	res[0]=v2[0]-v1[0];
	res[1]=v2[1]-v1[1];
	res[2]=v2[2]-v1[2];
}

static void algo_ressort(Link *link)
{
	double d = G3Xdist(link->M1->pos, link->M2->pos);
	if (d<EPS) return; /* pour éviter les problèmes éventuels */

	double f = - link->k * (1 - link->l0/d);
	
	G3Xvector vec;
	G3Xsetvct(vec, link->M1->pos, link->M2->pos);
	link->M1->frc[0] -= f * vec[0];
	link->M1->frc[1] -= f * vec[1];
	link->M1->frc[2] -= f * vec[2];
	link->M2->frc[0] += f * vec[0];
	link->M2->frc[1] += f * vec[1];
	link->M2->frc[2] += f * vec[2];
}

static void draw_ressort(Link *l)
{
  /*g3x_Line(l->M1->pos[0], l->M1->pos[1], l->M2->pos[0], l->M2->pos[1], bleu,1);*/
}

extern void ressort(Link *link, double k)
{
	link->algo = algo_ressort;
	link->draw = draw_ressort;
	link->k = k;
	link->M1 = NULL;
	link->M2 = NULL;
}


/* GRAVITE */
static void algo_gravite(Link *link)
{
  PMat* m=link->M1;
  while (m<link->M2)
  {
	  m->frc[1] += link->k;  /* la valeur de la gravité en y est stockée dans k */
	  m->frc[2] += link->k;
	  m++;
	}  
}

static void draw_gravite(Link *l)
{
  /* rien */
}

extern void gravite(Link *link, double g)
{
	link->algo = algo_gravite;
	link->draw = draw_gravite;
	link->k    = g;
	link->M1   = NULL;
	link->M2   = NULL;
}


extern void connectL(PMat *M1, Link *link, PMat *M2)
{
	link->M1 = M1;
	link->M2 = M2;
	link->l0 = G3Xdist(M1->pos, M2->pos);
}


