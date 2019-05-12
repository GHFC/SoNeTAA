#ifndef _THREADED_OBJECT
#define _THREADED_OBJECT

#include "DAQ.h"
#include "ofMain.h"
#include "VPI.h"

#define metronomeL 3000
#define transcient1L 1000
#define transcient2L 1000
#define behaviorL 4096

class threadedObject : public ofThread{

	public:
        float humPos, humVel, hum2Pos;
        float intervalPos[2][2];
	    float timeStep;
	    int timeStart, lastTime, lastBip;
        VPI agent;
        bool HUDFlag,TRAJFlag,trialStart,metronome;
        ofTrueTypeFont HUDFont, HUDFontBig;
        ofstream trialDataFile, trialReportFile;
        int trialNum;
        int HUMpos[480];
		int VPIpos[480];
		int currentStep;
		int currentModel;

		bool automaticMode;
		bool switch1,switch2,switch3;
        bool report;
        int question;
        bool trigFlag;
        bool turing;

        ofImage robot,human;

		threadedObject(){
			HUDFont.loadFont("ressources/Minecraftia.ttf",10, true, true);
			HUDFontBig.loadFont("ressources/Minecraftia.ttf",20, true, true);
			HUDFlag=false;
			TRAJFlag=false;
			trialStart=false;
			trialNum=0;
			currentModel=1;
			agent=VPI(currentModel);
			automaticMode=true;
			turing=false;
			report=false;
			question=1;
			trigFlag=false;
			intervalPos[0][0]=9999;
            intervalPos[0][1]=-9999;
            intervalPos[1][0]=9999;
            intervalPos[1][1]=-9999;
            human.loadImage("ressources/human.png");
            robot.loadImage("ressources/robot.png");
		}

		void start(){
            startThread(true, false);   // blocking, verbose
            initDAQ();
        }

        void stop(){
            if (trialDataFile.is_open()){
                trialDataFile.close();
            }
            if (trialReportFile.is_open()){
                trialReportFile.close();
            }
            stopThread();
        }

		//--------------------------
		void threadedFunction(){
			while( isThreadRunning() != 0 ){
				if( lock() ){
				    //Gather input to National Instrument card
                    float humPosN=(readDAQ(1)-intervalPos[0][0])/(intervalPos[0][1]-intervalPos[0][0])*2-1.0;
                    humVel=(humPosN-humPos)/(2*timeStep);
                    float hum2PosN=(readDAQ(2)-intervalPos[1][0])/(intervalPos[1][1]-intervalPos[1][0])*2-1.0;
                    float hum2Vel=(hum2PosN-hum2Pos)/(2*timeStep);
                    humPos=humPosN;
                    hum2Pos=hum2PosN;

                    //If we are in a trial
				    if(trialStart&&!report){
				        sleep(1);

				        // VP update
				        int deltaTime = ofGetElapsedTimeMillis()-lastTime;
                        timeStep= (float)deltaTime/1000.0;
                        float K=agent.update(humPos,humVel,timeStep);

                        // Record the data
                        trialDataFile << lastTime-timeStart << "\t" << agent.x << "\t" << agent.xd << "\t" << humPos << "\t" << humVel << "\t" << K << "\n";
                        //In case with two Goniometers
                        //trialDataFile << lastTime-timeStart << "\t" << agent.x << "\t" << agent.xd << "\t" << humPos << "\t" << humVel << hum2Pos << "\t" << hum2Vel << "\n";
                        //In case output to National Instrument card needed
                        //writeDAQ(sin(ofGetElapsedTimeMillis()/1000./2.*PI), cos(ofGetElapsedTimeMillis()/1000./2.*PI));

                        //Timing
                        lastTime=ofGetElapsedTimeMillis();

                        // Baseline end
                        if ((lastTime-timeStart)>=metronomeL) {
                            if(metronome){
                                trigger(1);trigFlag=true;
                            }
                            metronome=false;
                        }

                        if(automaticMode){
                            // Vision
                            if(((lastTime-timeStart)>(metronomeL+transcient1L))&&switch1){
                               trigger(1);trigFlag=true;
                               switch1=false;switch2=true;
                            }

                            // Behavior transition
                            if(((lastTime-timeStart)>(metronomeL+transcient1L+behaviorL))&&switch2){
                               trigger(1);trigFlag=true;
                               if(agent.transition==1){
                                  agent.flip();
                               }
                               switch2=false;switch3=true;
                            }

                            if(((lastTime-timeStart)>(metronomeL+transcient1L+behaviorL+behaviorL))&&switch3){
                               trigger(1);trigFlag=true;
                               switch3=false;
                            }

                            // Trisal end
                            if((lastTime-timeStart)>(metronomeL+transcient1L+behaviorL+behaviorL+transcient2L)){
                               trigger(1);trigFlag=true;
                               trialDataFile.close();
                               trialStart=false;
                               if(automaticMode){
                                    report=true;
                                }else{
                                    trialReportFile.close();
                                }
                            }
                        }

                        // Trajectory update
                        VPIpos[currentStep%480]=agent.x*100;
                        HUMpos[currentStep%480]=humPos*320;
                        currentStep++;
                    }
					unlock();
				}
			}
		}

