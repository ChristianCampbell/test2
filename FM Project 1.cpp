#include "maximilian.h"

maxiSample g, g2, g3;

maxiOsc myPhasor, myPhasor2, bassMod, FM1, FM2, FM3, Propeller, PropellerPhasor, PropMod;

maxiFilter filter;

maxiClock myClock;

int CurrentCount, CurrentCount2;

double gPlay, gPlay2, gPlay3, BassModVal,FMTick, FMTick2,FMOutput, FMOutput2, pitches, samplepitches, PropellerVal, PropellerPhasorVal, PropModVal, FMIntensity;

double controlpitches[16] = {65.4,77.78,97.999,103.83,65.4,77.78,97.999,103.83,48.99,58.27,97.999,103.83,65.4,77.78,97.999,103.83};

double csamplepitches[16] = {0.8,0.8,0.8,0.8,1,1,0.5,0.6,0.8,0.8,0.8,0.8,1,1,0.5,0.6};

maxiEnv myEnvelope;

//FILTER
float xs[3], ys[3];
float a0, a1, a2, b0, b1, b2;
float f0; //THE FREQUENCY
float Q;


void setup() {
    
    
    g.load("/Users/christian/Documents/3rd Year/Term 1/Advanced AV/Week 5/A Thing/g.wav");
    
    g2.load("/Users/christian/Documents/3rd Year/Term 1/Advanced AV/Week 5/A Thing/g copy.wav");
    
    g3.load("/Users/christian/Documents/3rd Year/Term 1/Advanced AV/Week 5/A Thing/g copy 2.wav");
    
    myClock.setTicksPerBeat(1);
    myClock.setTempo(60);
    
    
}

void play(double *output) {
    
    
    myClock.ticker();
    
    if (myClock.tick) {
        
        FMIntensity += 100;
        
        if (FMIntensity > 1500) {
            FMIntensity = 300;
        }
        
        
        FMTick += 0.5;
        
        if(FMTick > 8) {
            FMTick = 0;
        }
        
        CurrentCount += 1;
        
        if(CurrentCount > 15) {
            CurrentCount = 1;
        }
        
        pitches = controlpitches[CurrentCount];
        
        samplepitches = csamplepitches[CurrentCount];
        
    }
    
    FMTick2 = sin(FMTick);
    
    BassModVal = bassMod.sinewave(0.1);
    
    CurrentCount2=myPhasor2.phasor(BassModVal,1,9);
    
    PropellerPhasorVal=PropellerPhasor.phasor(2);
    
    
    if (CurrentCount >= 7) {
        
        gPlay2=g2.play(CurrentCount2);
        
        FMOutput = FM1.sinewave(pitches+(FM2.sinewave((FM3.sinewave(CurrentCount*100) * 1000)*pitches*200)*PropellerPhasorVal * FMIntensity));
        
        FMOutput2 = FMOutput * FMOutput;
        
        gPlay3=g3.play(BassModVal * CurrentCount2);
        
        gPlay=g.play(0.75);
        
    } else {
        
        
        gPlay3=g3.play(0.25);
        
        gPlay2=g2.play((CurrentCount2 + CurrentCount)/5);
        
        FMOutput = FM1.sinewave(pitches+(FM2.sinewave((FM3.sinewave(FMTick2*500) * pitches * 100)+1000)*PropellerPhasorVal* FMIntensity))*PropellerPhasorVal;
        
        FMOutput2 = FMOutput * FMOutput;
        
        gPlay=g.play(0.8);
        
    }
    
    PropellerVal = Propeller.noise();
    
    PropModVal = (PropMod.sinewave(1))+1;
    
    
    
    f0=30 + PropellerPhasorVal*400*PropModVal;
    Q=50;
    
    double w0 = 2*PI*f0/44100;
    double alpha = sin(w0)/(2*Q);
    
    b0 =  (1 - cos(w0))/2;
    b1 =   1 - cos(w0);
    b2 =  (1 - cos(w0))/2;
    a0 =   1 + alpha;
    a1 =  -2*cos(w0);
    a2 =   1 - alpha;
    
    
    
    xs[0] = PropellerVal;
    ys[0] = (b0/a0)*xs[0] + (b1/a0)*xs[1] + (b2/a0)*xs[2]
    - (a1/a0)*ys[1] - (a2/a0)*ys[2];
    
    
    
    ys[2] = ys[1]; ys[1] = ys[0];
    xs[2] = xs[1]; xs[1] = xs[0];
    
	
	output[0]= (3*gPlay*PropellerPhasorVal) + (3*gPlay2*PropellerPhasorVal) + (3*gPlay3*PropellerPhasorVal) + (0.15*FMOutput) + (0.13*ys[0]);
    //output[0]=(0.1*FMOutput);
    output[1]=(3*gPlay*PropellerPhasorVal) + (3*gPlay2*PropellerPhasorVal) + (3*gPlay3*PropellerPhasorVal) + (0.15*FMOutput2) + (0.13*ys[0]);
    
	
}

