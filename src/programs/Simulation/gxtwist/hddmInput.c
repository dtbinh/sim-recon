/*
 * hddmInput  - functions to handle Monte Carlo generator input to HDGeant
 *		through the standard hddm i/o mechanism.
 *
 * Interface:
 *	openInput(filename) - open input stream to file <filename>
 *	skipInput(count) - skip next <count> events on open input file
 *      nextInput() - advance to next event on open input stream
 *      loadInput() - push current input event to Geant kine structures
 *      storeInput() - pop current input event from Geant kine structures
 *	closeInput() - close currently open input stream
 *
 * Richard Jones
 * University of Connecticut
 * July 13, 2001
 *
 * Usage Notes:
 * 1)	Most Monte Carlo generators do not care where the vertex is placed
 *	inside the target, and specify only the final-state particles'
 *	momenta.  In this case the vertex position has to be randomized by
 *	the simulation within the beam/target overlap volume.  If the vertex
 *	position from the generator is (0,0,0) then the simulation vertex is
 *	generated uniformly inside the cylinder specified by TARGET_LENGTH,
 *	BEAM_DIAMETER, and TARGET_CENTER defined below.
 *
 * Revision history:
 * >  Dec 15, 2004 - Richard Jones
 *	Changed former behaviour of simulation to overwrite the vertex
 *	coordinates from the input record, if the simulation decides to
 *	override the input values.  At present this happens whenever the
 *      input record specifies 0,0,0 for the vertex, but in the future it
 * 	may be decided to let the simulator determine the vertex position
 *	in other cases.  Since it is not part of the simulation proper, the
 *	decision was made to store this information in the reaction tag.
 */

#define TARGET_LENGTH 0
#define BEAM_DIAMETER 0.2
#define TARGET_CENTER 0


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <HDDM/hddm_mc_s.h>
#include <geant3.h>

mc_s_iostream_t* thisInputStream = 0;
mc_s_HDDM_t* thisInputEvent = 0;

int openInput (char* filename)
{
   thisInputStream = open_mc_s_HDDM(filename);
   return (thisInputStream == 0);
}

int skipInput (int count)
{
   int* buff = (int*) malloc(1000000);
   while (thisInputStream && (count > 0))
   {
      int ret = fread(buff, sizeof(int), 1, thisInputStream->fd);
      if (ret == 1)
      {
         int nw = *buff;
         ret = fread(buff, sizeof(int), nw, thisInputStream->fd);
         --count;
      }
      else
      {
         break;
      }
   }
   free(buff);
   return count;
}

int nextInput ()
{
   if (thisInputStream == 0)
   {
      return 9;		/* input stream was never opened */
   }
   else if (thisInputEvent)
   {
      flush_mc_s_HDDM(thisInputEvent, 0);
   }
   thisInputEvent = read_mc_s_HDDM(thisInputStream);
   return (thisInputEvent == 0);
}

int loadInput ()
{
   mc_s_Reactions_t* reacts;
   int reactCount, ir;

   reacts = thisInputEvent->physicsEvents->in[0].reactions;
   reactCount = reacts->mult;
   for (ir = 0; ir < reactCount; ir++)
   {
      mc_s_Vertices_t* verts;
      int vertCount, iv;
      mc_s_Reaction_t* react = &reacts->in[ir];
      verts = react->vertices;
      vertCount = verts->mult;
      for (iv = 0; iv < vertCount; iv++)
      {
         float v[3];
         int ntbeam = 0;
         int nttarg = 0;
         int nubuf = 0;
         float ubuf;
         int nvtx;
         mc_s_Products_t* prods;
         int prodCount, ip;
         mc_s_Vertex_t* vert = &verts->in[iv];
         v[0] = vert->origin->vx;
         v[1] = vert->origin->vy;
         v[2] = vert->origin->vz;
         if ((v[0] == 0) && (v[1] == 0) && (v[2] == 0))
         {
            v[0] = 1;
            v[1] = 1;
            v[2] = TARGET_CENTER;
            while (v[0]*v[0] + v[1]*v[1] > 0.25)
            {
               int len = 3;
               grndm_(v,&len);
               v[0] -= 0.5;
               v[1] -= 0.5;
               v[2] -= 0.5;
            }
            v[0] *= BEAM_DIAMETER;
            v[1] *= BEAM_DIAMETER;
            v[2] *= TARGET_LENGTH;
            v[2] += TARGET_CENTER;
            vert->origin->vx = v[0];
            vert->origin->vy = v[1];
            vert->origin->vz = v[2];
         }
         gsvert_(v, &ntbeam, &nttarg, &ubuf, &nubuf, &nvtx);
         prods = vert->products;
         prodCount = prods->mult;
         for (ip = 0; ip < prodCount; ip++)
         {
            int ntrk;
            float p[3];
            Particle_t kind;
            mc_s_Product_t* prod = &prods->in[ip];
            kind = prod->type;
            p[0] = prod->momentum->px;
            p[1] = prod->momentum->py;
            p[2] = prod->momentum->pz;
            if (prod->decayVertex == 0)
            {
               gskine_(p, &kind, &nvtx, &ubuf, &nubuf, &ntrk);
            }
         }
      }
   }
   return 0;
}

