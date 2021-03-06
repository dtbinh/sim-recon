//
// $Id$
//
//
// (See comments in fa125algo16.h)
//
// The #defines below might normally be placed in the header file, but
// these have some generic names and are intended only for this CDC
// timing algorithm. They may also be replaced with JANA configuration
// parameters and/or CCDB constants.
//
// The main entry point here is the fa125_algos routine. The samples
// are passed in along with references to many different return values.
// 

#include <stdlib.h>

#include <fa125algo.h>

#ifndef _DBG_
#define _DBG_ cout<<__FILE__<<":"<<__LINE__<<" "
#define _DBG__ cout<<__FILE__<<":"<<__LINE__<<endl
#endif


#define CDC_NW 180      //trigger window length (data buffer length)
#define CDC_WS 46       //hit window start
#define CDC_WE 150      //hit window end

#define CDC_H 125       // 5 sigma hit threshold
#define CDC_NP 16       // # samples used for pedestal used to find hit. 2**integer
#define CDC_NP2 16      // # samples used for pedestal calculated just before hit. 2**integer

#define CDC_TH 100      // 4 sigma threshold
#define CDC_TL 25       // 1 sigma threshold
#define CDC_RT 24       // if pulse fails QA, return this many tenth-samples before threshold xing
#define CDC_IS 6        // if pulse fails QA, start integration with this sample 
#define CDC_IE 200      // end integration at the earlier of WE, or this many samples after threshold crossing of TH  

#define CDC_LIMIT_PED_MAX 511   //return rough time if any sample in 0 to PED_SAMPLE exceeds this
#define CDC_LIMIT_ADC_MAX 4094  // return rough time if any sample in PED_SAMPLE+1 to NSAMPLES exceeds this

#define CDC_NU 15       //number of samples in subset of array to pass to cdc_time
#define CDC_XTHR 9      // the hit_thres xing sample is sample[9] passed into cdc_time
#define CDC_PED 5       // take local ped as sample[5] passed into cdc_time

#define CDC_SET_ADC_MIN 20      // add an offset to the adc values to set the min value equal to this
#define CDC_LIMIT_UPS_ERR 30    // upsampling error tolerance, return midpoint time if error is greater than this



#define FDC_NW 100      //trigger window length (data buffer length)
#define FDC_WS 30       //hit window start
#define FDC_WE 52       //hit window end

#define FDC_H 100       // 5 sigma hit threshold
#define FDC_NP 16       // # samples used for pedestal used to find hit. 2**integer
#define FDC_NP2 16      // # samples used for pedestal calculated just before hit. 2**integer

#define FDC_TH 80       // 4 sigma threshold
#define FDC_TL 20       // 1 sigma threshold
#define FDC_RT 24       // if pulse fails QA, return this many tenth-samples before threshold xing
#define FDC_IS 6        // if pulse fails QA, start integration with this sample 
#define FDC_IE 10       // end integration at the earlier of WE, or this many samples after threshold crossing of TH  

#define FDC_LIMIT_PED_MAX 511   //return rough time if any sample in 0 to PED_SAMPLE exceeds this
#define FDC_LIMIT_ADC_MAX 4094  // return rough time if any sample in PED_SAMPLE+1 to NSAMPLES exceeds this

#define FDC_NU 15       //number of samples in subset of array to pass to cdc_time
#define FDC_XTHR 9      // the hit_thres xing sample is sample[9] passed into cdc_time
#define FDC_PED 5       // take local ped as sample[5] passed into cdc_time

#define FDC_SET_ADC_MIN 20      // add an offset to the adc values to set the min value equal to this
#define FDC_LIMIT_UPS_ERR 30    // upsampling error tolerance, return midpoint time if error is greater than this


