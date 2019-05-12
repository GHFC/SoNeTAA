#include "VPI.h"

VPI::VPI(){
	model=3;
	runge=true;
    loadParameters();
    x=ofRandomf();
    xd=ofRandomf();
}

VPI::VPI(int dynModel){
	model=dynModel;
	runge=true;
    loadParameters();
    x=ofRandomf();
    xd=ofRandomf();
}

float VPI::update(float y, float yd, float timeStep){
	float F=0;
	float I=0;

	float x_0=x;
	float xd_0=xd;
	float y_0=y;
    float yd_0=yd;
	float om_0=om;

    float xdd_1,xd_1,xdd_2,xd_2,xdd_3,xd_3,xdd_4,xd_4,omd_1,omd_2,omd_3,omd_4;

	switch (model) {
		case 1:
				// Initialization
				om=omega;

				// K1: Euler, slope at the beginning of the interval
				F = (xd_0-mu*yd_0)*(A+B*(pow((x_0-mu*y_0),2)));
				xdd_1=-(pow(om,2))*x_0+xd_0*(alpha-beta*(pow(xd_0,2))-gamma*(pow(x_0,2))) + F;
				xd_1=xd_0+xdd_1*timeStep;

				// Runge-Kutta method
				if(runge){
					// K2: slope at the midpoint of the interval using K1
					x_0=x+timeStep/2*xd_1;
					xd_0=xd+timeStep/2*xdd_1;
					y_0=y+timeStep/2*yd_0;
					yd_0=yd;

					F = (xd_0-mu*yd_0)*(A+B*(pow((x_0-mu*y_0),2)));
					xdd_2=-(pow(om,2))*x_0+xd_0*(alpha-beta*(pow(xd_0,2))-gamma*(pow(x_0,2))) + F;
					xd_2=xd_0+xdd_2*timeStep/2;

					// K3: slope at at the midpoint, but now using K2
					x_0=x+timeStep/2*xd_2;
					xd_0=xd+timeStep/2*xdd_2;
					y_0=y+timeStep/2*yd_0;
					yd_0=yd;

					F = (xd_0-mu*yd_0)*(A+B*(pow((x_0-mu*y_0),2)));
					xdd_3=-(pow(om,2))*x_0+xd_0*(alpha-beta*(pow(xd_0,2))-gamma*(pow(x_0,2))) + F;
					xd_3=xd_0+xdd_3*timeStep/2;

					// K4: slope at the end of the interval using K3
					x_0=x+timeStep*xd_3;
					xd_0=xd+timeStep*xdd_3;
					y_0=y+timeStep*yd_0;
					yd_0=yd;

					F = (xd_0-mu*yd_0)*(A+B*(pow((x_0-mu*y_0),2)));
					xdd_4=-(pow(om,2))*x_0+xd_0*(alpha-beta*(pow(xd_0,2))-gamma*(pow(x_0,2))) + F;
					xd_4=xd_0+xdd_4*timeStep;

					// Final
					xd=xd+timeStep*(xdd_1 + 2*xdd_2+ 2*xdd_3 + xdd_4)/6;
					x=x+timeStep*(xd_1 + 2*xd_2+ 2*xd_3 + xd_4)/6;
			    }else{
			    	xd=xd_1;
				    x=x+xd_1*timeStep;
			    }

			break;
		case 2:
				// Initialization
				om=omega;

				// K1: Euler, slope at the beginning of the interval
			    F = (xd_0-mu*yd_0)*(alpha+beta*pow(x_0-mu*y_0,2));
				I = mu*y_0*gain;
				xdd_1=(1-g1d(x_0))*tau*om*xd_0 + tau*om* (-om*(x_0-A-g2(x_0,xd_0/(tau*om)-x_0+g1(x_0))-I)/tau) + F;
			    xd_1=xd_0+xdd_1*timeStep;

				// Runge-Kutta method
				if(runge){
					// K2: slope at the midpoint of the interval using K1
					x_0=x+timeStep/2*xd_1;
					xd_0=xd+timeStep/2*xdd_1;
					y_0=y+timeStep/2*yd_0;
					yd_0=yd;

					F = (xd_0-mu*yd_0)*(A+B*(pow((x_0-mu*y_0),2)));
					I = mu*y_0*gain;
					xdd_2=(1-g1d(x_0))*tau*om*xd_0 + tau*om* (-om*(x_0-A-g2(x_0,xd_0/(tau*om)-x_0+g1(x_0))-I)/tau) + F;
					xd_2=xd_0+xdd_2*timeStep/2;

					// K3: slope at at the midpoint, but now using K2
					x_0=x+timeStep/2*xd_2;
					xd_0=xd+timeStep/2*xdd_2;
					y_0=y+timeStep/2*yd_0;
					yd_0=yd;

					F = (xd_0-mu*yd_0)*(A+B*(pow((x_0-mu*y_0),2)));
					I = mu*y_0*gain;
					xdd_3=(1-g1d(x_0))*tau*om*xd_0 + tau*om* (-om*(x_0-A-g2(x_0,xd_0/(tau*om)-x_0+g1(x_0))-I)/tau) + F;
					xd_3=xd_0+xdd_3*timeStep/2;

					// K4: slope at the end of the interval using K3
					x_0=x+timeStep*xd_3;
					xd_0=xd+timeStep*xdd_3;
					y_0=y+timeStep*yd_0;
					yd_0=yd;

					F = (xd_0-mu*yd_0)*(A+B*(pow((x_0-mu*y_0),2)));
					I = mu*y_0*gain;
					xdd_4=(1-g1d(x_0))*tau*om*xd_0 + tau*om* (-om*(x_0-A-g2(x_0,xd_0/(tau*om)-x_0+g1(x_0))-I)/tau) + F;
					xd_4=xd_0+xdd_4*timeStep;

					// Final
					xd=xd+timeStep*(xdd_1 + 2*xdd_2+ 2*xdd_3 + xdd_4)/6;
					x=x+timeStep*(xd_1 + 2*xd_2+ 2*xd_3 + xd_4)/6;
			    }else{
			    	xd=xd_1;
				    x=x+xd_1*timeStep;
			    }
			break;
		case 3:
				// K1: Euler, slope at the beginning of the interval
				F = (xd_0-mu*yd_0)*(alpha+beta*pow(x_0-mu*y_0,2));
				I = mu*y_0*gain;
				omd_1=kappa*((omega-om_0)*(1-lambda)+lambda/alpha*F*xd_0/sqrt(pow(xd_0,2)+pow(x_0,2)));
				xdd_1=(1-g1d(x_0))*tau*om_0*xd_0 + tau*om_0* (-om_0*(x_0-A-g2(x_0,xd_0/(tau*om_0)-x_0+g1(x_0))-I)/tau) + F;
				xd_1=xd_0+xdd_1*timeStep;

				// Runge-Kutta method
				if(runge){
					// K2: slope at the midpoint of the interval using K1
					x_0=x+timeStep/2*xd_1;
					xd_0=xd+timeStep/2*xdd_1;
					y_0=y+timeStep/2*yd_0;
					yd_0=yd;
					om_0=om+timeStep/2*omd_1;

					F = (xd_0-mu*yd_0)*(alpha+beta*pow(x_0-mu*y_0,2));
					I = mu*y_0*gain;
					omd_2=kappa*((omega-om_0)*(1-lambda)+lambda/alpha*F*xd_0/sqrt(pow(xd_0,2)+pow(x_0,2)));
					xdd_2=(1-g1d(x_0))*tau*om_0*xd_0 + tau*om_0* (-om_0*(x_0-A-g2(x_0,xd_0/(tau*om_0)-x_0+g1(x_0))-I)/tau) + F;
					xd_2=xd_0+xdd_2*timeStep/2;

					// K3: slope at at the midpoint, but now using K2
					x_0=x+timeStep/2*xd_2;
					xd_0=xd+timeStep/2*xdd_2;
					y_0=y+timeStep/2*yd_0;
					yd_0=yd;
					om_0=om_0+timeStep/2*omd_2;

					F = (xd_0-mu*yd_0)*(alpha+beta*pow(x_0-mu*y_0,2));
					I = mu*y_0*gain;
					omd_3=kappa*((omega-om_0)*(1-lambda)+lambda/alpha*F*xd_0/sqrt(pow(xd_0,2)+pow(x_0,2)));
					xdd_3=(1-g1d(x_0))*tau*om_0*xd_0 + tau*om_0* (-om_0*(x_0-A-g2(x_0,xd_0/(tau*om_0)-x_0+g1(x_0))-I)/tau) + F;
					xd_3=xd_0+xdd_3*timeStep/2;

					// K4: slope at the end of the interval using K3
					x_0=x+timeStep*xd_3;
					xd_0=xd+timeStep*xdd_3;
					y_0=y+timeStep*yd_0;
					yd_0=yd;
					om_0=om_0+timeStep*omd_3;

					F = (xd_0-mu*yd_0)*(alpha+beta*pow(x_0-mu*y_0,2));
					I = mu*y_0*gain;
					omd_4=kappa*((omega-om_0)*(1-lambda)+lambda/alpha*F*xd_0/sqrt(pow(xd_0,2)+pow(x_0,2)));
					xdd_4=(1-g1d(x_0))*tau*om_0*xd_0 + tau*om_0* (-om_0*(x_0-A-g2(x_0,xd_0/(tau*om_0)-x_0+g1(x_0))-I)/tau) + F;
					xd_4=xd_0+xdd_4*timeStep;

					// Final
					xd=xd_0+timeStep*(xdd_1 + 2*xdd_2+ 2*xdd_3 + xdd_4)/6;
					x=x_0+timeStep*(xd_1 + 2*xd_2+ 2*xd_3 + xd_4)/6;
					om=max((float)0.1,om+timeStep*(omd_1 + 2*omd_2+ 2*omd_3 + omd_4)/6);
				}else{
			    	xd=xd_1;
				    x=x+xd_1*timeStep;
				    om=max((float)0.1,om+timeStep*omd_1);
			    }
			break;
		default:
			break;
	}

	if(x != x){
		x=0;
		xd=0;
	}
	return F;
}

