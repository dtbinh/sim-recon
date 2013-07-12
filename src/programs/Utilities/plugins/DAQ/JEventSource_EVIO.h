// $Id$
//
//    File: JEventSource_EVIO.h
// Created: Tue Aug  7 15:22:29 EDT 2012
// Creator: davidl (on Darwin harriet.jlab.org 11.4.0 i386)
//

#ifndef _JEventSource_EVIO_
#define _JEventSource_EVIO_


#include <map>
#include <vector>
#include <queue>
#include <list>
#include <set>
using std::map;
using std::vector;
using std::queue;
using std::set;

#include <JANA/jerror.h>
#include <JANA/JEventSource.h>
#include <JANA/JEvent.h>
#include <JANA/JFactory.h>

#include <evioChannel.hxx>
#include <evioUtil.hxx>
using namespace evio;

#include "DModuleType.h"
#include "Df250PulseIntegral.h"
#include "Df250StreamingRawData.h"
#include "Df250WindowSum.h"
#include "Df250PulseRawData.h"
#include "Df250TriggerTime.h"
#include "Df250PulseTime.h"
#include "Df250WindowRawData.h"
#include "Df125TriggerTime.h"
#include "Df125PulseIntegral.h"
#include "Df125PulseTime.h"
#include "DF1TDCHit.h"
#include "DF1TDCTriggerTime.h"

//-----------------------------------------------------------------------
/// The JEventSource_EVIO class implements a JEventSource capable of reading in
/// EVIO data from raw data files produced in Hall-D. It can read in entangled
/// (or blocked) events as well as single events. The low-level objects produced
/// reflect the data generated by the front end digitization electronics.
///
/// The type of boards it can understand can be expanded to include additional
/// boards. To do this, files must be edited in a few places:
///
/// In DModuleType.h
/// ------------------
/// 1.) The new module type must be added to the type_id_t enum.
///     Make sure "N_MODULE_TYPES" is the last item in the enum.
///
/// 2.) Add a case for the module type to the switch in the
///     GetModule method.
///
///
/// Create Data Object files
/// -------------------------
///
/// 1.) For each type of data produced by the new module, create
///     a class to represent it. It is highly recommended to use
///     a naming scheme that reflects the naming convention used
///     in the manual. This makes it easier for people trying to
///     understand the low-level data objects in terms of the
///     manual for the module. See Df250WindowSum.h or
///     Df250PulseIntegral.h for examples.
///
///
/// JEventSource_EVIO.h
/// -------------------
/// 1.) Add an appropriate #include near the top of the file for
///     each type of data object created in the previous step.
///
/// 2.) Add an appropriate declaration for a "ParseXXXBank"
///     where the "XXX" is the new module type.
///     example:
///        void ParseF1TDCBank(evioDOMNodeP bankPtr, list<ObjList*> &events);
///
/// 3.) If the routine JFactory_base_CopyTo() still exists at the
///     bottom of this file, then add a line for each data type to it.
///
///
/// JEventSource_EVIO.cc
/// --------------------
/// 1.) In the JEventSource_EVIO::JEventSource_EVIO() constructor,
///     add a line to insert the data type into event_source_data_types
///     for each data type the module produces.
///
/// 2.) In the "ParseEVIOEvent()" method, add a case for the
///     new module type that calls the new "ParseXXXBank()"
///     method. (Note if this is JLab module, then you'll
///     need to add a case to ParseJLabModuleData() ).
///
/// 3.) Add the new ParseXXXBank() method. Preferrably to the
///     end of the file or more importantly, in the order the
///     method appears in the class definition.
///
///----------------------------------------------------------------------

class JEventSource_EVIO: public jana::JEventSource{
	public:
		                   JEventSource_EVIO(const char* source_name);
		           virtual ~JEventSource_EVIO();
		virtual const char* className(void){return static_className();}
		 static const char* static_className(void){return "JEventSource_EVIO";}
		
		  virtual jerror_t ReadEVIOEvent(void);
		
		          jerror_t GetEvent(jana::JEvent &event);
		              void FreeEvent(jana::JEvent &event);
				  jerror_t GetObjects(jana::JEvent &event, jana::JFactory_base *factory);
	
	protected:
	
		void ConnectToET(const char* source_name);
		
		int32_t last_run_number;
		