int storeInput (int runNo, int eventNo, int ntracks)
{
   mc_s_PhysicsEvents_t* pes;
   mc_s_Reactions_t* rs;
   mc_s_Vertices_t* vs;
   mc_s_Origin_t* or;
   mc_s_Products_t* ps;
   int nvtx, ntbeam, nttarg, itra, nubuf;
   float vert[3], plab[3], tofg, ubuf[10];
   Particle_t kind;

   if (thisInputEvent)
   {
      flush_mc_s_HDDM(thisInputEvent, 0);
   }
   thisInputEvent = make_mc_s_HDDM();
   thisInputEvent->physicsEvents = pes = make_mc_s_PhysicsEvents(1);
   pes->mult = 1;
   pes->in[0].reactions = rs = make_mc_s_Reactions(1);
   rs->mult = 1;
   rs->in[0].vertices = vs = make_mc_s_Vertices(99);
   vs->mult = 0;
   for (itra = 1; itra <= ntracks; itra++)
   {
      gfkine_(&itra,vert,plab,&kind,&nvtx,ubuf,&nubuf);
      if (nvtx < 1)
      {
         return 1;
      }
      else
      {
         vs->mult = (nvtx < vs->mult)? vs->mult : nvtx;
      }
      gfvert_(&nvtx,vert,&ntbeam,&nttarg,&tofg,ubuf,&nubuf);
      or = vs->in[nvtx-1].origin;
      ps = vs->in[nvtx-1].products;
      if (or == HDDM_NULL)
      {
         or = make_mc_s_Origin();
         vs->in[nvtx-1].origin = or;
         or->vx = vert[0];
         or->vy = vert[1];
         or->vz = vert[2];
         or->t = tofg;
      }
      if (ps == HDDM_NULL)
      {
         ps = make_mc_s_Products(ntracks);
         vs->in[nvtx-1].products = ps;
         ps->mult = 0;
      }
      ps->in[ps->mult].type = kind;
      ps->in[ps->mult].momentum = make_mc_s_Momentum();
      ps->in[ps->mult].momentum->px = plab[0];
      ps->in[ps->mult].momentum->py = plab[1];
      ps->in[ps->mult].momentum->pz = plab[2];
      ps->in[ps->mult].momentum->E  = plab[3];
      ps->mult++;
   }
   pes->in[0].runNo = runNo;
   pes->in[0].eventNo = eventNo;
   return 0;
}

int closeInput ()
{
   if (thisInputStream)
   {
      close_mc_s_HDDM(thisInputStream);
      thisInputStream = 0;
   }
   return 0;
}


/* entry points from Fortran */

int openinput_ (char* filename)
{
   int retcode = openInput(strtok(filename," "));
   return retcode;
}

int skipinput_ (int* count)
{
   return skipInput(*count);
}

int nextinput_ ()
{
   return nextInput();
}

int loadinput_ ()
{
   return loadInput();
}

int storeinput_ (int* runNo, int* eventNo, int* ntracks)
{
   return storeInput(*runNo,*eventNo,*ntracks);
}

int closeinput_ ()
{
   return closeInput();
}