		//--------------------------
		void draw(){
			if( lock() ){
                ofSetColor(128);ofFill();
			    // Global debuging user interface
			    if(HUDFlag){
			        int line = 2;
					switch (agent.model) {
						case 1:
							HUDFont.drawString("HKB", 20, line*20);line++;
							HUDFont.drawString("alpha: "+ofToString(agent.alpha), 20, line*20);line++;
							HUDFont.drawString("beta: "+ofToString(agent.beta), 20, line*20);line++;
							HUDFont.drawString("gamma: "+ofToString(agent.gamma), 20, line*20);line++;
							HUDFont.drawString("A: "+ofToString(agent.A), 20, line*20);line++;
							HUDFont.drawString("B: "+ofToString(agent.B), 20, line*20);line++;
							break;
						case 2:
							HUDFont.drawString("Excitator", 20, line*20);line++;
							HUDFont.drawString("alpha: "+ofToString(agent.alpha), 20, line*20);line++;
							HUDFont.drawString("beta: "+ofToString(agent.beta), 20, line*20);line++;
							HUDFont.drawString("A: "+ofToString(agent.A), 20, line*20);line++;
							HUDFont.drawString("B: "+ofToString(agent.B), 20, line*20);line++;
							HUDFont.drawString("tau: "+ofToString(agent.tau), 20, line*20);line++;
							HUDFont.drawString("gain: "+ofToString(agent.gain), 20, line*20);line++;
							break;
						case 3:
							HUDFont.drawString("Excitator 2", 20, line*20);line++;
							HUDFont.drawString("alpha: "+ofToString(agent.alpha), 20, line*20);line++;
							HUDFont.drawString("beta: "+ofToString(agent.beta), 20, line*20);line++;
							HUDFont.drawString("A: "+ofToString(agent.A), 20, line*20);line++;
							HUDFont.drawString("B: "+ofToString(agent.B), 20, line*20);line++;
							HUDFont.drawString("tau: "+ofToString(agent.tau), 20, line*20);line++;
							HUDFont.drawString("gain: "+ofToString(agent.gain), 20, line*20);line++;
							HUDFont.drawString("kappa: "+ofToString(agent.kappa), 20, line*20);line++;
							HUDFont.drawString("lambda: "+ofToString(agent.lambda), 20, line*20);line++;
							break;
						default:
							break;
					}
                    line++;
                    if(turing){
                        HUDFont.drawString("Human ("+ofToString(hum2Pos)+")", 20, line*20);line++;
                    }else{
                        if(agent.mu==-1){
                            HUDFont.drawString("Reversed VPI", 20, line*20);line++;
                        }else{
                            HUDFont.drawString("Normal VPI", 20, line*20);line++;
                        }
                    }
                    line++;
                    HUDFont.drawString("omega: "+ofToString(agent.om/(2*PI))+"/"+ofToString(agent.omega/(2*PI)), 20, line*20);line++;
                    HUDFont.drawString("x: "+ofToString(agent.x), 20, line*20);line++;
                    HUDFont.drawString("xd: "+ofToString(agent.xd), 20, line*20);line++;
                    line++;line++;
                    HUDFont.drawString("Timestep: "+ofToString(timeStep), 20, line*20);line++;
                    HUDFont.drawString("Framerate: "+ofToString(ofGetFrameRate()), 20, line*20);line++;
                    if(automaticMode){
                        HUDFont.drawString("Automatic Mode", 20, line*20);line++;
                    }else{
                        HUDFont.drawString("Normal Mode", 20, line*20);line++;
                    }
                    if(agent.runge){
                        HUDFont.drawString("Runge-Kutta methods", 20, line*20);line++;
                    }else{
                        HUDFont.drawString("Euler methods", 20, line*20);line++;
                    }
                    if((agent.task==1)||(agent.task==3)){
                        HUDFont.drawString("Task #"+ofToString(trialNum)+": in-phase", 20, line*20);line++;
                    }else if((agent.task==2)||(agent.task==4)){
                        HUDFont.drawString("Task #"+ofToString(trialNum)+": anti-phase", 20, line*20);line++;
                    }else{
                        HUDFont.drawString("No task", 20, line*20);line++;
                    }
			    }
                string msg;
			    // In automatic mode
			    if(automaticMode){
                    // Instruction during baseline
                    if(metronome){
                        if((agent.task==1)||(agent.task==3)){
                            msg="In-phase";
                        }else if((agent.task==2)||(agent.task==4)){
                            msg="Anti-phase";
                        }else if(agent.task==0){
                            msg="?";
                        }else if(agent.task==-1){
                            msg="+";
                        }
                        HUDFontBig.drawString(msg, ofGetWindowWidth()/2-HUDFontBig.stringWidth(msg)/2, ofGetWindowHeight()/2+HUDFontBig.stringHeight(msg)/4);
                    }else{
                        if(!trialStart){
                            ofSetColor(128);ofFill();
                            if(agent.task==-1){
                                msg="End of the experiment.";
                            }else{
                                if((trialNum%10==0)&&(trialNum>0)&&!report){
                                    msg="End of the block.";
                                }else{
                                    msg="+";
                                }
                            }
                            HUDFontBig.drawString(msg, ofGetWindowWidth()/2-HUDFontBig.stringWidth(msg)/2, ofGetWindowHeight()/2+HUDFontBig.stringHeight(msg)/2);
                        }else{
                            /*ofSetColor(0,128);ofFill();
                            if((agent.task==1)||(agent.task==3)){
                                msg="I";
                            }else if((agent.task==2)||(agent.task==4)){
                                msg="A";
                            }
                            HUDFontBig.drawString(msg, ofGetWindowWidth()/2-18-HUDFontBig.stringWidth(msg)/2, ofGetWindowHeight()/2-110+HUDFontBig.stringHeight(msg)/2);*/
                        }
                    }

                    // Subject report at the end of the trial
                    if(report&&!(agent.task==-1)&&!(agent.task==0)){
                        string task;
                        if(agent.task==1){
                                task="in-phase";
                            }else if(agent.task==2){
                                task="anti-phase";
                            }
                        switch(question){
							case 1:
                                msg="Your task was "+task+".";
                                HUDFontBig.drawString(msg, ofGetWindowWidth()/2-HUDFontBig.stringWidth(msg)/2, ofGetWindowHeight()/2-100);
                                msg="Early on, the partner was:";
                                HUDFontBig.drawString(msg, ofGetWindowWidth()/2-HUDFontBig.stringWidth(msg)/2, ofGetWindowHeight()/2-70);
                                ofLine(3*ofGetWindowWidth()/8,ofGetWindowHeight()/2-3,5*(float)ofGetWindowWidth()/8,ofGetWindowHeight()/2-3);
                                ofLine(min(max((humPos+1)*ofGetWindowWidth()/2,3*(float)ofGetWindowWidth()/8),5*(float)ofGetWindowWidth()/8),ofGetWindowHeight()/2+5-18,min(max((humPos+1)*ofGetWindowWidth()/2,3*(float)ofGetWindowWidth()/8),5*(float)ofGetWindowWidth()/8),ofGetWindowHeight()/2+25-18);
                                msg="COMPETITIVE";
                                HUDFontBig.drawString(msg, 3*ofGetWindowWidth()/8-HUDFontBig.stringWidth(msg)/2, ofGetWindowHeight()/2-20);
                                msg="COOPERATIVE";
                                HUDFontBig.drawString(msg, 5*ofGetWindowWidth()/8-HUDFontBig.stringWidth(msg)/2, ofGetWindowHeight()/2-20);
                                break;
                            case 2:
                                msg="Your task was "+task+".";
                                HUDFontBig.drawString(msg, ofGetWindowWidth()/2-HUDFontBig.stringWidth(msg)/2, ofGetWindowHeight()/2-100);
                                msg="Then, later on, the partner was:";
                                HUDFontBig.drawString(msg, ofGetWindowWidth()/2-HUDFontBig.stringWidth(msg)/2, ofGetWindowHeight()/2-70);
                                ofLine(3*ofGetWindowWidth()/8,ofGetWindowHeight()/2-3,5*(float)ofGetWindowWidth()/8,ofGetWindowHeight()/2-3);
                                ofLine(min(max((humPos+1)*ofGetWindowWidth()/2,3*(float)ofGetWindowWidth()/8),5*(float)ofGetWindowWidth()/8),ofGetWindowHeight()/2+5-18,min(max((humPos+1)*ofGetWindowWidth()/2,3*(float)ofGetWindowWidth()/8),5*(float)ofGetWindowWidth()/8),ofGetWindowHeight()/2+25-18);
                                msg="COMPETITIVE";
                                HUDFontBig.drawString(msg, 3*ofGetWindowWidth()/8-HUDFontBig.stringWidth(msg)/2, ofGetWindowHeight()/2-20);
                                msg="COOPERATIVE";
                                HUDFontBig.drawString(msg, 5*ofGetWindowWidth()/8-HUDFontBig.stringWidth(msg)/2, ofGetWindowHeight()/2-20);
                                break;
                        	case 3:
                                msg="Overall, how was the partner like?";
                                HUDFontBig.drawString(msg, ofGetWindowWidth()/2-HUDFontBig.stringWidth(msg)/2, ofGetWindowHeight()/2-70);
                                //ofLine(3*ofGetWindowWidth()/8,ofGetWindowHeight()/2-3,5*(float)ofGetWindowWidth()/8,ofGetWindowHeight()/2-3);
                                if(humPos<-0.5){
                                    ofLine(3.5*(float)ofGetWindowWidth()/8,ofGetWindowHeight()/2+5-18,3.5*(float)ofGetWindowWidth()/8,ofGetWindowHeight()/2+25-18);
                                }else{
                                    if(humPos>0.5){
                                        ofLine(4.5*(float)ofGetWindowWidth()/8,ofGetWindowHeight()/2+5-18,4.5*(float)ofGetWindowWidth()/8,ofGetWindowHeight()/2+25-18);
                                    }
                                }
                                msg="ROBOT";
                                HUDFontBig.drawString(msg, 3.5*ofGetWindowWidth()/8-HUDFontBig.stringWidth(msg)/2, ofGetWindowHeight()/2-20);
                                robot.draw(3.5*ofGetWindowWidth()/8-robot.getWidth()/2 , ofGetWindowHeight()/2-20+robot.getHeight()/2);
                                msg="HUMAN";
                                HUDFontBig.drawString(msg, 4.5*ofGetWindowWidth()/8-HUDFontBig.stringWidth(msg)/2, ofGetWindowHeight()/2-20);
                                human.draw(4.5*ofGetWindowWidth()/8-robot.getWidth()/2 , ofGetWindowHeight()/2-20+robot.getHeight()/2);
                                break;
                            case 4:
                                question=1;
                                report=false;
                                trialReportFile.close();
                                break;ofSetColor(128);ofFill();
                        }

                    }
                    if((agent.task==0)||(agent.task==3)||(agent.task==4)){
                        question=1;
                        report=false;
                        trialReportFile.close();
                        ofSetColor(128);ofFill();
                        }
			    }else{
			        // In normal mode
                    if((!trialStart)||(metronome)){
                        msg="+";
                        HUDFontBig.drawString(msg, ofGetWindowWidth()/2-HUDFontBig.stringWidth(msg)/2, ofGetWindowHeight()/2+HUDFontBig.stringHeight(msg)/2);
                    }
			    }

			    // Plot trajectories
				if(TRAJFlag&&trialStart&&!metronome){
					ofSetColor(255,100,100);ofNoFill();
					if(agent.model==1){
						for (int step=0; step<480; step++) {
							ofLine(4*VPIpos[(step+currentStep)%480]+ofGetWidth()/2,step*ofGetHeight()/475, 4*VPIpos[((step+currentStep)+1)%480]+ofGetWidth()/2, (step+1)*ofGetHeight()/475);
						}
					}else if(agent.model==2){
						for (int step=0; step<480; step++) {
							ofLine(VPIpos[(step+currentStep)%480]/1.5+ofGetWidth()/2,step*ofGetHeight()/475,VPIpos[((step+currentStep)+1)%480]/1.5+ofGetWidth()/2, (step+1)*ofGetHeight()/475);
						}
					}else{
						for (int step=0; step<480; step++) {
							ofLine(VPIpos[(step+currentStep)%480]/2+ofGetWidth()/2,step*ofGetHeight()/475,VPIpos[((step+currentStep)+1)%480]/2+ofGetWidth()/2, (step+1)*ofGetHeight()/475);
						}
					}

					ofSetColor(100,100,255);ofNoFill();
					for (int step=0; step<480; step++) {
						ofLine(HUMpos[(step+currentStep)%480]+ofGetWidth()/2,step*ofGetHeight()/475, HUMpos[((step+currentStep)+1)%480]+ofGetWidth()/2, (step+1)*ofGetHeight()/475);
					}
					ofSetColor(128);ofFill();
				}
				unlock();
			}else{
				ofLog() << "Can't lock!\Neither an error\nor the thread has stopped!\n";
			}
		}