// FA125 emulation functions cdc_hit, cdc_time etc


  // cdc_time q_code values:
  //
  //   0: Good
  //   1: ADC data did not go over threshold adc_thres_hi 
  //   2: Leading edge time is outside the upsampled region (cross adc_thres_lo too late in the buffer subset ) 
  //   3: Last upsampled point is <= low timing threshold
  //   4: Upsampled points did not go below low timing threshold
  //   5: ADC sample value of 0 found
  //   6: ADC sample value > ADC_LIMIT found
  //   7: Pedestal ADC[PED_SAMPLE] value > LIMIT_PED_MAX found
  //   8: Upsampled point is below 0
  //   9: Upsampling error is > SET_UPS_TOL


  // calc pedestal as mean of NPED samples before trigger and again as mean of NPED2 samples before hit
  // hit search is from samples WINDOW_START to WINDOW_END
  // pedestal is not subtracted from max amplitude


// Wrapper for routine below
void fa125_algos(int rocid, vector<uint16_t> samples, fa125_algos_data_t &fa125_algos_data)
{
	// Since we would have to write a lot of "cdc_algos_data."'s, make another
	// reference that's smaller so we just have to write "d." instead
	fa125_algos_data_t &d = fa125_algos_data;

	// This was defined in the cdcmininewt.C macro as the following:
	//
	//	Int_t maxnsamples = CDC_NU;
	//	if (FDC_NU > maxnsamples) maxnsamples = FDC_NU;
	//	const Int_t NSAMPLES = maxnsamples;
	//
	// It seems like it should depend on it being a CDC or FDC hit but
	// the following line follows more closely with Naomi's original code.
	d.NSAMPLES = CDC_NU>FDC_NU ? CDC_NU:FDC_NU;

	bool cdchit = kFALSE;
	if ((rocid > 24)&&(rocid < 29)) cdchit = kTRUE;   //CDC has rocid 25 to 28

	if (cdchit) {

		d.WINDOW_START = CDC_WS;
		d.WINDOW_END = CDC_WE;

		d.HIT_THRES = CDC_H;
		d.NPED = CDC_NP;
		d.NPED2 = CDC_NP2;

		d.HIGH_THRESHOLD = CDC_TH;
		d.LOW_THRESHOLD = CDC_TL;
		d.ROUGH_DT = CDC_RT;
		d.INT_SAMPLE = CDC_IS;
		d.INT_END = CDC_IE;

		d.LIMIT_PED_MAX = CDC_LIMIT_PED_MAX;
		d.LIMIT_ADC_MAX = CDC_LIMIT_ADC_MAX;

		d.XTHR_SAMPLE = CDC_XTHR;
		d.PED_SAMPLE = CDC_PED;

		d.SET_ADC_MIN = CDC_SET_ADC_MIN;
		d.LIMIT_UPS_ERR = CDC_LIMIT_UPS_ERR; 

	} else {

		d.WINDOW_START = FDC_WS;
		d.WINDOW_END = FDC_WE;

		d.HIT_THRES = FDC_H;
		d.NPED = FDC_NP;
		d.NPED2 = FDC_NP2;

		d.HIGH_THRESHOLD = FDC_TH;
		d.LOW_THRESHOLD = FDC_TL;
		d.ROUGH_DT = FDC_RT;
		d.INT_SAMPLE = FDC_IS;
		d.INT_END = FDC_IE;

		d.LIMIT_PED_MAX = FDC_LIMIT_PED_MAX;
		d.LIMIT_ADC_MAX = FDC_LIMIT_ADC_MAX;

		d.XTHR_SAMPLE = FDC_XTHR;
		d.PED_SAMPLE = FDC_PED;

		d.SET_ADC_MIN = FDC_SET_ADC_MIN;
		d.LIMIT_UPS_ERR = FDC_LIMIT_UPS_ERR; 

	}


	if(samples.size()<=(uint32_t)d.WINDOW_END){
		cout << "The number of samples passed into the fa125_algos routine is less than the" << endl;
		cout << "minimum (" << samples.size() << " <= " << d.WINDOW_END << "). The code is" << endl;
		cout << "currently not capable of handling this. " << endl;
		exit(-1);
	}
	
	// Copy uint16_t samples into Int_t type array so we can pass it into the cdc_algos2
	// routine that does the actual work
	Int_t adc[d.WINDOW_END+1];
	for(uint32_t i=0; i<=(uint32_t)d.WINDOW_END; i++) adc[i] = (Int_t)samples[i];
	
	// Call the actual routine that does the heavy lifting
	fa125_algos(d.time, d.q_code, d.pedestal, d.integral, d.overflows, d.maxamp, adc, d.NSAMPLES, d.WINDOW_START, d.WINDOW_END, d.HIT_THRES, d.NPED, d.NPED2, d.HIGH_THRESHOLD, d.LOW_THRESHOLD, d.ROUGH_DT, d.INT_SAMPLE, d.INT_END, d.LIMIT_PED_MAX, d.LIMIT_ADC_MAX, d.XTHR_SAMPLE, d.PED_SAMPLE, d.SET_ADC_MIN, d.LIMIT_UPS_ERR);

}