void VPI::changeModel(int newModel){
    model=newModel;
	loadParameters();
}

void VPI::loadTrial(int trial){
    if( XML.loadFile("trials.xml") ){
		ofLog() << "trials.xml loaded!\n";
	}else{
		ofLog() << "Unable to load trials.xml - check data/ folder\n";
	}
	string key;
	key="trial"+ofToString(trial)+":model";
	model=XML.getValue(key,1.0);
	loadParameters();
	XML.loadFile("trials.xml");
	key="trial"+ofToString(trial)+":mu";
	mu=XML.getValue(key,1.0);
	key="trial"+ofToString(trial)+":a";
	switch (model) {
		case 1:
            A=XML.getValue(key,1.0);
            break;
 		case 2:
            alpha=XML.getValue(key,1.0);
            break;
        case 3:
            alpha=XML.getValue(key,1.0);
            break;
		default:
			break;
	}
    key="trial"+ofToString(trial)+":task";
	task=XML.getValue(key,-1);
	key="trial"+ofToString(trial)+":tr";
	transition=XML.getValue(key,-1);
}

void VPI::loadParameters(){
	if( XML.loadFile("ressources/VPI.xml") ){
		ofLog() << "VPI.xml loaded!\n";
	}else{
		ofLog() << "Unable to load VPI.xml - check data/ folder\n";
	}
	switch (model) {
		//float kc;
		case 1:
			alpha=XML.getValue("HKB:alpha", 0.641);
			beta=XML.getValue("HKB:beta",0.00709);
			gamma=XML.getValue("HKB:gamma",12.457);
			A=XML.getValue("HKB:A",0.12);
			B=XML.getValue("HKB:B",0.025);
			omega=XML.getValue("HKB:omega", 1.6)*2*PI;
			break;
		case 2:
			tau=XML.getValue("EXC:tau", 1.0);
			// Transition entre 2 et 2.5 Hz
			alpha=XML.getValue("EXC:alpha", 1.0);
			//kc=ofRandomf()*0.3+2.25;
			beta=XML.getValue("EXC:beta",-0.2); //-2*kc/(1+4*kc); //
			gain=XML.getValue("EXC:gain",1.0);
			A=XML.getValue("EXC:A",0.5);
			B=XML.getValue("EXC:B",0.0);
			omega=XML.getValue("EXC:omega", 1.0)*2*PI;
			break;
		case 3:
			tau=XML.getValue("EXC2:tau", 1.0);
			// Transition entre 2 et 2.5 Hz
			alpha=XML.getValue("EXC2:alpha", 1.0);
			//kc=ofRandomf()*0.3+2.25;
			beta=XML.getValue("EXC2:beta",-0.2); //-2*kc/(1+4*kc); //
			kappa=XML.getValue("EXC2:kappa",5.0);
			lambda=XML.getValue("EXC2:lambda",0.5);
			gain=XML.getValue("EXC2:gain",1.0);
			A=XML.getValue("EXC2:A",0.5);
			B=XML.getValue("EXC2:B",0.0);
			omega=XML.getValue("EXC2:omega", 1.0)*2*PI;
			break;
		default:
			break;
	}

	om=omega;
	mu=XML.getValue("CFG:mu",1.0);
	task=XML.getValue("CFG:task",1.0);
	transition=XML.getValue("CFG:tr",0.0);
}

void VPI::flip(){
    mu=(-1)*mu;
}

float VPI::g1(float x)
{
 return x-pow(x,3)/3;
}

float VPI::g1d(float x)
{
 return pow(x,2);
}

float VPI::g2(float x, float y)
{
 return -B*y;
}