		//--------------------------------------------------------------
        void calibration(){
            intervalPos[0][0]=min(intervalPos[0][0],readDAQ(1));
            intervalPos[0][1]=max(intervalPos[0][1],readDAQ(1));
            intervalPos[1][0]=min(intervalPos[1][0],readDAQ(2));
            intervalPos[1][1]=max(intervalPos[1][1],readDAQ(2));
            humPos=(readDAQ(1)-intervalPos[0][0])/(intervalPos[0][1]-intervalPos[0][0])*2-1.0;
            ofLog() << "DAQ:" << readDAQ(1) << " min:" << intervalPos[0][0] << "max:" << intervalPos[0][1] << "hum:" << humPos << endl;
        }

        void trial(){
            if(trialStart){
                trigger(1);trigFlag=true;
                // End trial
                trialDataFile.close();
				metronome=false;
                trialStart=false;
                trialDataFile.close();
                if(automaticMode){
                    report=true;
                }else{
                    trialReportFile.close();
                }
            }else if(!report){
                // Start a new one
                trialNum++;trigger(1);
                ofLog() << " Trial " << trialNum << endl;
                trialDataFile.open(ofToDataPath(ofToString(ofGetYear())+"-"+ofToString(ofGetMonth())+"-"+ofToString(ofGetDay())+"_Trial"+ofToString(trialNum)+"_data.txt").c_str(),ios::out);
                trialReportFile.open(ofToDataPath(ofToString(ofGetYear())+"-"+ofToString(ofGetMonth())+"-"+ofToString(ofGetDay())+"_Trial"+ofToString(trialNum)+"_report.txt").c_str(),ios::out);
                if(automaticMode){
                    agent.loadTrial(trialNum);
                    switch1=true;switch2=false;switch3=false;
                }
				switch (agent.model) {
					case 1:
                        trialReportFile << "Task=" << agent.task << "\n";
                        trialReportFile << "Transition=" << agent.transition << "\n\n";
						trialReportFile << "Model=HKB\n";
						trialReportFile << "Mu=" << agent.mu << "\n";
						trialReportFile << "Omega=" << agent.omega << "\n";
						trialReportFile << "Alpha=" << agent.alpha << "\n";
						trialReportFile << "Beta=" << agent.beta << "\n";
						trialReportFile << "A=" << agent.A << "\n";
						trialReportFile << "B=" << agent.B << "\n";
						trialReportFile << "Gamma=" << agent.gamma << "\n\n\n\n\n";
						break;
					case 2:
                        trialReportFile << "Task=" << agent.task << "\n";
                        trialReportFile << "Transition=" << agent.transition << "\n\n";
						trialReportFile << "Model=EXC\n";
						trialReportFile << "Mu=" << agent.mu << "\n";
						trialReportFile << "Omega=" << agent.omega << "\n";
						trialReportFile << "Alpha=" << agent.alpha << "\n";
						trialReportFile << "Beta=" << agent.beta << "\n";
						trialReportFile << "A=" << agent.A << "\n";
						trialReportFile << "B=" << agent.B << "\n";
						trialReportFile << "Tau=" << agent.tau << "\n";
						trialReportFile << "Gain=" << agent.gain << "\n\n\n\n";
						break;
					case 3:
                        trialReportFile << "Task=" << agent.task << "\n";
                        trialReportFile << "Transition=" << agent.transition << "\n\n";
						trialReportFile << "Model=EXC2\n";
						trialReportFile << "Mu=" << agent.mu << "\n";
						trialReportFile << "Omega=" << agent.omega << "\n";
						trialReportFile << "Alpha=" << agent.alpha << "\n";
						trialReportFile << "Beta=" << agent.beta << "\n";
						trialReportFile << "A=" << agent.A << "\n";
						trialReportFile << "B=" << agent.B << "\n";
						trialReportFile << "Tau=" << agent.tau << "\n";
						trialReportFile << "Gain=" << agent.gain << "\n";
						trialReportFile << "Kappa=" << agent.kappa << "\n";
						trialReportFile << "Lambda=" << agent.lambda << "\n\n";
						break;
					default:
						break;
				}
				for (int step=0; step<480; step++) {
					VPIpos[step]=0.;
					HUMpos[step]=0.;
				}
				currentStep=0;
                trialStart=true;
                trigger(1);trigFlag=true;
                metronome=true;
				timeStart=ofGetElapsedTimeMillis();
                lastTime=ofGetElapsedTimeMillis();
				lastBip=ofGetElapsedTimeMillis();
            }else{
                // If the trial ended and we are in Automatic Mode, then verbal report of the subjects
                switch(question){
                    case 1:
                        trialReportFile << "CooperationStart";
                        ofLog() << "CooperationStart: ";
                        trialReportFile << "=" << 4*min(max((humPos+1)*ofGetWindowWidth()/2,3*(float)ofGetWindowWidth()/8),5*(float)ofGetWindowWidth()/8)/(float)ofGetWindowWidth()-1.5 << "\n";
                        ofLog() << 4*min(max((humPos+1)*ofGetWindowWidth()/2,3*(float)ofGetWindowWidth()/8),5*(float)ofGetWindowWidth()/8)/(float)ofGetWindowWidth()-1 << "\n";
                        break;
                    case 2:
                        trialReportFile << "CooperationEnd";
                        ofLog() << "CooperationEnd: ";
                        trialReportFile << "=" << 4*min(max((humPos+1)*ofGetWindowWidth()/2,3*(float)ofGetWindowWidth()/8),5*(float)ofGetWindowWidth()/8)/(float)ofGetWindowWidth()-1.5 << "\n";
                        ofLog() << 4*min(max((humPos+1)*ofGetWindowWidth()/2,3*(float)ofGetWindowWidth()/8),5*(float)ofGetWindowWidth()/8)/(float)ofGetWindowWidth()-1 << "\n";
                        break;
                    case 3:
                        trialReportFile << "Humanness";
                        ofLog() << "Humanness: ";
                        if(humPos<-0.5){
                            trialReportFile << "=" << 0 << "\n";
                            ofLog() << 0 << "\n";
                        }else{
                            if(humPos>0.5){
                                trialReportFile << "=" << 1 << "\n";
                                ofLog() << 1 << "\n";
                            }else{
                                trialReportFile << "=" << 0.5 << "\n";
                                ofLog() << 0.5 << "\n";
                            }
                        }

                        break;
                }
                trigger(3);trigFlag=true;
                question++;
            }
        }

        void HUD(){
            HUDFlag=!HUDFlag;
        }

        void TRAJ(){
            TRAJFlag=!TRAJFlag;
        }

	    void changeDynModel(int newDynModel){
			currentModel=newDynModel;
			if(!trialStart){
				agent.changeModel(currentModel);
			}
        }
};
#endif
