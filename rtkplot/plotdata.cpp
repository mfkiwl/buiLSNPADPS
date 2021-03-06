#include "rtklib.h"
#include "rtkplotmain.h"

static char path_str[MAXNFILE][1024];

#define MAX_SIMOBS	16384			// max genrated obs epochs

#define THRES_SLIP  2.0
//#define THRES_SLIP  80.0

//---------------------------------------------------------------------------
//check observation data types
int __fastcall TrtkplotMainWindow::CheckObs(AnsiString file)
{
	char *p;

    trace(3,"CheckObs\n");

    if (!(p=strrchr(file.c_str(),'.'))) return 0;
    if (!strcmp(p,".z")||!strcmp(p,".gz")||!strcmp(p,".zip")||
        !strcmp(p,".Z")||!strcmp(p,".GZ")||!strcmp(p,".ZIP")) {
        return *(p-1)=='o'||*(p-1)=='O'||*(p-1)=='d'||*(p-1)=='D';
    }
    return !strcmp(p,".obs")||!strcmp(p,".OBS")||
           !strcmp(p+3,"o" )||!strcmp(p+3,"O" )||
           !strcmp(p+3,"d" )||!strcmp(p+3,"D" );
}

//---------------------------------------------------------------------------
void __fastcall TrtkplotMainWindow::ReadObs(TStrings *files)
{
	obs_t obs={0};
    nav_t nav={0};
    sta_t sta={0};
    AnsiString s;
    char file[1024];
    int i,nobs;

    trace(3,"ReadObs\n");

	if (files->Count<=0)
		return;

	ReadWaitStart();
    ShowLegend(NULL);

	if ((nobs=ReadObsRnx(files,&obs,&nav,&sta))<=0) {
        ReadWaitEnd();
        return;
	}
	ClearObs();
	Obs=obs;
	Nav=nav;
    Sta=sta;
	SimObs=0;
	UpdateObs(nobs);
	UpdateMp();
	UpdateCycleSlipMark();
	UpdateIon();

	if(ObsFiles!=files)
		ObsFiles->Assign(files);
	NavFiles->Clear();

    strcpy(file,U2A(files->Strings[0]).c_str());

	//Caption=s.sprintf("%s%s",file,files->Count>1?"...":"");

	//BtnSol1->Down=true;
	time2gpst(Obs.data[0].time,&Week);
	SolIndex[0]=SolIndex[1]=ObsIndex=0;

	/*if (PlotType<PLOT_OBS||PLOT_DOP<PlotType) {
		UpdateType(PLOT_OBS);
	}
	else {
		UpdatePlotType();
	}*/
	UpdateType(PLOT_VisSat);

    FitTime();

	ReadWaitEnd();
	UpdateObsType();
	UpdateTime();
    UpdatePlot();
}
// update Ion ----------------------------------------------------------------------
void __fastcall TrtkplotMainWindow::UpdateIon(void)
{
	AnsiString s;
	obsd_t *data;
	double lam1,lam2,I,C,B;
	int i,j,k,f1,f2,sat,sys,per,per_=-1,n;

	trace(3,"UpdateIon\n");

	for(i=0;i<NFREQ+NEXOBS;i++) {
        delete [] Ion[i]; Ion[i] = NULL;
	}
	if(Obs.n<=0) return;

	for(i=0;i<NFREQ+NEXOBS;i++) {
        Ion[i] = new double[Obs.n];
	}

	ReadWaitStart();
	ShowLegend(NULL);

	for(i=0;i<Obs.n;i++) {
		data = Obs.data+i;
		sys = satsys(data->sat,NULL);

		for(j=0;j<NFREQ+NEXOBS;j++) {
			Ion[j][i]=0.0;

			code2obs(data->code[j],&f1);

			if (sys==SYS_CMP) {
                if      (f1==5) f1=2; /* B2 */
                else if (f1==4) f1=3; /* B3 */
            }
            if      (sys==SYS_GAL) f2=f1==1?3:1; /* E1/E5a */
            else if (sys==SYS_SBS) f2=f1==1?3:1; /* L1/L5 */
            else if (sys==SYS_CMP) f2=f1==1?2:1; /* B1/B2 */
			else                   f2=f1==1?2:1; /* L1/L2 */

			lam1=satwavelen(data->sat,f1-1,&Nav);
			lam2=satwavelen(data->sat,f2-1,&Nav);
			if(lam1==0.0||lam2==0.0) continue;

			if(data->L[j]!=0.0 && data->L[f2-1]) {
				C = (-1.0)*SQR(lam1)/(SQR(lam1)-SQR(lam2));
				I = lam1*data->L[j]-lam2*data->L[f2-1];
                Ion[j][i]=C*I;
            }
        }
	}

	for(sat=1;sat<=MAXSAT;sat++) {
		for(i=0;i<NFREQ+NEXOBS;i++) {
			sys = satsys(sat,NULL);
			for(j=k=n=0,B=0.0;j<Obs.n;j++) {
				if (Obs.data[j].sat!=sat) continue;

				code2obs(Obs.data[j].code[i],&f1);

                if (sys==SYS_CMP) {
					if      (f1==5) f1=2; /* B2 */
					else if (f1==4) f1=3; /* B3 */
				}
				if      (sys==SYS_GAL) f2=f1==1?3:1;
				else if (sys==SYS_CMP) f2=f1==1?2:1;
				else                   f2=f1==1?2:1;

				//if ((Obs.data[j].LLI[i]&1)||(Obs.data[j].LLI[f2-1]&1)||fabs(Ion[i][j]-B)>THRES_SLIP) {
				if ((Obs.data[j].LLI[i]&1)||(Obs.data[j].LLI[f2-1]&1)||fabs(Ion[i][j]-B)>10) {
					for (;k<j;k++) if (Obs.data[k].sat==sat) Ion[i][k]-=B;
					B=Ion[i][j]; n=1; k=j;
				}
				else {
					if (n==0) k=j;
					B+=(Ion[i][j]-B)/++n;
				}
			}

			if(n>0) {
            	for (;k<j;k++) if (Obs.data[k].sat==sat) Ion[i][k]-=B;
			}

            per=sat*100/MAXSAT;
			if (per!=per_) {
				ShowMsg(s.sprintf("计算电离层延迟误差... (%d%%)",(per_=per)));
				Application->ProcessMessages();
			}

        }
    }


    ReadWaitEnd();
}
// update CycleSlipMark ------------------------------------------------------------
void __fastcall TrtkplotMainWindow::UpdateCycleSlipMark(void)
{
	AnsiString s;
	double lam1, lam2,B;
	int i,j,k,n,per,per_=-1,f1,f2,sys,sat;
    obsd_t *data;
	double *GF[NFREQ+NEXOBS];
	double g0=0.0,g1=0.0;
	for(i=0;i<NFREQ+NEXOBS;i++) {
		GF[i]=NULL;
    }

	if(Obs.n<=0) return;

	for(i=0;i<NFREQ+NEXOBS;i++)
		GF[i] = new double[Obs.n];

	ReadWaitStart();
	ShowLegend(NULL);

	/*for(i=0;i<Obs.n;i++) {
		data = Obs.data +i;
		sys = satsys(data->sat, NULL);

		for (j = 0; j<NFREQ + NEXOBS; j++) {
			GF[j][i]=0.0;
			code2obs(data->code[j], &f1);

			if(sys==SYS_CMP) {
				if (f1 == 5) f1 = 2; //B2
				else if (f1 == 4) f1 = 3; //B3
			}
			if (sys == SYS_GAL) f2 = f1 == 1 ? 3 : 1; //E1/E5a
			else if (sys == SYS_SBS) f2 = f1 == 1 ? 3 : 1; //L1/L5
			else if (sys == SYS_CMP) f2 = f1 == 1 ? 2 : 1; //B1/B2
			else                   f2 = f1 == 1 ? 2 : 1; //L1/L2

            lam1 = satwavelen(data->sat, f1 - 1, &Nav);
			lam2 = satwavelen(data->sat, f2 - 1, &Nav);
			if (lam1 == 0.0 || lam2 == 0.0) continue;

			if(data->L[j]!=0.0 && data->L[f2-1]!=0.0) {
            	GF[j][i] = lam1*data->L[j]-lam2*data->L[f2-1];
            }
        }
	}
	for(sat=1;sat<=MAXSAT;sat++) {
		for(i=0;i<NFREQ+NEXOBS;i++) {
			sys = satsys(sat,NULL);

			for(j=0;j<Obs.n;j++) {
				if(Obs.data[j].sat !=sat) continue;

				if(GF[i][j]==0.0) continue;

				g0 = g1;
				g1 = GF[i][j];

				if(g0!=0.0 && fabs(g1-g0)>0.05) {
					CycleSlipMark[i][j] = 1;
				}
			}
			
			per=sat*100/MAXSAT;
			if (per != per_) {
				ShowMsg(s.sprintf("GF方法进行周跳探测... (%d%%)", (per_ = per)));
				Application->ProcessMessages();
			}
        }
	}*/

	for(i=0;i<Obs.n;i++) {
		//if(El[i]*R2D<10.0) continue;
		for(j=0;j<NFREQ+NEXOBS;j++) {
			if(!(Obs.data[i].LLI[j]&3)) continue;
				CycleSlipMark[j][i] = 1;
		}
		per=i*100/Obs.n;
        if (per != per_) {
			ShowMsg(s.sprintf("周跳探测... (%d%%)", (per_ = per)));
			Application->ProcessMessages();
		}
	}

    ReadWaitEnd();
}
// update Multipath ------------------------------------------------------------
void __fastcall TrtkplotMainWindow::UpdateMp(void)
{
    AnsiString s;
    obsd_t *data;
    double lam1,lam2,I,C,B;
    int i,j,k,f1,f2,sat,sys,per,per_=-1,n;

    trace(3,"UpdateMp\n");

    for (i=0;i<NFREQ+NEXOBS;i++) {
		delete [] Mp[i]; Mp[i]=NULL;
		delete [] CycleSlipMark[i]; CycleSlipMark[i]=NULL;
    }
    if (Obs.n<=0) return;

    for (i=0;i<NFREQ+NEXOBS;i++) {
		Mp[i]=new double[Obs.n];
		CycleSlipMark[i]=new int[Obs.n];
    }
    ReadWaitStart();
    ShowLegend(NULL);

    for (i=0;i<Obs.n;i++) {
        data=Obs.data+i;
        sys=satsys(data->sat,NULL);

        for (j=0;j<NFREQ+NEXOBS;j++) {
			Mp[j][i]=0.0;
			CycleSlipMark[j][i]=0;

            code2obs(data->code[j],&f1);

            if (sys==SYS_CMP) {
                if      (f1==5) f1=2; /* B2 */
                else if (f1==4) f1=3; /* B3 */
            }
            if      (sys==SYS_GAL) f2=f1==1?3:1; /* E1/E5a */
            else if (sys==SYS_SBS) f2=f1==1?3:1; /* L1/L5 */
            else if (sys==SYS_CMP) f2=f1==1?2:1; /* B1/B2 */
            else                   f2=f1==1?2:1; /* L1/L2 */

            lam1=satwavelen(data->sat,f1-1,&Nav);
            lam2=satwavelen(data->sat,f2-1,&Nav);
            if (lam1==0.0||lam2==0.0) continue;

            if (data->P[j]!=0.0&&data->L[j]!=0.0&&data->L[f2-1]) {
                C=SQR(lam1)/(SQR(lam1)-SQR(lam2));
                I=lam1*data->L[j]-lam2*data->L[f2-1];
                Mp[j][i]=data->P[j]-lam1*data->L[j]+2.0*C*I;
            }
        }
    }
	for (sat=1;sat<=MAXSAT;sat++)
		for (i=0;i<NFREQ+NEXOBS;i++)
		{
			sys=satsys(sat,NULL);

			for (j=k=n=0,B=0.0;j<Obs.n;j++) {
				if (Obs.data[j].sat!=sat) continue;

				code2obs(Obs.data[j].code[i],&f1);

				if (sys==SYS_CMP) {
					if      (f1==5) f1=2; /* B2 */
					else if (f1==4) f1=3; /* B3 */
				}
				if      (sys==SYS_GAL) f2=f1==1?3:1;
				else if (sys==SYS_CMP) f2=f1==1?2:1;
				else                   f2=f1==1?2:1;

				if ((Obs.data[j].LLI[i]&1)||(Obs.data[j].LLI[f2-1]&1)||fabs(Mp[i][j]-B)>THRES_SLIP) {
				//if ((Obs.data[j].LLI[i]&1)||(Obs.data[j].LLI[f2-1]&1)) {
				//if(fabs(Mp[i][j]-B)>THRES_SLIP) {
					//if(El[j]*R2D>10)
					//CycleSlipMark[i][j]=1;
					if(fabs(Mp[i][j]-B)>10 &&(Obs.data[j].LLI[i]&1)==false&&(Obs.data[j].LLI[f2-1]&1)==false)
                    	CycleSlipMark[i][j]=1;
					for (;k<j;k++) if (Obs.data[k].sat==sat) Mp[i][k]-=B;
					B=Mp[i][j]; n=1; k=j;
				}
				else {
					if (n==0) k=j;
					B+=(Mp[i][j]-B)/++n;
				}
			}
			if (n>0) {
				for (;k<j;k++) if (Obs.data[k].sat==sat) Mp[i][k]-=B;
			}
			per=sat*100/MAXSAT;
			if (per!=per_) {
				//ShowMsg(s.sprintf("updating multipath... (%d%%)",(per_=per)));
				ShowMsg(s.sprintf("计算多路径误差并探测周跳... (%d%%)",(per_=per)));
				Application->ProcessMessages();
			}
		}
    ReadWaitEnd();
}
// update observation data index, azimuth/elevation, satellite list ---------
void __fastcall TrtkplotMainWindow::UpdateObs(int nobs)
{
    AnsiString s;
    prcopt_t opt=prcopt_default;
    gtime_t time;
    sol_t sol={0};
    double pos[3],rr[3],e[3],azel[MAXOBS*2]={0},rs[6],dts[2],var;
    int i,j,k,svh,per,per_=-1;
    char msg[128],name[16];

    trace(3,"UpdateObs\n");

    delete [] IndexObs; IndexObs=NULL;
    delete [] Az; Az=NULL;
	delete [] El; El=NULL;
	delete [] Intervals; Intervals=NULL;
    NObs=0;
    if (nobs<=0) return;

    IndexObs=new int[nobs+1];
    Az=new double[Obs.n];
	El=new double[Obs.n];
    Intervals = new double[Obs.n];

    opt.err[0]=900.0;

    ReadWaitStart();
    ShowLegend(NULL);

	double dt;
    for (i=0;i<Obs.n;i=j) {
        time=Obs.data[i].time;
		for (j=i;j<Obs.n;j++) {
			dt = timediff(Obs.data[j].time,time);
			if (dt>TTOL) break;
		}
		Intervals[NObs]=dt;
        IndexObs[NObs++]=i;

        for (k=0;k<j-i;k++) {
            azel[k*2]=azel[1+k*2]=0.0;
        }
        if (RcvPos==0) {
            pntpos(Obs.data+i,j-i,&Nav,&opt,&sol,azel,NULL,msg);
            matcpy(rr,sol.rr,3,1);
            ecef2pos(rr,pos);
        }
        else {
            if (RcvPos==1) { // lat/lon/height
                for (k=0;k<3;k++) pos[k]=OOPos[k];
                pos2ecef(pos,rr);
            }
            else { // rinex header position
                for (k=0;k<3;k++) rr[k]=Sta.pos[k];
                ecef2pos(rr,pos);
            }
            for (k=0;k<j-i;k++) {
                azel[k*2]=azel[1+k*2]=0.0;
                if (!satpos(time,time,Obs.data[i+k].sat,EPHOPT_BRDC,&Nav,rs,dts,
                            &var,&svh)) continue;
                if (geodist(rs,rr,e)>0.0) satazel(pos,e,azel+k*2);
            }
        }
        // satellite azel by tle data
        for (k=0;k<j-i;k++) {
            if (azel[k*2]!=0.0||azel[1+k*2]!=0.0) continue;
            satno2id(Obs.data[i+k].sat,name);
            if (!tle_pos(time,name,"","",&TLEData,NULL,rs)) continue;
            if (geodist(rs,rr,e)>0.0) satazel(pos,e,azel+k*2);
        }
        for (k=0;k<j-i;k++) {
            Az[i+k]=azel[  k*2];
			El[i+k]=azel[1+k*2];
			Obs.data[i+k].elevation = El[i+k];
            if (Az[i+k]<0.0) Az[i+k]+=2.0*PI;
        }
        per=(i+1)*100/Obs.n;
        if (per!=per_) {
			//ShowMsg(s.sprintf("updating azimuth/elevation... (%d%%)",(per_=per)));
			ShowMsg(s.sprintf("计算方位角/高度角...(%d%%)",(per_=per)));
            Application->ProcessMessages();
        }
    }
    IndexObs[NObs]=Obs.n;

    UpdateSatList();

    ReadWaitEnd();
}
// read observation data rinex ----------------------------------------------
int __fastcall TrtkplotMainWindow::ReadObsRnx(TStrings *files, obs_t *obs, nav_t *nav,
                                 sta_t *sta)
{
    AnsiString s;
    gtime_t ts,te;
    double tint;
    int i,n;
    char obsfile[1024],navfile[1024]="",*p,*q,*opt=RnxOpts.c_str();

    trace(3,"ReadObsRnx\n");

    TimeSpan(&ts,&te,&tint);

    for (i=0;i<files->Count;i++) {
        strcpy(obsfile,U2A(files->Strings[i]).c_str());

		//ShowMsg(s.sprintf("reading obs data... %s",obsfile));
		ShowMsg(s.sprintf("读取观测数据... %s",obsfile));
		Application->ProcessMessages();

        if (readrnxt(obsfile,1,ts,te,tint,opt,obs,nav,sta)<0) {
            ShowMsg("error: insufficient memory");
            return -1;
        }
    }
    ShowMsg("读取星历数据...");
    Application->ProcessMessages();

    for (i=0;i<files->Count;i++) {
        strcpy(navfile,U2A(files->Strings[i]).c_str());

        if (!(p=strrchr(navfile,'.'))) continue;

        if (!strcmp(p,".obs")||!strcmp(p,".OBS")) {
            strcpy(p,".nav" ); readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
            strcpy(p,".gnav"); readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
            strcpy(p,".hnav"); readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
            strcpy(p,".qnav"); readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
            strcpy(p,".lnav"); readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
        }
        else if (!strcmp(p+3,"o" )||!strcmp(p+3,"d" )||
                 !strcmp(p+3,"O" )||!strcmp(p+3,"D" )) {
            n=nav->n;

			strcpy(p+3,"N");
			readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
            strcpy(p+3,"n");
			readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
			strcpy(p+3,"G");
			readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
			strcpy(p+3,"g");
			readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
			strcpy(p+3,"H");
			readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
			strcpy(p+3,"h");
			readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
			strcpy(p+3,"Q");
			readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
			strcpy(p+3,"q");
			readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
			strcpy(p+3,"L");
			readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
			strcpy(p+3,"l");
			readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
			strcpy(p+3,"P");
			readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
			strcpy(p+3,"p");
			readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);

			if (nav->n>n||!(q=strrchr(navfile,'\\')))
				continue;

			// read brdc navigation data
			memcpy(q+1,"BRDC",4);
			strcpy(p+3,"N");
			readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
			strcpy(p+3,"P");
			readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);

			memcpy(q+1,"brdc",4);
            strcpy(p+3,"p"); readrnxt(navfile,1,ts,te,tint,opt,NULL,nav,NULL);
        }
    }
    if (obs->n<=0) {
		ShowMsg(s.sprintf("观测数据为空: %s...",files->Strings[0].c_str()));
        freenav(nav,0xFF);
        return 0;
    }
    uniqnav(nav);
    return sortobs(obs);
}