void fa125_algos(Int_t &time, Int_t &q_code, Int_t &pedestal, Long_t &integral, Int_t &overflows, Int_t &maxamp, Int_t adc[], const Int_t NSAMPLES, Int_t WINDOW_START, Int_t WINDOW_END, Int_t HIT_THRES, Int_t NPED, Int_t NPED2, Int_t HIGH_THRESHOLD, Int_t LOW_THRESHOLD, Int_t ROUGH_DT, Int_t INT_SAMPLE, Int_t INT_END, Int_t LIMIT_PED_MAX, Int_t LIMIT_ADC_MAX, Int_t XTHR_SAMPLE, Int_t PED_SAMPLE, Int_t SET_ADC_MIN, Int_t LIMIT_UPS_ERR) {

 
  Int_t adc_subset[NSAMPLES]; 

  Int_t hitfound=0; //hit found or not (1=found,0=not)
  Int_t hitsample=-1;  // if hit found, sample number of threshold crossing

  Int_t integral1=0;   // signal integral, from le time to threshold crossing
  Long_t integral2=0;   // signal integral, from threshold crossing to end of signal

  Int_t i=0;

  time=0;       // hit time in 0.1xsamples since start of buffer passed to cdc_time
  q_code=-1;    // quality code, 0=good, 1=returned rough estimate
  pedestal=0;   // pedestal just before hit
  integral=0;   // signal integral, total
  overflows=0;  // count of samples with overflow bit set (need raw data, not possible from my root files)
  maxamp=0;     // signal amplitude at first max after hit


  // look for hit using mean pedestal of 16 samples before trigger 
  cdc_hit(hitfound,hitsample,pedestal,adc, WINDOW_START, WINDOW_END, HIT_THRES, NPED, NPED2, XTHR_SAMPLE, PED_SAMPLE);


  if (hitfound==1) {

    for (i=0; i<NSAMPLES; i++) {
      adc_subset[i] = adc[hitsample+i-XTHR_SAMPLE];
    }

    cdc_time(time, q_code, integral1, adc_subset, NSAMPLES, HIGH_THRESHOLD, LOW_THRESHOLD, ROUGH_DT, INT_SAMPLE, LIMIT_PED_MAX, LIMIT_ADC_MAX, XTHR_SAMPLE, PED_SAMPLE, SET_ADC_MIN, LIMIT_UPS_ERR);

    cdc_integral(integral2, overflows, hitsample, adc, WINDOW_END, INT_END);

    cdc_max(maxamp, hitsample, adc, WINDOW_END);

    time = 10*(hitsample-XTHR_SAMPLE) + time;   // integer number * 0.1 samples

    integral = (Long_t)integral1 + integral2;

  }

}




