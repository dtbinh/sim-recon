#ifndef _DKinFitParticle_
#define _DKinFitParticle_

#include <deque>
#include <math.h>

#include "TVector3.h"
#include "TLorentzVector.h"
#include "TMatrixDSym.h"

#include "DKinFitConstraints.h"

using namespace std;

enum DKinFitParticleType
{
	d_DetectedParticle = 0, 
	d_BeamParticle,
	d_TargetParticle,
	d_DecayingParticle, 
	d_MissingParticle
};

enum DKinFitPullType
{
	d_EPull = 0,
	d_PxPull,
	d_PyPull,
	d_PzPull,
	d_XxPull,
	d_XyPull,
	d_XzPull,
	d_TPull
};

// dCovarianceMatrix is owned by DKinFitter (DKinFitter is responsible for new/delete)
	//And for a given particle, it is probably shared amongst several DKinFitParticle objects (e.g. setup for different constraints/fits)
	//However, at the end of the fit, a unique copy is set for the object

class DKinFitParticle
{
	friend class DKinFitter;
	friend class DKinFitConstraint_P4;
	friend class DKinFitConstraint_Vertex;
	friend class DKinFitConstraint_Spacetime;

	public:

		inline int Get_PID(void) const{return dPID;}
		inline double Get_Energy(void) const{return sqrt(dMass*dMass + dMomentum.Mag2());}
		inline int Get_Charge(void) const{return dCharge;}
		inline TLorentzVector Get_P4(void) const{return TLorentzVector(dMomentum, Get_Energy());}
		inline TVector3 Get_Momentum(void) const{return dMomentum;}
		inline TVector3 Get_Position(void) const{return dSpacetimeVertex.Vect();}
		inline double Get_Mass(void) const{return dMass;}
		inline double Get_Beta(void) const{return dMomentum.Mag()/(Get_Energy());}
		inline double Get_Time(void) const{return dSpacetimeVertex.T();}
		inline double Get_ShowerEnergy(void) const{return dShowerEnergy;}
		inline double Get_PathLength(void) const{return dPathLength;}
		inline double Get_PathLengthUncertainty(void) const{return dPathLengthUncertainty;}
		inline const TMatrixDSym* Get_CovarianceMatrix(void) const{return dCovarianceMatrix;}
		inline TLorentzVector Get_SpacetimeVertex(void) const{return dSpacetimeVertex;}

		inline unsigned short int Get_VertexConstraintFlag(void) const{return dVertexConstraintFlag;}
		inline DKinFitParticleType Get_KinFitParticleType(void) const{return dKinFitParticleType;}
		inline bool Get_DecayingParticleAtProductionVertexFlag(void) const{return dDecayingParticleAtProductionVertexFlag;}

		inline deque<const DKinFitConstraint_VertexBase*> Get_CommonVertexAndOrTimeConstraints(void) const
		{
			return deque<const DKinFitConstraint_VertexBase*>(dCommonVertexAndOrTimeConstraints.begin(), dCommonVertexAndOrTimeConstraints.end());
		}
		inline size_t Get_NumVertexFits(void) const{return dCommonVertexAndOrTimeConstraints.size();};

		inline deque<const DKinFitConstraint_P4*> Get_P4Constraints(void) const
		{
			return deque<const DKinFitConstraint_P4*>(dP4Constraints.begin(), dP4Constraints.end());
		}
		inline size_t Get_NumP4Constraints(void) const{return dP4Constraints.size();}

		const DKinFitConstraint_P4* Get_DefinedAtP4Constraint(void) const;
		const DKinFitConstraint_P4* Get_ConstrainedAtP4Constraint(void) const;
		const DKinFitConstraint_P4* Get_P4ConstraintWhenInitial(void) const;

		const DKinFitConstraint_VertexBase* Get_DefinedAtVertexAndOrTimeConstraint(void) const;
		const DKinFitConstraint_VertexBase* Get_ConstrainedAtVertexAndOrTimeConstraint(void) const;

		inline int Get_PxParamIndex(void) const{return dPxParamIndex;}
		inline int Get_VxParamIndex(void) const{return dVxParamIndex;}
		inline int Get_TParamIndex(void) const{return dTParamIndex;}
		inline int Get_EParamIndex(void) const{return dEParamIndex;}
		inline int Get_LParamIndex(void) const{return dLParamIndex;}

		inline int Get_CovMatrixEParamIndex(void) const{return ((dCovarianceMatrix == NULL) ? -1 : ((dCovarianceMatrix->GetNcols() == 7) ? -1 : 0));}
		inline int Get_CovMatrixPxParamIndex(void) const{return ((dCovarianceMatrix == NULL) ? -1 : ((dCovarianceMatrix->GetNcols() == 7) ? 0 : -1));}
		inline int Get_CovMatrixVxParamIndex(void) const{return ((dCovarianceMatrix == NULL) ? -1 : ((dCovarianceMatrix->GetNcols() == 7) ? 3 : 1));}
		inline int Get_CovMatrixTParamIndex(void) const{return ((dCovarianceMatrix == NULL) ? -1 : ((dCovarianceMatrix->GetNcols() == 7) ? 6 : 4));}

		TVector3 Get_CommonVertex(void) const;
		double Get_CommonTime(void) const;
		TLorentzVector Get_CommonSpacetimeVertex(void) const;

