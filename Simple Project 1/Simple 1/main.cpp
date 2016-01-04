#include "maximilian.h"

maxiSample g, g2, g3;

maxiOsc myPhasor, myPhasor2, bassMod, FM1, FM2, FM3, Propeller, PropellerPhasor, PropMod;

maxiFilter filter;

maxiClock myClock;

// Initialising variables

int CurrentCount, CurrentCount2;

double gPlay, gPlay2, gPlay3, BassModVal,FMTick, FMTick2,FMOutput, FMOutput2, pitches, samplepitches, PropellerVal, PropellerPhasorVal, PropModVal, FMIntensity;


// Pitches for the synthesizer
double controlpitches[16] = {65.4,77.78,97.999,103.83,65.4,77.78,97.999,103.83,48.99,58.27,97.999,103.83,65.4,77.78,97.999,103.83};


// Pitches for the samples
double csamplepitches[16] = {0.8,0.8,0.8,0.8,1,1,0.5,0.6,0.8,0.8,0.8,0.8,1,1,0.5,0.6};

maxiEnv myEnvelope;

//Tilter
float xs[3], ys[3];
float a0, a1, a2, b0, b1, b2;
float f0; //The Frequency
float Q;


void setup() {
    
    
    /*
        Loading in the three samples (you'll need to change these to where yours are, they are in the 'data'
        section of the 'Simple Project 1' folder
     */
    
    g.load("/Users/christian/Documents/3rd Year/Term 1/Advanced AV/Final Project/Simple Project 1/data/g.wav");
    
    g2.load("/Users/christian/Documents/3rd Year/Term 1/Advanced AV/Final Project/Simple Project 1/data/g copy.wav");
    
    g3.load("/Users/christian/Documents/3rd Year/Term 1/Advanced AV/Final Project/Simple Project 1/data/g copy 2.wav");
    
    myClock.setTicksPerBeat(1);
    // Tick set to once per second
    myClock.setTempo(60);
    
    
}

void play(double *output) {
    
    
    myClock.ticker();
    
    // Triggered each time the clock ticks
    if (myClock.tick) {
        
        
        // Intensity of the modulator for the FM is increased
        FMIntensity += 100;
        
        if (FMIntensity > 1500) {
            //Returns to 300 if the intensity goes above 1500
            FMIntensity = 300;
        }
        
        
        // Another FM modulator is increased each tick. This is later put through a sin function so it loops
        FMTick += 0.5;
        
        if(FMTick > 8) {
            
            // Returns back to 0 when it hits 8 (after 16 ticks)
            FMTick = 0;
        }
        
        
        // CurrentCount is the structural function for the piece. It dictates what is triggerend when
        CurrentCount += 1;
        
        if(CurrentCount > 15) {
            
            // When it is bigger than 15 (i.e 16) it returns back to 1
            CurrentCount = 1;
        }
        
        // This is where the arrays are controlled for the pitches
        
        pitches = controlpitches[CurrentCount];
        
        samplepitches = csamplepitches[CurrentCount];
        
    }
    
    FMTick2 = sin(FMTick);
    
    // This will modulate the pitch of the lowest sample played
    BassModVal = bassMod.sinewave(0.1);
    
    // Changes the pitch of different parts depending on the section played
    CurrentCount2=myPhasor2.phasor(BassModVal,1,9);
    
    // The level of the noise
    PropellerPhasorVal=PropellerPhasor.phasor(2);
    
    
    if (CurrentCount >= 7) {
        
        // Plays one sample at 0.75x its normal speed
        gPlay=g.play(0.75);
        
        // Plays one sample at the speed dictated by CurrentCount2
        gPlay2=g2.play(CurrentCount2);
        
        // Plays one sample using the BassModVal value as a modulator of CurrentCount2
        gPlay3=g3.play(BassModVal * CurrentCount2);
        
        /*
         This is the output for the FM sound. There is a lot of modulation within this funtion with three
         sine waves
         
         */
        FMOutput = FM1.sinewave(pitches+(FM2.sinewave((FM3.sinewave(CurrentCount*100) * 1000)*pitches*200)*PropellerPhasorVal * FMIntensity));
        
        // FMOutput 2 is for the left channel, whereas FMOutput is for the right
        FMOutput2 = FMOutput * FMOutput;
      
    } else {
        
        // Plays one sample at 0.8x its normal speed
        gPlay=g.play(0.8);
        
        // Plays one sample at the speed dictated by CurrentCount2 added to CurrenCount/5
        gPlay2=g2.play((CurrentCount2 + CurrentCount)/5);
        
        // Plays one sample at 0.25x its original speed
        gPlay3=g3.play(0.25);
        
        /*
         This is the output for the FM sound. There is a lot of modulation within this funtion with three
         sine waves
         
         */
        
        FMOutput = FM1.sinewave(pitches+(FM2.sinewave((FM3.sinewave(FMTick2*500) * pitches * 100)+1000)*PropellerPhasorVal* FMIntensity))*PropellerPhasorVal;
        
        // FMOutput 2 is for the left channel, whereas FMOutput is for the right
        FMOutput2 = FMOutput * FMOutput;
        
    }
    
    // Control for the windy noise sound produced
    PropellerVal = Propeller.noise();
    
    //changes the value of the top point of the cutoff each time it ticks
    PropModVal = (PropMod.sinewave(1))+1;
    
    
    
    // Low pass filter for the noise value, the frequency value changes on a ramp
    
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
    
	
	// Left output, the volumes are fixed
    output[0]= (3*gPlay*PropellerPhasorVal) + (3*gPlay2*PropellerPhasorVal) + (3*gPlay3*PropellerPhasorVal) + (0.15*FMOutput) + (0.13*ys[0]);
    
    // Right output, the volumes are fixed
    output[1]=(3*gPlay*PropellerPhasorVal) + (3*gPlay2*PropellerPhasorVal) + (3*gPlay3*PropellerPhasorVal) + (0.15*FMOutput2) + (0.13*ys[0]);
    
	
}