		evioChannel *chan;
		map<tagNum, MODULE_TYPE> module_type;

		bool AUTODETECT_MODULE_TYPES;
		bool DUMP_MODULE_MAP;

		// Utility class to hold pointers to containers for
		// all types of data objects we produce. This gets passed
		// into bank processor methods so that they can append
		// to the lists. Note that the naming scheme here needs to
		// include the exact name of the class with a "v" in front
		// and an "s" in back. (See #define in JEventSource_EVIO.cc
		// for more details.)
		vector< vector<DDAQAddress*> > hit_objs;
		class ObjList{
		public:
			
			int32_t run_number;
			bool own_objects; // keeps track of whether these objects were copied to factories or not
			
			vector<DDAQAddress*> hit_objs;
		};
	
		// EVIO events with more than one DAQ event ("blocked" or
		// "entangled" events") are parsed and have the events
		// stored in the following container so they can be dispensed
		// as needed.
		queue<ObjList*> stored_events;
		
		// List of the data types this event source can provide
		// (filled in the constructor)
		set<string> event_source_data_types;
	
		int32_t GetRunNumber(evioDOMTree *evt);
		MODULE_TYPE GuessModuleType(const uint32_t *istart, const uint32_t *iend);
		bool IsF250ADC(const uint32_t *istart, const uint32_t *iend);
		bool IsF1TDC(const uint32_t *istart, const uint32_t *iend);
		void DumpModuleMap(void);
		void DumpBinary(const uint32_t *iptr, const uint32_t *iend, uint32_t MaxWords);

		
		void MergeObjLists(list<ObjList*> &events1, list<ObjList*> &events2);