		inline bool Get_IsInVertexOrSpacetimeFitFlag(void) const{return (!dCommonVertexAndOrTimeConstraints.empty());}
		bool Get_IsInVertexFitFlag(void) const;
		bool Get_IsInSpacetimeFitFlag(void) const;
		inline bool Get_IsInP4FitFlag(void) const{return (!dP4Constraints.empty());}
		inline bool Get_IsNeutralShowerFlag(void) const{return dIsNeutralShowerFlag;}

	private:

		DKinFitParticle(void);
		~DKinFitParticle(void){}

		inline void Set_PID(int locPID){dPID = locPID;}
		inline void Set_Charge(int locCharge){dCharge = locCharge;}
		inline void Set_Mass(double locMass){dMass = locMass;}
		inline void Set_Position(TVector3 locPosition){dSpacetimeVertex.SetVect(locPosition);}
		inline void Set_Time(double locTime){dSpacetimeVertex.SetT(locTime);}
		inline void Set_Momentum(TVector3 locMomentum){dMomentum = locMomentum;}
		inline void Set_CovarianceMatrix(TMatrixDSym* locCovarianceMatrix){dCovarianceMatrix = locCovarianceMatrix;}
		inline void Set_ShowerEnergy(double locShowerEnergy){dShowerEnergy = locShowerEnergy;}
		inline void Set_PathLength(double locPathLength){dPathLength = locPathLength;}
		inline void Set_PathLengthUncertainty(double locPathLengthUncertainty){dPathLengthUncertainty = locPathLengthUncertainty;}

		inline void Set_PxParamIndex(int locPxParamIndex){dPxParamIndex = locPxParamIndex;}
		inline void Set_VxParamIndex(int locVxParamIndex){dVxParamIndex = locVxParamIndex;}
		inline void Set_TParamIndex(int locTParamIndex){dTParamIndex = locTParamIndex;}
		inline void Set_EParamIndex(int locEParamIndex){dEParamIndex = locEParamIndex;}
		inline void Set_LParamIndex(int locLParamIndex){dLParamIndex = locLParamIndex;}

		inline void Set_IsNeutralShowerFlag(bool locIsNeutralShowerFlag){dIsNeutralShowerFlag = locIsNeutralShowerFlag;}
		inline void Set_VertexConstraintFlag(unsigned short int locVertexConstraintFlag){dVertexConstraintFlag = locVertexConstraintFlag;}
		inline void Add_CommonVertexAndOrTimeConstraint(DKinFitConstraint_VertexBase* locCommonVertexAndOrTimeConstraint){dCommonVertexAndOrTimeConstraints.push_back(locCommonVertexAndOrTimeConstraint);}
		inline void Set_CommonVertexAndOrTimeConstraints(deque<DKinFitConstraint_VertexBase*> locCommonVertexAndOrTimeConstraints){dCommonVertexAndOrTimeConstraints = locCommonVertexAndOrTimeConstraints;}
		inline void Add_P4Constraint(DKinFitConstraint_P4* locP4Constraint){dP4Constraints.push_back(locP4Constraint);}

		inline void Set_KinFitParticleType(DKinFitParticleType locKinFitParticleType){dKinFitParticleType = locKinFitParticleType;}
		inline void Set_DecayingParticleAtProductionVertexFlag(bool locDecayingParticleAtProductionVertexFlag){dDecayingParticleAtProductionVertexFlag = locDecayingParticleAtProductionVertexFlag;}

		void Reset(void);
		int Get_CommonVxParamIndex(void) const;
		int Get_CommonTParamIndex(void) const;

		int dPID; //PDG PID
		int dCharge;
		double dMass;
		//p, x, & t must all coincide: t & p at point x (for charged tracks p is a function of x in a b-field!)
		TLorentzVector dSpacetimeVertex;
		double dShowerEnergy;
		TVector3 dMomentum; //must be the value of the momentum at dSpacetimeVertex
		TMatrixDSym* dCovarianceMatrix; //is 7x7 for charged particles, and is either 7x7 or 5x5 for neutrals
		//7x7 format: px, py, pz, x, y, z, t
		//5x5 format: E, x, y, z, t

		double dPathLength; //kinfit if v & t constraint of charged particles in b-field, else calculated if a vertex is fit // = distance between input point and common point
		double dPathLengthUncertainty;

		unsigned short int dVertexConstraintFlag; //unused unless in vertex fit //can choose between equations //only for non-accelerating particles not constrained in time

		DKinFitParticleType dKinFitParticleType;

		int dEParamIndex;
		int dPxParamIndex;
		int dVxParamIndex;
		int dTParamIndex;
		int dLParamIndex;

		//true if the object's p3, v3, & t are defined at its production vertex (& common v & t are at decay vertex). else at it's decay vertex (& common v & t are at production vertex)
			//note: if a decaying particle is not in a vertex fit, then this quantity doesn't matter (default true)
		bool dDecayingParticleAtProductionVertexFlag;

		bool dIsNeutralShowerFlag;

		//used in time constraint to point to simultaneously constrained vertex so that can update vertex position
		deque<DKinFitConstraint_VertexBase*> dCommonVertexAndOrTimeConstraints; //can initially have more than one if a decaying particle (if so, the first one should be used)
		deque<DKinFitConstraint_P4*> dP4Constraints;
};

#endif // _DKinFitParticle_