void cdc_hit(Int_t &hitfound, Int_t &hitsample, Int_t &ped, Int_t adc[], Int_t WINDOW_START, Int_t WINDOW_END, Int_t HIT_THRES, Int_t NPED, Int_t NPED2, Int_t XTHR_SAMPLE, Int_t PED_SAMPLE) {


  ped=0;  //pedestal
  Int_t threshold=0;

  Int_t i=0;

  // calc pedestal as mean of NPED samples before trigger
  for (i=0; i<NPED; i++) ped += adc[WINDOW_START-NPED+i];

  ped = ped/NPED;   // Integer div is ok as fpga will do 2 rightshifts

  threshold = ped + HIT_THRES;

  // look for threshold crossing
  i = WINDOW_START - 1;
  hitfound = 0;

  while ((hitfound==0) && (i<WINDOW_END)) {

    i++;

    if (adc[i] >= threshold) {

      hitfound = 1;
      hitsample = i;

    }
  }

  if (hitfound == 1) {

    //calculate new pedestal ending just before the hit

    ped = 0;

    for (i=0; i<NPED2; i++) {
      ped += adc[hitsample-XTHR_SAMPLE+PED_SAMPLE-i];
    }

    ped = ped/NPED2;
  }


}




void cdc_integral(Long_t& integral, Int_t& overflows, Int_t hitsample, Int_t adc[], Int_t WINDOW_END, Int_t INT_END) {

  Int_t i=0;

  integral = 0;
  overflows = 0;

  Int_t lastsample = hitsample + INT_END;

  if (lastsample > WINDOW_END) lastsample = WINDOW_END;

  for (i = hitsample; i <= lastsample; i++ ) {

    integral += (Long_t)adc[i];
    if (adc[i]>4095) overflows++;    // only a placeholder at present. need to test on sample's overflow bit

  }


}




void cdc_max(Int_t& maxamp, Int_t hitsample, Int_t adc[], Int_t WINDOW_END) {

  maxamp = 0;

  Int_t maxbin=0;

  maxbin = hitsample;

  while ( (adc[maxbin]<=adc[maxbin+1]) && (maxbin <= WINDOW_END ) ){
    maxbin++;
  }

  maxamp = adc[maxbin];

}