		void ParseEVIOEvent(evioDOMTree *evt, uint32_t run_number);
		void ParseJLabModuleData(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void Parsef250Bank(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void Parsef125Bank(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseF1TDCBank(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseF1TDCBank_style1(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseF1TDCBank_style2(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseTSBank(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseTIBank(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);
		void ParseCAEN1190(int32_t rocid, const uint32_t* &iptr, const uint32_t *iend, list<ObjList*> &events);

		// f250 methods
		void MakeDf250WindowRawData(ObjList *objs, uint32_t rocid, uint32_t slot, uint32_t itrigger, const uint32_t* &iptr);
		void MakeDf250PulseRawData(ObjList *objs, uint32_t rocid, uint32_t slot, uint32_t itrigger, const uint32_t* &iptr);

	
};


//----------------------------------------------------------------------
/// JFactory_base_CopyToT and JFactory_base_CopyTo
///
/// A Mantis request has been submitted to add a virtual method to
/// JFactory_base that takes a vector<JObject*>& with an overload
/// of that method in the JFactory<T> subclass. The JFactory<T> method
/// will then try to dynamically cast each JObject* into the appropriate
/// type and store it in the factory. When that is working, these two
/// routines will not be required here.
/// 
/// In the  meantime, this serves as a placeholder that can be easily
/// converted once JANA has been updated.
//----------------------------------------------------------------------
template<class T>
bool JFactory_base_CopyToT(jana::JFactory_base *fac, vector<jana::JObject *>& objs)
{
	// Try casting this factory to the desired type of JFactory<>
	jana::JFactory<T> *tfac = dynamic_cast<jana::JFactory<T>* >(fac);
	if(!tfac) return false;
	
	// Factory cast worked. Cast all pointers
	vector<T*> tobjs;
	for(unsigned int i=0; i<objs.size(); i++){
		T *tobj = dynamic_cast<T*>(objs[i]);
		if(tobj) tobjs.push_back(tobj);
	}
	
	// If all input objects weren't converted, then just return false
	if(tobjs.size() != objs.size()) return false;
	
	// Copy pointers into factory
	if(tobjs.size()>0) tfac->CopyTo(tobjs);
	return true;
}

//----------------------------
// JFactory_base_CopyTo
//----------------------------
static bool JFactory_base_CopyTo(jana::JFactory_base *fac, vector<jana::JObject *>& objs)
{
	// Eventually, this will be a virtual method of JFactory_base
	// that gets implemented in JFactory<T> which will know how
	// to cast the objects. For now though, we have to try all known
	// data types.
	if( JFactory_base_CopyToT<Df250PulseIntegral>(fac, objs) ) return true;
	if( JFactory_base_CopyToT<Df250StreamingRawData>(fac, objs) ) return true;
	if( JFactory_base_CopyToT<Df250WindowSum>(fac, objs) ) return true;
	if( JFactory_base_CopyToT<Df250PulseRawData>(fac, objs) ) return true;
	if( JFactory_base_CopyToT<Df250TriggerTime>(fac, objs) ) return true;
	if( JFactory_base_CopyToT<Df250PulseTime>(fac, objs) ) return true;
	if( JFactory_base_CopyToT<Df250WindowRawData>(fac, objs) ) return true;
	if( JFactory_base_CopyToT<Df125PulseIntegral>(fac, objs) ) return true;
	if( JFactory_base_CopyToT<Df125TriggerTime>(fac, objs) ) return true;
	if( JFactory_base_CopyToT<Df125PulseTime>(fac, objs) ) return true;
	if( JFactory_base_CopyToT<DF1TDCHit>(fac, objs) ) return true;
	if( JFactory_base_CopyToT<DF1TDCTriggerTime>(fac, objs) ) return true;

	return false;
}

//----------------------------
// AddIfAppropriate
//----------------------------
template<class T>
void AddIfAppropriate(DDAQAddress *obj, vector<T*> &v)
{
	T *t = dynamic_cast<T*>(obj);
	if(t!= NULL) v.push_back(t);
}

//----------------------------
// LinkAssociationsWithPulseNumber
//----------------------------
template<class T, class U>
void LinkAssociationsWithPulseNumber(vector<T*> &a, vector<U*> &b)
{
	/// Template routine to loop over two vectors of pointers to
	/// objects derived from DDAQAddress. This will find any hits
	/// coming from the same DAQ channel and add each to the other's
	/// AssociatedObjects list. This will also check if the member
	/// "pulse_number" is the same (use LinkAssociations to not check
	/// the pulse_number such as when either "T" or "U" does not have
	/// a member named "pulse_number".)
	for(unsigned int j=0; j<a.size(); j++){
		for(unsigned int k=0; k<b.size(); k++){
			if(a[j]->pulse_number != b[k]->pulse_number) continue;
			if(*a[j] == *b[k]){ // compare only the DDAQAddress parts
				a[j]->AddAssociatedObject(b[k]);
				b[k]->AddAssociatedObject(a[j]);
			}
		}
	}
}

//----------------------------
// LinkAssociations
//----------------------------
template<class T, class U>
void LinkAssociations(vector<T*> &a, vector<U*> &b)
{
	/// Template routine to loop over two vectors of pointers to
	/// objects derived from DDAQAddress. This will find any hits
	/// coming from the same DAQ channel and add each to the other's
	/// AssociatedObjects list. This will NOT check if the member
	/// "pulse_number" is the same (use LinkAssociationsWithPulseNumber
	/// for that.)
	for(unsigned int j=0; j<a.size(); j++){
		for(unsigned int k=0; k<b.size(); k++){
			if(*a[j] == *b[k]){ // compare only the DDAQAddress parts
				a[j]->AddAssociatedObject(b[k]);
				b[k]->AddAssociatedObject(a[j]);
			}
		}
	}
}

//----------------------------
// LinkAssociationsModuleOnly
//----------------------------
template<class T, class U>
void LinkAssociationsModuleOnly(vector<T*> &a, vector<U*> &b)
{
	/// Template routine to loop over two vectors of pointers to
	/// objects derived from DDAQAddress. This will find any hits
	/// coming from the same DAQ module (channel number is not checked)
	/// When a match is found, the pointer from "a" will be added
	/// to "b"'s AssociatedObjects list. This will NOT do the inverse
	/// of adding "b" to "a"'s list. It is intended for adding a module
	/// level trigger time object to all hits from that module. Adding
	/// all of the hits to the trigger time object seems like it would
	/// be a little expensive with no real use case.
	for(unsigned int j=0; j<a.size(); j++){
		for(unsigned int k=0; k<b.size(); k++){
			if(a[j]->rocid != b[k]->rocid) continue;
			if(a[j]->slot != b[k]->slot) continue;

			b[k]->AddAssociatedObject(a[j]);
		}
	}
}

#endif // _JEventSourceGenerator_DAQ_

