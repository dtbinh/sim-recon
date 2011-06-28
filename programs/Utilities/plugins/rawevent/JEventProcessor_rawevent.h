// $Id$
//
//    File: JEventProcessor_rawevent.h
// Created: Fri Jun 24 12:05:19 EDT 2011
// Creator: wolin (on Linux stan.jlab.org 2.6.18-194.11.1.el5 x86_64)
//

#ifndef _JEventProcessor_rawevent_
#define _JEventProcessor_rawevent_


#include <vector>


#include <JANA/JApplication.h>
#include <JANA/JEventProcessor.h>
#include <JANA/JEventLoop.h>


#include <evioFileChannel.hxx>
#include <evioUtil.hxx>


#include "FCAL/DFCALHit.h"
#include "BCAL/DBCALHit.h"
#include "TOF/DTOFRawHit.h"
#include "CDC/DCDCHit.h"
#include "FDC/DFDCHit.h"
#include "START_COUNTER/DSCHit.h"
#include "TAGGER/DTagger.h"


#include<boost/tuple/tuple.hpp>


using namespace std;
using namespace jana;
using namespace evio;
using namespace boost;


typedef tuple<int,int,int> cscVal;



//----------------------------------------------------------------------------


class JEventProcessor_rawevent : public jana::JEventProcessor {

	public:
		JEventProcessor_rawevent();
		~JEventProcessor_rawevent();
		const char* className(void){return "JEventProcessor_rawevent";}

	private:
		jerror_t init(void);
		jerror_t brun(jana::JEventLoop *eventLoop, int runnumber);
		jerror_t evnt(jana::JEventLoop *eventLoop, int eventnumber);
		jerror_t erun(void);
		jerror_t fini(void);


                // these routines access the translation table
                cscVal DTOFRawHitTranslation(int bar,int plane, int lr);
                cscVal DBCALHitTranslation(int module,int layer,int sector,int end);
                cscVal DFCALHitTranslation(int row,int column,float x,float y);
                cscVal DFDCHitTranslation(int layer,int module,int element,int plane,int gPlane,int gLayer);
                cscVal DCDCHitTranslation(int ring,int straw);
                cscVal DSCHitTranslation(int sector);
                cscVal DTaggerTranslation(int row,int column);


};

#endif // _JEventProcessor_rawevent_


//----------------------------------------------------------------------------