// read wait start ----------------------------------------------------------
void __fastcall TrtkplotMainWindow::ReadWaitStart(void)
{
	//MenuFile->Enabled=false;
	//MenuEdit->Enabled=false;
	//MenuView->Enabled=false;
	//MenuHelp->Enabled=false;
    Panel1->Enabled=false;
    Disp->Enabled=false;
    Screen->Cursor=crHourGlass;
}
// read wait end ------------------------------------------------------------
void __fastcall TrtkplotMainWindow::ReadWaitEnd(void)
{
	//MenuFile->Enabled=true;
	//MenuEdit->Enabled=true;
	//MenuView->Enabled=true;
    //MenuHelp->Enabled=true;
    Panel1->Enabled=true;
    Disp->Enabled=true;
    Screen->Cursor=crDefault;
}
// clear obs data --------------------------------------------------------------
void __fastcall TrtkplotMainWindow::ClearObs(void)
{
    sta_t sta0={0};
    int i;

    freeobs(&Obs);
    freenav(&Nav,0xFF);
    delete [] IndexObs; IndexObs=NULL;
    delete [] Az; Az=NULL;
    delete [] El; El=NULL;
    for (i=0;i<NFREQ+NEXOBS;i++) {
        delete [] Mp[i]; Mp[i]=NULL;
    }
    ObsFiles->Clear();
    NavFiles->Clear();
    NObs=0;
    Sta=sta0;
    ObsIndex=0;
    SimObs=0;
}
// clear solution --------------------------------------------------------------
void __fastcall TrtkplotMainWindow::ClearSol(void)
{
    int i;

    for (i=0;i<2;i++) {
        freesolbuf(SolData+i);
        free(SolStat[i].data);
        SolStat[i].n=0;
        SolStat[i].data=NULL;
    }
    SolFiles[0]->Clear();
    SolFiles[1]->Clear();
    SolIndex[0]=SolIndex[1]=0;
}
// clear data ------------------------------------------------------------------
void __fastcall TrtkplotMainWindow::Clear(void)
{
    AnsiString s;
    double ep[]={2010,1,1,0,0,0};
    int i;

    trace(3,"Clear\n");

    Week=0;

    ClearObs();
    ClearSol();

    for (i=0;i<3;i++) {
        TimeEna[i]=0;
    }
    TimeStart=TimeEnd=epoch2time(ep);
    BtnAnimate->Down=false;

	/*if (PlotType>PLOT_NSAT) {
		UpdateType(PLOT_TRK);
	}
    if (!ConnectState) {
        initsolbuf(SolData  ,0,0);
        initsolbuf(SolData+1,0,0);
        Caption=Title!=""?Title:s.sprintf("%s ver.%s",PRGNAME,VER_RTKLIB);
    }
    else {
        initsolbuf(SolData  ,1,RtBuffSize+1);
        initsolbuf(SolData+1,1,RtBuffSize+1);
    }*/
    //GoogleEarthView->Clear();

    UpdateTime();
    UpdatePlot();
}

