// $Id$

//    File: DDANAEVIO_factory.h
// Created: Mon Mar 15 09:08:37 EDT 2010
// Creator: wolin (on Linux stan.jlab.org 2.6.18-164.el5 x86_64)



#ifndef _DDANAEVIO_factory_
#define _DDANAEVIO_factory_


#include <string>
#include <map>
#include <set>
using namespace std;


#include <JANA/JFactory.h>
using namespace jana;


#include <danaevio/DDANAEVIODOMTree.h>



//------------------------------------------------------------------------------------


class DDANAEVIO_factory : public JFactory<DDANAEVIODOMTree> {
  
 public:
  DDANAEVIO_factory();
  ~DDANAEVIO_factory() {};

  static const map< string, pair<uint16_t,uint8_t> > *getTagMapPointer();


 private:
  jerror_t evnt(JEventLoop *eventLoop, int eventnumber);
  void get_tagNum_dictionary(void);
  static void startElement(void *userData, const char *xmlname, const char **atts);

  template<typename T> evioDOMNodeP createLeafNode(string nameId);
  evioDOMNodeP createContainerNode(string nameId);


  void addObjIdBank(evioDOMTree &tree);

  void addDMCTrackHit(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDBeamPhoton(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDMCThrown(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDFCALTruthShower(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDBCALTruthShower(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDTOFTruth(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDSCTruthHit(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDMCTrajectoryPoint(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDCDCHit(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDFDCHit(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDFCALHit(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDHDDMBCALHit(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDHDDMTOFHit(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDSCHit(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDCDCTrackHit(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDFDCPseudo(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDTrackWireBased(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDTrackTimeBased(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDChargedTrack(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDPhoton(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDVertex(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDTrackCandidate(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDBCALPhoton(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDFCALPhoton(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDChargedTruthMatch(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDTOFRawHitMC(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDTOFRawHit(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDTOFHit(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDTOFPoint(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDTOFMCResponse(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDBCALHit(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDBCALMCResponse(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDBCALShower(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDFCALCluster(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDFDCCathodeCluster(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDFDCSegment(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDTwoGammaFit(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDParticle(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDTAGMHit(JEventLoop *eventLoop, evioDOMTree &tree);
  void addDTAGHHit(JEventLoop *eventLoop, evioDOMTree &tree);


  // map of which factory/tags to convert, can be set on-the-fly via setEVIOMap()
  map< string, set<string> > evioMap;


  // event-specific global object id map
  map<int,string> objIdMap;


public:
  void setEVIOMap(string danaevio);

};

#endif // _DDANAEVIO_factory_


//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
