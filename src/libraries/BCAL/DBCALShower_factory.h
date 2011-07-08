#ifndef _DBCALShower_factory_
#define _DBCALShower_factory_

/*
 *  DBCALShower_factory.h
 *
 *  Created by Matthew Shepherd on 3/24/11.
 *
 */

#include <JANA/JFactory.h>
#include <JANA/JEventLoop.h>

using namespace jana;

#include "BCAL/DBCALShower.h"

class DBCALShower_factory : public JFactory< DBCALShower > {
  
public:
  
  DBCALShower_factory();
  ~DBCALShower_factory(){}
  
private:
  
  jerror_t evnt(JEventLoop *loop, int eventnumber);	

  float m_zTarget;

// energy calibration parameters
  
  float m_scaleZ_p0;
  float m_scaleZ_p1;
  float m_scaleZ_p2;
  float m_scaleZ_p3;
  
  float m_nonlinZ_p0;
  float m_nonlinZ_p1;
  float m_nonlinZ_p2;
  float m_nonlinZ_p3;
  
};

#endif