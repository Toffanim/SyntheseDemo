#include "pmat.h"


/*#include <g3x.h>*/


static double r=0.27;           /* ball ray (used for collision detection)           */

/*static G3Xcolor rouge  ={1.0,0.0,0.0};
static G3Xcolor jaune  ={1.0,1.0,0.0};
static G3Xcolor vert   ={0.0,1.0,0.0};
static G3Xcolor cyan   ={0.0,1.0,1.0};
static G3Xcolor bleu   ={0.0,0.0,1.0};
static G3Xcolor magenta={1.0,0.0,1.0};

static double alpha= 0.5;
static double ambi = 0.2;
static double diff = 0.3;
static double spec = 0.4;
static double shin = 0.5;*/

static void algo_pfixe(PMat *p)
{
	p->frc[0] = 0;
	p->frc[1] = 0;
	p->frc[2] = 0;
}

static void draw_fixe(PMat *p)
{
	/*g2x_FillCircle(p->pos.x,p->pos.y,r,G2Xr);	*/	
  /*	glPushMatrix();
    	glTranslatef(p->pos[0],p->pos[1],p->pos[2]);
    	glScalef(0.5,0.5,0.5);
		g3x_Material(rouge,ambi,diff,spec,shin,1.);
		glutSolidSphere(r,40,40);
  	glPopMatrix();*/
}

extern void pointFixe(PMat *p, G3Xpoint pos){
	p->algo = algo_pfixe;
	p->draw = draw_fixe;
	p->m = 1;
	p->pos[0] = pos[0];
	p->pos[1] = pos[1];
	p->pos[2] = pos[2];
	p->vit[0] = 0;
	p->vit[1] = 0;
	p->vit[2] = 0;
	p->frc[0] = 0;
	p->frc[1] = 0;
	p->frc[2] = 0;
}


/*extern double h;*/

static void algo_particule(PMat *p){ /* h représente le pas, a mettre en global */
	/*double h = 0.01;*/
	double h = 1/50.;
	p->vit[0] += (h / p->m) * p ->frc[0] ;
	p->vit[1] += (h / p->m) * p ->frc[1] ;
	p->vit[2] += (h / p->m) * p ->frc[2] ;
	
	p->pos[0] += h * p->vit[0];
	p->pos[1] += h * p->vit[1];
	p->pos[2] += h * p->vit[2];
	
	p->frc[0] = 0;
	p->frc[1] = 0;
	p->frc[2] = 0;
}

static void draw_mobile(PMat *p)
{
	/*g2x_FillCircle(p->pos.x,p->pos.y,r,G2Xb);	
  	glPushMatrix();
    	glTranslatef(p->pos[0],p->pos[1],p->pos[2]);
    	glScalef(0.5,0.5,0.5);
		g3x_Material(bleu,ambi,diff,spec,shin,1.);
		glutSolidSphere(r,40,40);
  	glPopMatrix();	*/	
}


extern void pointMobile(PMat *p, G3Xpoint pos, G3Xpoint vit, double m){
	p->algo = algo_particule;
	p->draw = draw_mobile;
	p->pos[0] = pos[0];
	p->pos[1] = pos[1];
	p->pos[2] = pos[2];
	p->vit[0] = vit[0];
	p->vit[1] = vit[1];
	p->vit[2] = vit[2];
	p->m = m;
	p->frc[0] = 0;
	p->frc[1] = 0;
	p->frc[2] = 0;
}