void cdc_time(Int_t &le_time, Int_t &q_code, Int_t &integral, Int_t adc[], Int_t NSAMPLES, Int_t HIGH_THRESHOLD, Int_t LOW_THRESHOLD, Int_t ROUGH_DT, Int_t INT_SAMPLE, Int_t LIMIT_PED_MAX, Int_t LIMIT_ADC_MAX, Int_t XTHR_SAMPLE, Int_t PED_SAMPLE, Int_t SET_ADC_MIN, Int_t LIMIT_UPS_ERR) {


  // returned quantities:

  //  Int_t le_time = 0;  // leading edge time as 0.1 samples since time of first sample supplied
  //  Int_t q_code = 0;   // quality code, 0=good, 1=rough estimate as data did not exceed threshold

  // q_code list:
  //   0: Good
  //   1: ADC data did not go over threshold adc_thres_hi 
  //   2: Leading edge time is outside the upsampled region (cross adc_thres_lo too late in the buffer subset ) 
  //   3: Last upsampled point is <= low timing threshold
  //   4: Upsampled points did not go below low timing threshold
  //   5: ADC sample value of 0 found
  //   6: ADC sample value > LIMIT_ADC_MAX found
  //   7: Pedestal ADC[PED_SAMPLE] value > LIMIT_PED_MAX found
  //   8: Upsampled point is below 0
  //   9: Difference between upsampled and sampled values > LIMIT_UPS_ERR

  // Input:
  //  Int_t adc[NSAMPLES] = {65,62,56,46,41,56,85,109,120,122,127,150,181,197};
  //  defined as Int_t to save type casts later on


  // config constants, defined above as globals

  //#define NSAMPLES 15;    //number of samples to pass to cdc_time
  //#define XTHR_SAMPLE 9;  // the 5 sigma thres xing is sample[8] passed into cdc_time, starting with sample[0]
  //#define PED_SAMPLE 5;   // take local ped as sample[4] passed into cdc_time

  //#define HIGH_THRESHOLD 64;    // 4 sigma
  //#define LOW_THRESHOLD 16;    // 1 sigma
  //#define ROUGH_DT 20;    // if algo fails, return this many tenth-samples before threshold xing
  //#define NUPSAMPLED 8;   // number of upsampled values to calculate
  //#define INT_SAMPLE 6; // if algo fails start integration with this sample 


  // internal constants

  const Int_t NUPSAMPLED = 8;       // number of upsampled values to calculate, minimum is 8

  const Int_t START_SEARCH = PED_SAMPLE+1; // -- start looking for hi threshold xing with this sample

  const Int_t ROUGH_TIME = (XTHR_SAMPLE*10)-ROUGH_DT; // --return this for time if the algo fails

  Int_t iubuf[NUPSAMPLED] = {0};  // array of upsampled values
   
  //	-- iubuf 0 corresponds to 0.2 before low thres xing sample
  //	-- iubuf 1 maps to low thres xing sample


  Int_t adc_thres_hi = 0; // high threshold
  Int_t adc_thres_lo = 0; // low threshold

  //    -- contributions to hit time, these are summed together eventually, units of sample/10

  Int_t itime1 = 0; // which sample
  Int_t itime2 = 0; // which minisample
  Int_t itime3 = 0; // correction from interpolation
    

  //    -- search vars

  Int_t adc_sample_hi = 0; // integer range 0 to NSAMPLES := 0;  --sample number for adc val at or above hi thres
  Int_t adc_sample_lo = 0; // integer range 0 to NSAMPLES := 0;  -- sample num for adc val at or below lo thres
  Int_t adc_sample_lo2 = 0; // integer range 0 to 12:= 0;  -- minisample num for adc val at or below lo thres

  Bool_t over_threshold = kFALSE;
  Bool_t below_threshold = kFALSE;


  // interpolation vars

  Int_t denom = 0; 
  Int_t limit = 0;
  Int_t sum = 0;
  Int_t ifrac = 0;
  
  // upsampling checks
  Int_t ups_err1 = 0;
  Int_t ups_err2 = 0;
  Int_t ups_err_sum = 0;
  Int_t ups_adjust = 0;


  Int_t i = 0;



  //check all samples are >0

  Bool_t adczero = kFALSE;

  i = 0;
  while ((!adczero)&&(i<NSAMPLES)) {

    if (adc[i] == 0) {
      adczero = kTRUE;
    }

    i++;
  }


  if (adczero) {

    le_time = ROUGH_TIME;
    q_code = 5;

    integral = 0;    

    for (i=INT_SAMPLE; i<XTHR_SAMPLE; i++) integral += adc[i];

    return;
   
  }


  //check all samples are <= LIMIT_ADC_MAX

  Bool_t adclimit = kFALSE;

  i = 0;
  while ((!adclimit)&&(i<NSAMPLES)) {

    if (adc[i] > LIMIT_ADC_MAX) {
      adclimit = kTRUE;
    }

    i++;
  }


  if (adclimit) {

    le_time = ROUGH_TIME;
    q_code = 6;

    integral = 0;    

    for (i=INT_SAMPLE; i<XTHR_SAMPLE; i++) integral += adc[i];

    return;
   
  }


  //check all samples from 0 to pedestal are <= LIMIT_PED_MAX

  Bool_t pedlimit = kFALSE;

  i = 0;
  while ((!pedlimit)&&(i<PED_SAMPLE+1)) {

    if (adc[i] > LIMIT_PED_MAX) {
      pedlimit = kTRUE;
    }

    i++;
  }



  if (pedlimit) {

    le_time = ROUGH_TIME;
    q_code = 7;

    integral = 0;    

    for (i=INT_SAMPLE; i<XTHR_SAMPLE; i++) integral += adc[i];

    return;
   
  }

  //  add offset to move min val in subset equal to SET_ADC_MIN
  //  this is to move samples away from 0 to avoid upsampled pts going -ve (on a curve betw 2 samples)

  Int_t adcmin = 4095; 

  i=0; 

  while (i<NSAMPLES) {

    if (adc[i] < adcmin) {
      adcmin = adc[i];
    }

    i++;
  }

  Int_t adcoffset = SET_ADC_MIN - adcmin;  

  i=0; 

  while (i<NSAMPLES) {
    adc[i] = adc[i] + adcoffset;
    i++;
  }

  // eg if adcmin is 100, setmin is 30, adcoffset = 30 - 100 = -70, move adc down by 70
 

  //////////////////////////////

  // calc thresholds

  adc_thres_hi = adc[PED_SAMPLE] + HIGH_THRESHOLD;
  adc_thres_lo = adc[PED_SAMPLE] + LOW_THRESHOLD;

  // search for high threshold crossing

  over_threshold = kFALSE;
  i = START_SEARCH;

  while ((!over_threshold)&&(i<NSAMPLES)) {

    if (adc[i] >= adc_thres_hi) {
      adc_sample_hi = i;
      over_threshold = kTRUE;
    }

    i++;
  }


  if (!over_threshold) {

    le_time = ROUGH_TIME;
    q_code = 1;

    integral = 0;    

    for (i=INT_SAMPLE; i<XTHR_SAMPLE; i++) integral += adc[i];

    return;
   
  }


  // search for low threshold crossing

  below_threshold = kFALSE;
  i = adc_sample_hi-1;

  while ((!below_threshold) && (i>=PED_SAMPLE)) {   //************changed START_SEARCH to PED_SAMPLE**********

    if (adc[i] <= adc_thres_lo) {
      adc_sample_lo = i;
      itime1 = i*10;
      below_threshold = kTRUE;
    }

    i--;
  }

  if (adc_sample_lo > NSAMPLES-7) {

    le_time = ROUGH_TIME;
    q_code = 2;

    integral = 0;    

    for (i=INT_SAMPLE; i<XTHR_SAMPLE; i++) integral += adc[i];

    return;
   
  }

          

  //upsample values from adc_sample_lo - 0.2 to adc_sample_lo + 1.2 

  upsamplei(adc, adc_sample_lo, iubuf, NUPSAMPLED);



  //check upsampled values are >0

  Bool_t negups = kFALSE;
  
  i=0;
  while ((!negups)&&(i<NUPSAMPLED)) {

    if (iubuf[i] < 0 ) {
      negups = kTRUE;
    }

    i++;
  }


  if (negups) {

    le_time = itime1 + 5;   // better bail-out //midway between adc_sample_lo and _lo + 1

    q_code = 8;
 
    integral = 0;    

          
    for (i=PED_SAMPLE-1; i<XTHR_SAMPLE; i++) {
      if (i>adc_sample_lo) integral += adc[i];
    }

    return;
   
  }




  // correct errors

  ups_err1 = iubuf[1] - adc[adc_sample_lo];
  ups_err2 = iubuf[6] - adc[adc_sample_lo+1];

  ups_err_sum = ups_err1 + ups_err2;

  ups_adjust = (Int_t)(0.5*ups_err_sum);


  // if this is more than set limit, bail out

  if (ups_err_sum > LIMIT_UPS_ERR) { //upsampled 

    le_time = itime1 + 5;   // better bail-out //midway between adc_sample_lo and _lo + 1

    q_code = 9;

    integral = 0;    
       
    for (i=PED_SAMPLE-1; i<XTHR_SAMPLE; i++) {
      if (i>adc_sample_lo) integral += adc[i];
    }

    return;
  }






  // move threshold correspondingly instead of correcting upsampled values

  adc_thres_lo = adc_thres_lo + ups_adjust;

  //iubuf(0) is at adc_sample_lo - 0.2
  //search through upsampled array


  if (iubuf[NUPSAMPLED-1]<= adc_thres_lo) { //bad upsampling

    le_time = itime1 + 5;   //midway

    q_code = 3;

    integral = 0;    
       
    for (i=PED_SAMPLE-1; i<XTHR_SAMPLE; i++) {
      if (i>adc_sample_lo) integral += adc[i];
    }

    return;
  }





  below_threshold = kFALSE;
  i = NUPSAMPLED-2;

  while ((!below_threshold) && (i>=0)) {

    if (iubuf[i] <= adc_thres_lo) {
      adc_sample_lo2 = i;
      below_threshold = kTRUE;
    }

    i--;
  }


  /************ bug fix ************/
  /*** do not trust upsampling completely **/

  if (!below_threshold) { //upsampled points did not go below thres

    le_time = itime1 + 5;   //midway

    q_code = 4;

    integral = 0;    
       
    for (i=PED_SAMPLE-1; i<XTHR_SAMPLE; i++) {
      if (i>adc_sample_lo) integral += adc[i];
    }

    return;
  }




  itime2 = adc_sample_lo2*2;  //  convert from sample/5 to sample/10



  //interpolate

  itime3 = 0;
            
  if (iubuf[adc_sample_lo2] != adc_thres_lo) {
                        
    denom = iubuf[adc_sample_lo2+1] - iubuf[adc_sample_lo2];
    limit = (adc_thres_lo - iubuf[adc_sample_lo2])*2;

    sum = 0;
    ifrac = 0;
                          
    while (sum<limit) {

      sum = sum + denom;
      ifrac = ifrac + 1;           

    }

    if (2*(sum-limit) > denom) { //  --round
      itime3 = ifrac - 1;        
    } else {
      itime3 = ifrac;
    }

  }


  //--              need to subtract 1/5 sample from itime2 because upsampling starts
  //--              1 minisample before adc_sample_lo
     
  le_time = itime1 - 2 + itime2 + itime3;  //   -- this is time from first sample point, in 1/10ths of samples
  q_code = 0;
  integral = 0;
          
  for (i=PED_SAMPLE-1; i<XTHR_SAMPLE; i++) {
    if (i>adc_sample_lo) integral += adc[i];
  }


}


