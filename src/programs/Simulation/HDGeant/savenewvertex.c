/*
savenewvertex: particle stoped because it decayed
               save the daughter particles and the
               decay vertex.


*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <HDDM/hddm_s.h>
#include <geant3.h>
#include "gid_map.h"

extern s_HDDM_t* thisInputEvent;

int getLastId();

void SaveNewVertex(int kcase, int Npart, float *gkin, 
		   float vertex[3], float tofg, int *iflgk,
                   int ipart, int itra, int istak) {


  // get pointer to all vertices
  s_Vertices_t* verts = thisInputEvent->physicsEvents->in[0].reactions->in[0].vertices;
  int VertexCount = verts->mult;

  // create additional space for one more vertex
  s_Vertices_t* NewVerts = make_s_Vertices(VertexCount+1);
  int i;
  for (i=0;i<VertexCount;i++){
    NewVerts->in[i] = verts->in[i];
  }
  NewVerts->mult = VertexCount;
  thisInputEvent->physicsEvents->in[0].reactions->in[0].vertices = NewVerts;
  free(verts);
  verts = NewVerts;
  //verts->in[VertexCount] = make_s_Vertex();
  verts->mult++;
  //printf("Number of stored Vertices is now: %d\n",verts->mult);
  
  // copy in the new vertex coordinates
  s_Origin_t* or = make_s_Origin();
  verts->in[VertexCount].origin = or;
  or->vx = vertex[0];
  or->vy = vertex[1];
  or->vz = vertex[2];
  or->t = tofg * 1e9;

  int lastId = getLastId();

  // copy in the new particles at this vertex
  s_Products_t* ps = make_s_Products(Npart);
  verts->in[VertexCount].products = ps;
  ps->mult = Npart;
  int thisId = lastId + 1;
  for (i = 0;i<Npart;i++){
    ps->in[i].momentum = make_s_Momentum();
    ps->in[i].momentum->px = gkin[i*5+0];
    ps->in[i].momentum->py = gkin[i*5+1];
    ps->in[i].momentum->pz = gkin[i*5+2];
    ps->in[i].momentum->E  = gkin[i*5+3];
    ps->in[i].type = gkin[i*5+4];
    ps->in[i].pdgtype = PDGtype(gkin[i*5+4]);
    ps->in[i].parentid = gidGetId(itra);
    ps->in[i].id = thisId;
    ps->in[i].mech = kcase;
    ps->in[i].decayVertex = VertexCount;
    gidSet(iflgk[i], thisId);
    thisId++;
  }
    
}



/* entry point from fortran */

void savenewvertex_ (int *kcase, int *N, float* gkin, 
		     float* vertex, float* tofg, int* iflgk, 
                     int* ipart, int* itra, int* istak) {

  SaveNewVertex(*kcase, *N, gkin, vertex, *tofg, iflgk, *ipart, *itra, *istak);

}

int getLastId() {
  int maxId = 0;
  s_Reactions_t* reacts;
  int reactCount, ir;
  reacts = thisInputEvent ->physicsEvents->in[0].reactions;
  reactCount = reacts->mult;
  for (ir = 0; ir < reactCount; ir++) {
    s_Vertices_t* verts;
    int vertCount, iv;
    s_Reaction_t* react = &reacts->in[ir];
    verts = react->vertices;
    vertCount = verts->mult;
    for (iv = 0; iv < vertCount; iv++) {
      s_Products_t* prods;
      int prodCount, ip;
      s_Vertex_t* vert = &verts->in[iv];
      prods = vert->products;
      prodCount = prods->mult;
      for (ip = 0; ip < prodCount; ip++) {
	s_Product_t* prod = &prods->in[ip];
	if (prod->id > maxId) {
	  maxId = prod->id;
	}
      }
    }
  }
  return maxId;
}