void upsamplei(Int_t x[], Int_t startpos, Int_t z[], const Int_t NUPSAMPLED) {

  // x is array of samples
  // z is array of upsampled data
  // startpos is where to start upsampling in array x, only need to upsample a small region


  const Int_t nz = NUPSAMPLED;

  const Int_t K[43]={-8,-18,-27,-21,10,75,165,249,279,205,-2,-323,-673,-911,-873,-425,482,1773,3247,4618,5591,5943,5591,4618,3247,1773,482,-425,-873,-911,-673,-323,-2,205,279,249,165,75,10,-21,-27,-18,-8}; //32768

  Int_t k,j,dk;

  const Int_t Kscale = 32768;

  //don't need to calculate whole range possible
  //earliest value k=42 corresponds to sample 4.2
  //               k=43                sample 4.4
  //               k=46                sample 5.0
  
  // I need my first value to be for sample startpos - 0.2

  // sample 4 (if possible) would be at k=41
  // sample 4.2                         k=42
  // sample 5                           k=46

  // sample x                           k=41 + (x-4)*5
  // sample x-0.2                       k=40 + (x-4)*5


  Int_t firstk = 40 + (startpos-4)*5;


  for (k=firstk; k<firstk+nz; k++) {

    dk = k - firstk;    

    z[dk]=0.0;

    for (j=k%5;j<43;j+=5) {

      z[dk] += x[(k-j)/5]*K[j]; 

    }

    //    printf("dk %i z %i 5z %i  5z/scale %i\n",dk,z[dk],5.0*z[dk],5.0*z[dk]/Kscale);

    z[dk] = (Int_t)(5*z[dk])/Kscale;

  }

}
