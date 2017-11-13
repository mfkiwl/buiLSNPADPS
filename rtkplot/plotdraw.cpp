#include <vcl.h>
#include "rtklib.h"
#include "rtkplotmain.h"
#include "graph.h"


#define ATAN2(x,y)  ((x)*(x)+(y)*(y)>1E-12?atan2(x,y):0.0)


// draw slip on observation-data-plot ---------------------------------------
void __fastcall TrtkplotMainWindow::DrawObsSlip(double *yp)
{
    AnsiString ObsTypeText=ObsType->Text;
    obsd_t *obs;
    TPoint ps[2];
    double gfp[MAXSAT]={0},gf;
    const char *code;
	int i,j,slip;
	int tempSatCount=0;

    trace(3,"DrawObsSlip\n");

    code=ObsType->ItemIndex?ObsTypeText.c_str()+1:"";

    for (i=0;i<Obs.n;i++) {
        if (El[i]<ElMask*D2R) continue;
        if (ElMaskP&&El[i]<ElMaskData[(int)(Az[i]*R2D+0.5)]) continue;
        obs=&Obs.data[i];
        if (!SatSel[obs->sat-1]) continue;

        if (!GraphR->ToPoint(TimePos(obs->time),yp[obs->sat-1],ps[0])) continue;
        ps[1].x=ps[0].x;
        ps[1].y=ps[0].y+MarkSize*3/2+1;
        ps[0].y=ps[0].y-MarkSize*3/2;

        if (ShowHalfC) {
            slip=0;
            for (j=0;j<NFREQ+NEXOBS;j++) {
				if ((!*code||strstr(code2obs(obs->code[j],NULL),code))&&(obs->LLI[j]&2)) {
					slip=1;
					tempSatCount+=1;
                }
            }
            if (slip) GraphR->DrawPoly(ps,2,MColor[0][0],0);
        }
        if (ShowSlip) {
            slip=0;
			/*//if (ShowSlip==2) { // LLI
			if(ShowSlip) {
                for (j=0;j<NFREQ+NEXOBS;j++) {
					if ((!*code||strstr(code2obs(obs->code[j],NULL),code))&&(obs->LLI[j]&1)) {
						slip=1;
                    	tempSatCount+=1;
					}
					if(satsys(obs->sat,NULL)!=SYS_GAL && ((int)obs->code[j])!=0 &&CycleSlipMark[j][i]==1) {
                        slip=1;
						tempSatCount+=1;
                    }
                }
            }
			//else if (!*code||!strcmp(code,"1")||!strcmp(code,"2")) {
			if(!*code||!strcmp(code,"1")||!strcmp(code,"2")) {
				if(obs->L[0]!=0.0 && obs->L[1]!=0.0 &&
					satsys(obs->sat,NULL)!=SYS_GLO && satsys(obs->sat,NULL)!=SYS_CMP) {
					gf=CLIGHT*(obs->L[0]/FREQ1-obs->L[1]/FREQ2);
					if (fabs(gfp[obs->sat-1]-gf)>THRESLIP) {
                        tempSatCount+=1;
						slip=1;
                    }
					gfp[obs->sat-1]=gf;
				}
			}*/

            for (j=0;j<NFREQ+NEXOBS;j++) {
				if (CycleSlipMark[j][i]==1) {
					slip=1;
                }
            }

			if (slip) GraphR->DrawPoly(ps,2,MColor[0][5],0);
		}
	}
}
// draw ephemeris on observation-data-plot ----------------------------------
void __fastcall TrtkplotMainWindow::DrawObsEphem(double *yp)
{
    TPoint ps[3];
    int i,j,k,in,svh,off[MAXSAT]={0};

    trace(3,"DrawObsEphem\n");

    if (!ShowEph) return;

    for (i=0;i<MAXSAT;i++) {
        if (!SatSel[i]) continue;
        for (j=0;j<Nav.n;j++) {
            if (Nav.eph[j].sat!=i+1) continue;
            GraphR->ToPoint(TimePos(Nav.eph[j].ttr),yp[i],ps[0]);
            in=GraphR->ToPoint(TimePos(Nav.eph[j].toe),yp[i],ps[2]);
            ps[1]=ps[0];
            off[Nav.eph[j].sat-1]=off[Nav.eph[j].sat-1]?0:3;

            for (k=0;k<3;k++) ps[k].y+=MarkSize+2+off[Nav.eph[j].sat-1];
            ps[0].y-=2;

            svh=Nav.eph[j].svh;
            if (satsys(i+1,NULL)==SYS_QZS) svh&=0xFE; /* mask QZS LEX health */

            GraphR->DrawPoly(ps,3,svh?MColor[0][5]:CColor[1],0);

            if (in) GraphR->DrawMark(ps[2],0,svh?MColor[0][5]:CColor[1],svh?4:3,0);
        }
        for (j=0;j<Nav.ng;j++) {
            if (Nav.geph[j].sat!=i+1) continue;
            GraphR->ToPoint(TimePos(Nav.geph[j].tof),yp[i],ps[0]);
            in=GraphR->ToPoint(TimePos(Nav.geph[j].toe),yp[i],ps[2]);
            ps[1]=ps[0];
            off[Nav.geph[j].sat-1]=off[Nav.geph[j].sat-1]?0:3;
            for (k=0;k<3;k++) ps[k].y+=MarkSize+2+off[Nav.geph[j].sat-1];
            ps[0].y-=2;

            GraphR->DrawPoly(ps,3,Nav.geph[j].svh?MColor[0][5]:CColor[1],0);

            if (in) GraphR->DrawMark(ps[2],0,Nav.geph[j].svh?MColor[0][5]:CColor[1],
                                     Nav.geph[j].svh?4:3,0);
        }
        for (j=0;j<Nav.ns;j++) {
            if (Nav.seph[j].sat!=i+1) continue;
            GraphR->ToPoint(TimePos(Nav.seph[j].tof),yp[i],ps[0]);
            in=GraphR->ToPoint(TimePos(Nav.seph[j].t0),yp[i],ps[2]);
            ps[1]=ps[0];
            off[Nav.seph[j].sat-1]=off[Nav.seph[j].sat-1]?0:3;
            for (k=0;k<3;k++) ps[k].y+=MarkSize+2+off[Nav.seph[j].sat-1];
            ps[0].y-=2;

            GraphR->DrawPoly(ps,3,Nav.seph[j].svh?MColor[0][5]:CColor[1],0);

            if (in) GraphR->DrawMark(ps[2],0,Nav.seph[j].svh?MColor[0][5]:CColor[1],
                                     Nav.seph[j].svh?4:3,0);
        }
    }
}
// draw observation-data-plot -----------------------------------------------
void __fastcall TrtkplotMainWindow::DrawVisSat(int level)
{
    AnsiString label;
    TPoint p1,p2,p;
    gtime_t time;
    obsd_t *obs;
    double xs,ys,xt,xl[2],yl[2],tt[MAXSAT]={0},xp,xc,yc,yp[MAXSAT]={0};
    int i,j,m=0,sats[MAXSAT]={0},ind=ObsIndex,prn;
    char id[16];

    trace(3,"DrawObs: level=%d\n",level);

    for (i=0;i<Obs.n;i++) {
        if (SatMask[Obs.data[i].sat-1]) continue;
        sats[Obs.data[i].sat-1]=1;
    }
    for (i=0;i<MAXSAT;i++) if (sats[i]) m++;

    GraphR->XLPos=TimeLabel?6:1;
    GraphR->YLPos=0;
    GraphR->Week=Week;
    GraphR->GetLim(xl,yl);
    yl[0]=0.5;
    yl[1]=m>0?m+0.5:m+10.5;
    GraphR->SetLim(xl,yl);
    GraphR->SetTick(0.0,1.0);

	//if (0<=ind&&ind<NObs&&BtnShowTrack->Down&&BtnFixHoriz->Down) {
	bool btnfixhorizdown = (BtnShowTrack->Down)?0:1;
	if (0<=ind&&ind<NObs&&BtnShowTrack->Down && btnfixhorizdown) {
        xp=TimePos(Obs.data[IndexObs[ind]].time);
		//if (BtnFixHoriz->Down) {
		if(btnfixhorizdown) {
            double xl[2],yl[2],off;
            GraphR->GetLim(xl,yl);
            off=Xcent*(xl[1]-xl[0])/2.0;
            GraphR->GetCent(xc,yc);
            GraphR->SetCent(xp-off,yc);
        }
        else {
            GraphR->GetRight(xc,yc);
            GraphR->SetRight(xp,yc);
        }
    }
    GraphR->DrawAxis(1,1);
    GraphR->GetPos(p1,p2);

    for (i=0,j=0;i<MAXSAT;i++) {
        if (!sats[i]) continue;
        p.x=p1.x;
        p.y=p1.y+(int)((p2.y-p1.y)*(j+0.5)/m);
        yp[i]=m-(j++);
        satno2id(i+1,id);
        label=id;
        GraphR->DrawText(p,label,CColor[2],2,0,0);
    }
    p1.x=Disp->Font->Size;
    p1.y=(p1.y+p2.y)/2;
	//GraphR->DrawText(p1,"SATELLITE NO",CColor[2],0,0,90);
    GraphR->DrawText(p1,"卫星号",CColor[2],0,0,90);

	//if (!BtnSol1->Down)
	if(!(NObs>0))
		return;

    if (level&&PlotStyle<=2) {
        DrawObsEphem(yp);
    }
    if (level&&PlotStyle<=2) {
        GraphR->GetScale(xs,ys);
        for (i=0;i<Obs.n;i++) {
            obs=&Obs.data[i];
            TColor col=ObsColor(obs,Az[i],El[i]);
            if (col==clBlack) continue;

            xt=TimePos(obs->time);
            if (fabs(xt-tt[obs->sat-1])/xs>0.9) {
                GraphR->DrawMark(xt,yp[obs->sat-1],0,PlotStyle<2?col:CColor[3],
                                 PlotStyle<2?MarkSize:0,0);
                tt[obs->sat-1]=xt;
            }
        }
    }
    if (level&&PlotStyle<=2) {
        DrawObsSlip(yp);
    }
    if (BtnShowTrack->Down&&0<=ind&&ind<NObs) {
        i=IndexObs[ind];
        time=Obs.data[i].time;

        GraphR->GetLim(xl,yl);
        xl[0]=xl[1]=TimePos(Obs.data[i].time);
        GraphR->DrawPoly(xl,yl,2,CColor[2],0);

        for (;i<Obs.n&&timediff(Obs.data[i].time,time)==0.0;i++) {
            obs=&Obs.data[i];
            TColor col=ObsColor(obs,Az[i],El[i]);
            if (col==clBlack) continue;
            GraphR->DrawMark(xl[0],yp[obs->sat-1],0,col,MarkSize*2+2,0);
        }
        GraphR->DrawMark(xl[0],yl[1]-1E-6,0,CColor[2],5,0);
		//if (!BtnFixHoriz->Down) {
		if(!btnfixhorizdown){
            GraphR->DrawMark(xl[0],yl[1]-1E-6,1,CColor[2],9,0);
        }
    }
}
// draw polyline without time-gaps ------------------------------------------
void __fastcall TrtkplotMainWindow::DrawPolyS(TGraph *graph, double *x, double *y, int n,
    TColor color, int style)
{
    int i,j;

    for (i=0;i<n;i=j) {
        for (j=i+1;j<n;j++) if (fabs(x[j]-x[j-1])>TBRK) break;
        graph->DrawPoly(x+i,y+i,j-i,color,style);
    }
}
//draw CNR plot
void __fastcall TrtkplotMainWindow::DrawCNR(int level)
{
	AnsiString s,ObsTypeText=ObsType2->Text;
	//AnsiString label[]={"SNR","Multipath","Elevation"};
	AnsiString label[] = {"载噪比","多路径误差","高度角"};
	AnsiString unit[]={"dBHz","m",CHARDEG};
	TPoint p1,p2;
	TColor *col,colp[MAXSAT];
	gtime_t time={0};
	double *x,*y,xl[2],yl[2],off,xc,yc,xp,yp[MAXSAT],ave[3]={0},rms[3]={0};
	char *code=ObsTypeText.c_str()+1;
	int i,j,k,l,n,np,c,sat,ind=ObsIndex,nrms[3]={0};

	trace(3,"DrawSnr: level=%d\n",level);

	if (0<=ind&&ind<NObs&&BtnShowTrack->Down) {
		time=Obs.data[IndexObs[ind]].time;
	}

	bool btnfixhorizdown = (BtnShowTrack->Down)?0:1;
	if(0<=ind && ind<NObs && BtnShowTrack->Down && btnfixhorizdown) {
		GraphG[0]->GetLim(xl,yl);
        off=Xcent*(xl[1]-xl[0])/2.0;
        xp=TimePos(time);
        for (j=0;j<3;j++) {
            GraphG[j]->GetCent(xc,yc);
            GraphG[j]->SetCent(xp-off,yc);
        }
    }

	p1.x=0;p1.y=0; p2.x =Disp->Width; p2.y= Disp->Height;
	int h,tmargin,bmargin,rmargin,lmargin;
    tmargin=5;                             // top margin
    bmargin=(int)(Disp->Font->Size*1.5)+3; // bottom
    rmargin=8;                             // right
	lmargin=Disp->Font->Size*3+15;         // left
	p1.x+=lmargin; p1.y+=tmargin;
	p2.x-=rmargin; p2.y=p2.y-bmargin;
	p1.y=tmargin; p2.y=p1.y;
	h=(Disp->Height-tmargin-bmargin);
	p2.y+=h;

	j = 0;
	for (i=0;i<3;i++) {
		if(i>0)
			continue;
		GraphG[i]->SetPos(p1,p2);
		GraphG[i]->XLPos=TimeLabel?(i==j?6:5):(i==j?1:0);
        GraphG[i]->Week=Week;
        GraphG[i]->DrawAxis(ShowLabel,ShowLabel);
	}
	bool btnsol1 = (NObs>0)?1:0;
    if(NObs>0 && btnsol1) {
        x=new double[NObs],
        y=new double[NObs];
        col=new TColor[NObs];

		for (i=l=0;i<3;i++) {
			if(i>0) continue;
            for (sat=1,np=0;sat<=MAXSAT;sat++) {
                if (SatMask[sat-1]||!SatSel[sat-1]) continue;

                for (j=n=0;j<Obs.n;j++) {
                    if (Obs.data[j].sat!=sat) continue;

                    for (k=0;k<NFREQ+NEXOBS;k++) {
                        if (strstr(code2obs(Obs.data[j].code[k],NULL),code)) break;
                    }
                    if (k>=NFREQ+NEXOBS) continue;

                    x[n]=TimePos(Obs.data[j].time);
                    if (i==0) {
                        y[n]=Obs.data[j].SNR[k]*0.25;
                        col[n]=MColor[0][4];
                    }
                    else if (i==1) {
                        if (!Mp[k]||Mp[k][j]==0.0) continue;
                        y[n]=Mp[k][j];
                        col[n]=MColor[0][4];
                    }
                    else {
                        y[n]=El[j]*R2D;
						if (SimObs) col[n]=SysColor(Obs.data[j].sat);
                        else col[n]=SnrColor(Obs.data[j].SNR[k]*0.25);
                        if (El[j]>0.0&&El[j]<ElMask*D2R) col[n]=MColor[0][0];
                    }
                    if (timediff(time,Obs.data[j].time)==0.0&&np<MAXSAT) {
                        yp[np]=y[n];
                        colp[np++]=col[n];
                    }
                    if (n<NObs) n++;
                }
                if (!level||!(PlotStyle%2)) {
                    for (j=0;j<n;j=k) {
                        for (k=j+1;k<n;k++) if (fabs(y[k-1]-y[k])>30.0) break;
                        DrawPolyS(GraphG[i],x+j,y+j,k-j,CColor[3],0);
                    }
                }
                if (level&&PlotStyle<2) {
                    for (j=0;j<n;j++) {
                        if (i!=1&&y[j]<=0.0) continue;
                        GraphG[i]->DrawMark(x[j],y[j],0,col[j],MarkSize,0);
                    }
                }
                for (j=0;j<n;j++) {
                    if (y[j]==0.0) continue;
                    ave[i]+=y[j];
                    rms[i]+=SQR(y[j]);
                    nrms[i]++;
                }
            }
            if (level&&i==1&&nrms[i]>0&&ShowStats&&!BtnShowTrack->Down) {
                ave[i]=ave[i]/nrms[i];
                rms[i]=sqrt(rms[i]/nrms[i]);
                GraphG[i]->GetPos(p1,p2);
                p1.x=p2.x-8; p1.y+=3;
                DrawLabel(GraphG[i],p1,s.sprintf("AVE=%.4fm RMS=%.4fm",ave[i],
						  rms[i]),2,2);
			}
			if (BtnShowTrack->Down&&0<=ind&&ind<NObs&&btnsol1) {
				if(i>0) continue;
                GraphG[i]->GetLim(xl,yl);
                xl[0]=xl[1]=TimePos(time);
                GraphG[i]->DrawPoly(xl,yl,2,CColor[2],0);

                if (l++==0) {
                    GraphG[i]->DrawMark(xl[0],yl[1]-1E-6,0,CColor[2],5,0);

					if (!btnfixhorizdown) {
                        GraphG[i]->DrawMark(xl[0],yl[1]-1E-6,1,CColor[2],9,0);
                    }
                }
                for (k=0;k<np;k++) {
                    if (i!=1&&yp[k]<=0.0) continue;
                    GraphG[i]->DrawMark(xl[0],yp[k],0,CColor[0],MarkSize*2+4,0);
                    GraphG[i]->DrawMark(xl[0],yp[k],0,colp[k],MarkSize*2+2,0);
                }
                if (np<=0||np>1||(i!=1&&yp[0]<=0.0)) continue;

                GraphG[i]->GetPos(p1,p2);
                p1.x=p2.x-8; p1.y+=3;
				DrawLabel(GraphG[i],p1,s.sprintf("%.*f %s",i==1?4:1,yp[0],unit[i]),2,2);
            }
        }
        delete [] x;
        delete [] y;
        delete [] col;
    }
    for (i=0;i<3;i++) {
		if(i>0) continue;
        GraphG[i]->GetPos(p1,p2);
        p1.x+=5; p1.y+=3;
        DrawLabel(GraphG[i],p1,s.sprintf("%s (%s)",label[i],unit[i]),1,2);
    }
}
//draw MP plot
void __fastcall TrtkplotMainWindow::DrawMP(int level)
{
	AnsiString s,ObsTypeText=ObsType2->Text;
	//AnsiString label[]={"SNR","Multipath","Elevation"};
    AnsiString label[] = {"载噪比","多路径误差","高度角"};
	AnsiString unit[]={"dBHz","m",CHARDEG};
	TPoint p1,p2;
	TColor *col,colp[MAXSAT];
	gtime_t time={0};
	double *x,*y,xl[2],yl[2],off,xc,yc,xp,yp[MAXSAT],ave[3]={0},rms[3]={0};
	char *code=ObsTypeText.c_str()+1;
	int i,j,k,l,n,np,c,sat,ind=ObsIndex,nrms[3]={0};

	trace(3,"DrawSnr: level=%d\n",level);

	if (0<=ind&&ind<NObs&&BtnShowTrack->Down) {
		time=Obs.data[IndexObs[ind]].time;
	}

	bool btnfixhorizdown = (BtnShowTrack->Down)?0:1;
	if(0<=ind && ind<NObs && BtnShowTrack->Down && btnfixhorizdown) {
		GraphG[0]->GetLim(xl,yl);
        off=Xcent*(xl[1]-xl[0])/2.0;
        xp=TimePos(time);
        for (j=0;j<3;j++) {
            GraphG[j]->GetCent(xc,yc);
            GraphG[j]->SetCent(xp-off,yc);
        }
	}

	p1.x=0;p1.y=0; p2.x =Disp->Width; p2.y= Disp->Height;
	int h,tmargin,bmargin,rmargin,lmargin;
    tmargin=5;                             // top margin
    bmargin=(int)(Disp->Font->Size*1.5)+3; // bottom
    rmargin=8;                             // right
	lmargin=Disp->Font->Size*3+15;         // left
	p1.x+=lmargin; p1.y+=tmargin;
	p2.x-=rmargin; p2.y=p2.y-bmargin;
	p1.y=tmargin; p2.y=p1.y;
	h=(Disp->Height-tmargin-bmargin);
	p2.y+=h;

	j = 1;
	for (i=0;i<3;i++) {
		if(i==0 || i==2) continue;
		GraphG[i]->SetPos(p1,p2);
        GraphG[i]->XLPos=TimeLabel?(i==j?6:5):(i==j?1:0);
        GraphG[i]->Week=Week;
        GraphG[i]->DrawAxis(ShowLabel,ShowLabel);
	}
	bool btnsol1 = (NObs>0)?1:0;
	if(NObs>0 && btnsol1) {
        x=new double[NObs],
        y=new double[NObs];
        col=new TColor[NObs];

		for (i=l=0;i<3;i++) {
			if(i==0 || i==2) continue;
            for (sat=1,np=0;sat<=MAXSAT;sat++) {
                if (SatMask[sat-1]||!SatSel[sat-1]) continue;

                for (j=n=0;j<Obs.n;j++) {
                    if (Obs.data[j].sat!=sat) continue;

                    for (k=0;k<NFREQ+NEXOBS;k++) {
                        if (strstr(code2obs(Obs.data[j].code[k],NULL),code)) break;
                    }
                    if (k>=NFREQ+NEXOBS) continue;

                    x[n]=TimePos(Obs.data[j].time);
                    if (i==0) {
                        y[n]=Obs.data[j].SNR[k]*0.25;
                        col[n]=MColor[0][4];
                    }
                    else if (i==1) {
                        if (!Mp[k]||Mp[k][j]==0.0) continue;
                        y[n]=Mp[k][j];
                        col[n]=MColor[0][4];
                    }
                    else {
                        y[n]=El[j]*R2D;
                        if (SimObs) col[n]=SysColor(Obs.data[j].sat);
                        else col[n]=SnrColor(Obs.data[j].SNR[k]*0.25);
                        if (El[j]>0.0&&El[j]<ElMask*D2R) col[n]=MColor[0][0];
                    }
                    if (timediff(time,Obs.data[j].time)==0.0&&np<MAXSAT) {
                        yp[np]=y[n];
                        colp[np++]=col[n];
                    }
                    if (n<NObs) n++;
                }
                if (!level||!(PlotStyle%2)) {
                    for (j=0;j<n;j=k) {
                        for (k=j+1;k<n;k++) if (fabs(y[k-1]-y[k])>30.0) break;
                        DrawPolyS(GraphG[i],x+j,y+j,k-j,CColor[3],0);
                    }
                }
                if (level&&PlotStyle<2) {
                    for (j=0;j<n;j++) {
                        if (i!=1&&y[j]<=0.0) continue;
                        GraphG[i]->DrawMark(x[j],y[j],0,col[j],MarkSize,0);
                    }
                }
                for (j=0;j<n;j++) {
                    if (y[j]==0.0) continue;
                    ave[i]+=y[j];
                    rms[i]+=SQR(y[j]);
                    nrms[i]++;
                }
            }
            if (level&&i==1&&nrms[i]>0&&ShowStats&&!BtnShowTrack->Down) {
                ave[i]=ave[i]/nrms[i];
                rms[i]=sqrt(rms[i]/nrms[i]);
                GraphG[i]->GetPos(p1,p2);
                p1.x=p2.x-8; p1.y+=3;
                DrawLabel(GraphG[i],p1,s.sprintf("AVE=%.4fm RMS=%.4fm",ave[i],
                          rms[i]),2,2);
			}
			if (BtnShowTrack->Down&&0<=ind&&ind<NObs&&btnsol1) {
				if(i==0 || i==2) continue;
                GraphG[i]->GetLim(xl,yl);
                xl[0]=xl[1]=TimePos(time);
                GraphG[i]->DrawPoly(xl,yl,2,CColor[2],0);

                if (l++==0) {
                    GraphG[i]->DrawMark(xl[0],yl[1]-1E-6,0,CColor[2],5,0);

					if (!btnfixhorizdown) {
                        GraphG[i]->DrawMark(xl[0],yl[1]-1E-6,1,CColor[2],9,0);
                    }
                }
                for (k=0;k<np;k++) {
                    if (i!=1&&yp[k]<=0.0) continue;
                    GraphG[i]->DrawMark(xl[0],yp[k],0,CColor[0],MarkSize*2+4,0);
                    GraphG[i]->DrawMark(xl[0],yp[k],0,colp[k],MarkSize*2+2,0);
                }
                if (np<=0||np>1||(i!=1&&yp[0]<=0.0)) continue;

                GraphG[i]->GetPos(p1,p2);
                p1.x=p2.x-8; p1.y+=3;
                DrawLabel(GraphG[i],p1,s.sprintf("%.*f %s",i==1?4:1,yp[0],unit[i]),2,2);
            }
        }
        delete [] x;
        delete [] y;
        delete [] col;
    }
    for (i=0;i<3;i++) {
		if(i==0 || i==2) continue;
        GraphG[i]->GetPos(p1,p2);
        p1.x+=5; p1.y+=3;
        DrawLabel(GraphG[i],p1,s.sprintf("%s (%s)",label[i],unit[i]),1,2);
    }
}
//draw Ele plot
void __fastcall TrtkplotMainWindow::DrawEle(int level)
{
	AnsiString s,ObsTypeText=ObsType2->Text;
	//AnsiString label[]={"SNR","Multipath","Elevation"};
    AnsiString label[] = {"载噪比","多路径误差","高度角"};
	AnsiString unit[]={"dBHz","m",CHARDEG};
	TPoint p1,p2;
	TColor *col,colp[MAXSAT];
	gtime_t time={0};
	double *x,*y,xl[2],yl[2],off,xc,yc,xp,yp[MAXSAT],ave[3]={0},rms[3]={0};
	char *code=ObsTypeText.c_str()+1;
	int i,j,k,l,n,np,c,sat,ind=ObsIndex,nrms[3]={0};

	trace(3,"DrawSnr: level=%d\n",level);

	if (0<=ind&&ind<NObs&&BtnShowTrack->Down) {
		time=Obs.data[IndexObs[ind]].time;
	}

	bool btnfixhorizdown = (BtnShowTrack->Down)?0:1;
	if(0<=ind && ind<NObs && BtnShowTrack->Down && btnfixhorizdown) {
		GraphG[0]->GetLim(xl,yl);
        off=Xcent*(xl[1]-xl[0])/2.0;
        xp=TimePos(time);
        for (j=0;j<3;j++) {
            GraphG[j]->GetCent(xc,yc);
            GraphG[j]->SetCent(xp-off,yc);
        }
	}

	p1.x=0;p1.y=0; p2.x =Disp->Width; p2.y= Disp->Height;
	int h,tmargin,bmargin,rmargin,lmargin;
    tmargin=5;                             // top margin
    bmargin=(int)(Disp->Font->Size*1.5)+3; // bottom
    rmargin=8;                             // right
	lmargin=Disp->Font->Size*3+15;         // left
	p1.x+=lmargin; p1.y+=tmargin;
	p2.x-=rmargin; p2.y=p2.y-bmargin;
	p1.y=tmargin; p2.y=p1.y;
	h=(Disp->Height-tmargin-bmargin);
	p2.y+=h;

	j = 2;
	for (i=0;i<3;i++) {
		if(i==0 || i==1) continue;
		GraphG[i]->SetPos(p1,p2);
        GraphG[i]->XLPos=TimeLabel?(i==j?6:5):(i==j?1:0);
        GraphG[i]->Week=Week;
        GraphG[i]->DrawAxis(ShowLabel,ShowLabel);
	}
	bool btnsol1 = (NObs>0)?1:0;
    if(NObs>0 && btnsol1) {
        x=new double[NObs],
        y=new double[NObs];
        col=new TColor[NObs];

		for (i=l=0;i<3;i++) {
			if(i==0 || i==1) continue;
            for (sat=1,np=0;sat<=MAXSAT;sat++) {
                if (SatMask[sat-1]||!SatSel[sat-1]) continue;

                for (j=n=0;j<Obs.n;j++) {
                    if (Obs.data[j].sat!=sat) continue;

                    for (k=0;k<NFREQ+NEXOBS;k++) {
                        if (strstr(code2obs(Obs.data[j].code[k],NULL),code)) break;
                    }
                    if (k>=NFREQ+NEXOBS) continue;

                    x[n]=TimePos(Obs.data[j].time);
                    if (i==0) {
                        y[n]=Obs.data[j].SNR[k]*0.25;
                        col[n]=MColor[0][4];
                    }
                    else if (i==1) {
                        if (!Mp[k]||Mp[k][j]==0.0) continue;
                        y[n]=Mp[k][j];
                        col[n]=MColor[0][4];
                    }
                    else {
                        y[n]=El[j]*R2D;
                        if (SimObs) col[n]=SysColor(Obs.data[j].sat);
                        else col[n]=SnrColor(Obs.data[j].SNR[k]*0.25);
                        if (El[j]>0.0&&El[j]<ElMask*D2R) col[n]=MColor[0][0];
                    }
                    if (timediff(time,Obs.data[j].time)==0.0&&np<MAXSAT) {
                        yp[np]=y[n];
                        colp[np++]=col[n];
                    }
                    if (n<NObs) n++;
                }
                if (!level||!(PlotStyle%2)) {
                    for (j=0;j<n;j=k) {
                        for (k=j+1;k<n;k++) if (fabs(y[k-1]-y[k])>30.0) break;
                        DrawPolyS(GraphG[i],x+j,y+j,k-j,CColor[3],0);
                    }
                }
                if (level&&PlotStyle<2) {
                    for (j=0;j<n;j++) {
                        if (i!=1&&y[j]<=0.0) continue;
                        GraphG[i]->DrawMark(x[j],y[j],0,col[j],MarkSize,0);
                    }
                }
                for (j=0;j<n;j++) {
                    if (y[j]==0.0) continue;
                    ave[i]+=y[j];
                    rms[i]+=SQR(y[j]);
                    nrms[i]++;
                }
            }
            if (level&&i==1&&nrms[i]>0&&ShowStats&&!BtnShowTrack->Down) {
                ave[i]=ave[i]/nrms[i];
                rms[i]=sqrt(rms[i]/nrms[i]);
                GraphG[i]->GetPos(p1,p2);
                p1.x=p2.x-8; p1.y+=3;
                DrawLabel(GraphG[i],p1,s.sprintf("AVE=%.4fm RMS=%.4fm",ave[i],
                          rms[i]),2,2);
			}
			if (BtnShowTrack->Down&&0<=ind&&ind<NObs&&btnsol1) {
				if(i==0 || i==1) continue;
				GraphG[i]->GetLim(xl,yl);
                xl[0]=xl[1]=TimePos(time);
                GraphG[i]->DrawPoly(xl,yl,2,CColor[2],0);

                if (l++==0) {
                    GraphG[i]->DrawMark(xl[0],yl[1]-1E-6,0,CColor[2],5,0);

					if (!btnfixhorizdown) {
                        GraphG[i]->DrawMark(xl[0],yl[1]-1E-6,1,CColor[2],9,0);
                    }
                }
                for (k=0;k<np;k++) {
                    if (i!=1&&yp[k]<=0.0) continue;
                    GraphG[i]->DrawMark(xl[0],yp[k],0,CColor[0],MarkSize*2+4,0);
                    GraphG[i]->DrawMark(xl[0],yp[k],0,colp[k],MarkSize*2+2,0);
                }
                if (np<=0||np>1||(i!=1&&yp[0]<=0.0)) continue;

                GraphG[i]->GetPos(p1,p2);
                p1.x=p2.x-8; p1.y+=3;
                DrawLabel(GraphG[i],p1,s.sprintf("%.*f %s",i==1?4:1,yp[0],unit[i]),2,2);
            }
        }
        delete [] x;
        delete [] y;
        delete [] col;
    }
    for (i=0;i<3;i++) {
		if(i==0 || i==1) continue;
        GraphG[i]->GetPos(p1,p2);
        p1.x+=5; p1.y+=3;
        DrawLabel(GraphG[i],p1,s.sprintf("%s (%s)",label[i],unit[i]),1,2);
    }
}
// draw sky-plot-CNR ------------------------------------------------------------
void __fastcall TrtkplotMainWindow::DrawSkyCNR(int level)
{
    TPoint p1,p2;
    AnsiString s,ss,ObsTypeText=ObsType->Text;
    obsd_t *obs;
    gtime_t t[MAXSAT]={{0}};
    double p[MAXSAT][2]={{0}},gfp[MAXSAT]={0},p0[MAXSAT][2]={{0}};
    double x,y,xp,yp,xs,ys,dt,dx,dy,xl[2],yl[2],r,gf;
    int i,j,ind=ObsIndex;
    int hh=(int)(Disp->Font->Size*1.5),prn,color,slip;
    const char *code=ObsType->ItemIndex?ObsTypeText.c_str()+1:"";
    char id[16];

    trace(3,"DrawSky: level=%d\n",level);

    GraphS->GetLim(xl,yl);
    r=(xl[1]-xl[0]<yl[1]-yl[0]?xl[1]-xl[0]:yl[1]-yl[0])*0.45;

	//if (BtnShowMap->Down) {
	//    DrawSkyImage(level);
    //}
    if (BtnShowSkyplot->Down) {
        GraphS->DrawSkyPlot(0.0,0.0,CColor[1],CColor[2],CColor[0],r*2.0);
    }
	//if (!BtnSol1->Down) return;
	if(!(NObs>0)) return;

    GraphS->GetScale(xs,ys);

    if (PlotStyle<=2) {
        for (i=0;i<Obs.n;i++) {
            obs=&Obs.data[i];
            if (SatMask[obs->sat-1]||!SatSel[obs->sat-1]||El[i]<=0.0) continue;
            TColor col=ObsColor(obs,Az[i],El[i]);
            if (col==clBlack) continue;

            x =r*sin(Az[i])*(1.0-2.0*El[i]/PI);
            y =r*cos(Az[i])*(1.0-2.0*El[i]/PI);
            xp=p[obs->sat-1][0];
            yp=p[obs->sat-1][1];

            if ((x-xp)*(x-xp)+(y-yp)*(y-yp)>=xs*xs) {
                int siz=PlotStyle<2?MarkSize:1;
                GraphS->DrawMark(x,y,0,PlotStyle<2?col:CColor[3],siz,0);
                p[obs->sat-1][0]=x;
                p[obs->sat-1][1]=y;
            }
            if (xp==0.0&&yp==0.0) {
                p0[obs->sat-1][0]=x;
                p0[obs->sat-1][1]=y;
            }
        }
    }
    if ((PlotStyle==0||PlotStyle==2)&&!BtnShowTrack->Down) {

        for (i=0;i<MAXSAT;i++) {
            if (p0[i][0]!=0.0||p0[i][1]!=0.0) {
                TPoint pnt;
                if (GraphS->ToPoint(p0[i][0],p0[i][1],pnt)) {
                    satno2id(i+1,id); s=id;
                    DrawLabel(GraphS,pnt,s,1,0);
                }
            }
        }
    }
    if (!level) return;

    if (ShowSlip&&PlotStyle<=2) {

        for (i=0;i<Obs.n;i++) {
            obs=&Obs.data[i];
            if (SatMask[obs->sat-1]||!SatSel[obs->sat-1]||El[i]<=0.0) continue;

            slip=0;
            if (ShowSlip==2) { // LLI
                for (j=0;j<NFREQ+NEXOBS;j++) {
                    if ((!*code||strstr(code2obs(obs->code[j],NULL),code))&&
                        (obs->LLI[j]&1)) slip=1;
                }
            }
            else if (!*code||!strcmp(code,"1")||!strcmp(code,"2")) {
                if (obs->L[0]!=0.0&&obs->L[1]!=0.0&&
                    satsys(obs->sat,NULL)!=SYS_GLO) {
                    gf=CLIGHT*(obs->L[0]/FREQ1-obs->L[1]/FREQ2);
                    if (fabs(gfp[obs->sat-1]-gf)>THRESLIP) slip=1;
                    gfp[obs->sat-1]=gf;
                }
            }
            x=r*sin(Az[i])*(1.0-2.0*El[i]/PI);
            y=r*cos(Az[i])*(1.0-2.0*El[i]/PI);
            dt=timediff(obs->time,t[obs->sat-1]);
            dx=x-p[obs->sat-1][0];
            dy=y-p[obs->sat-1][1];
            t[obs->sat-1]=obs->time;
            p[obs->sat-1][0]=x;
            p[obs->sat-1][1]=y;
            if (fabs(dt)>300.0) continue;
            if (El[i]<ElMask*D2R) continue;
            if (ElMaskP&&El[i]<ElMaskData[(int)(Az[i]*R2D+0.5)]) continue;
            if (slip) {
                GraphS->DrawMark(x,y,4,MColor[0][5],MarkSize*3+2,ATAN2(dy,dx)*R2D+90);
            }
        }
    }
    if (ElMaskP) {
        double *x=new double [361];
        double *y=new double [361];
        for (i=0;i<=360;i++) {
            x[i]=r*sin(i*D2R)*(1.0-2.0*ElMaskData[i]/PI);
            y[i]=r*cos(i*D2R)*(1.0-2.0*ElMaskData[i]/PI);
        }
        GraphS->DrawPoly(x,y,361,CColor[1],0);
        delete [] x;
        delete [] y;
    }
    if (BtnShowTrack->Down&&0<=ind&&ind<NObs) {

        for (i=IndexObs[ind];i<Obs.n&&i<IndexObs[ind+1];i++) {
            obs=&Obs.data[i];
            if (SatMask[obs->sat-1]||!SatSel[obs->sat-1]||El[i]<=0.0) continue;
            TColor col=ObsColor(obs,Az[i],El[i]);
            if (col==clBlack) continue;

            x=r*sin(Az[i])*(1.0-2.0*El[i]/PI);
            y=r*cos(Az[i])*(1.0-2.0*El[i]/PI);

            satno2id(obs->sat,id);
            GraphS->DrawMark(x,y,0,col,Disp->Font->Size*2+5,0);
            GraphS->DrawMark(x,y,1,col==clBlack?MColor[0][0]:CColor[2],Disp->Font->Size*2+5,0);
            GraphS->DrawText(x,y,s=id,CColor[0],0,0,0);
        }
    }
    GraphS->GetPos(p1,p2);
    p1.x+=10; p1.y+=8; p2.x-=10; p2.y=p1.y;

    if (ShowStats&&!SimObs) {
        s.sprintf("MARKER: %s %s",Sta.name,Sta.marker);
        DrawLabel(GraphS,p1,s,1,2); p1.y+=hh;
        s.sprintf("REC: %s %s %s",Sta.rectype,Sta.recver,Sta.recsno);
        DrawLabel(GraphS,p1,s,1,2); p1.y+=hh;
        s.sprintf("ANT: %s %s",Sta.antdes,Sta.antsno);
        DrawLabel(GraphS,p1,s,1,2); p1.y+=hh;
    }
    // show statistics
    if (ShowStats&&BtnShowTrack->Down&&0<=ind&&ind<NObs&&!SimObs) {

        s.sprintf("SAT: OBS : SNR : LLI%s",!*code?"":" : CODE");
        DrawLabel(GraphS,p2,s,2,2);
        p2.y+=3;

        for (i=IndexObs[ind];i<Obs.n&&i<IndexObs[ind+1];i++) {
            obs=&Obs.data[i];
            if (SatMask[obs->sat-1]||!SatSel[obs->sat-1]) continue;
            if (HideLowSat&&El[i]<ElMask*D2R) continue;
            if (HideLowSat&&ElMaskP&&El[i]<ElMaskData[(int)(Az[i]*R2D+0.5)]) continue;

            satno2id(obs->sat,id);
            s.sprintf("%-3s: ",id);

            if (!*code) {
                for (j=0;j<NFREQ;j++) s+=obs->P[j]==0.0?"-":"C";
                for (j=0;j<NFREQ;j++) s+=obs->L[j]==0.0?"-":"L";
                s+=" : ";
                for (j=0;j<NFREQ;j++) s+=ss.sprintf("%02.0f ",obs->SNR[j]*0.25);
                s+=": ";
                for (j=0;j<NFREQ;j++) s+=ss.sprintf("%d",obs->LLI[j]);
            }
            else {
                for (j=0;j<NFREQ+NEXOBS;j++) {
                    if (strstr(code2obs(obs->code[j],NULL),code)) break;
                }
                if (j>=NFREQ+NEXOBS) continue;

                s+=ss.sprintf("%s%s%s : %04.1f : %d : %s",obs->P[j]==0.0?"-":"C",
                              obs->L[j]==0.0?"-":"L",obs->D[j]==0.0?"-":"D",
                              obs->SNR[j]*0.25,obs->LLI[j],
                              code2obs(obs->code[j],NULL));
            }
            TColor col=ObsColor(obs,Az[i],El[i]);
            p2.y+=hh;
            GraphS->DrawText(p2,s,col==clBlack?MColor[0][0]:col,2,2,0);
        }
    }
    if (Nav.n<=0&&Nav.ng<=0&&!SimObs) {
        GraphS->GetPos(p1,p2);
        p2.x-=10;
        p2.y-=3;
        DrawLabel(GraphS,p2,"No Navigation Data",2,1);
    }
}
// draw statistics on dop and number-of-satellite plot ----------------------
void __fastcall TrtkplotMainWindow::DrawDopStat(double *dop, int *ns, int n)
{
    AnsiString s0[MAXOBS+2],s1[MAXOBS+2],s2[MAXOBS+2];
    TPoint p1,p2,p3,p4;
    double ave[4]={0};
    int i,j,m=0;
    int ndop[4]={0},nsat[MAXOBS]={0},fonth=(int)(Disp->Font->Size*1.5);

    trace(3,"DrawDopStat: n=%d\n",n);

    if (!ShowStats) return;

    for (i=0;i<n;i++) nsat[ns[i]]++;

    for (i=0;i<4;i++) {
        for (j=0;j<n;j++) {
            if (dop[i+j*4]<=0.0||dop[i+j*4]>MaxDop) continue;
            ave[i]+=dop[i+j*4];
            ndop[i]++;
        }
        if (ndop[i]>0) ave[i]/=ndop[i];
    }
    if (DopType->ItemIndex==0||DopType->ItemIndex>=2) {
        s2[m++].sprintf("AVE= GDOP:%4.1f PDOP:%4.1f HDOP:%4.1f VDOP:%4.1f",
                        ave[0],ave[1],ave[2],ave[3]);
        s2[m++].sprintf("NDOP=%d(%4.1f%%) %d(%4.1f%%) %d(%4.1f%%) %d(%4.1f%%)",
                        ndop[0],n>0?ndop[0]*100.0/n:0.0,
                        ndop[1],n>0?ndop[1]*100.0/n:0.0,
                        ndop[2],n>0?ndop[2]*100.0/n:0.0,
                        ndop[3],n>0?ndop[3]*100.0/n:0.0);
    }
    if (DopType->ItemIndex<=1) {

        for (i=0,j=0;i<MAXOBS;i++) {
            if (nsat[i]<=0) continue;
            s0[m].sprintf("%s%2d:",j++==0?"NSAT= ":"",i);
            s1[m].sprintf("%7d",nsat[i]);
            s2[m++].sprintf("(%4.1f%%)",nsat[i]*100.0/n);
        }
    }
    GraphR->GetPos(p1,p2);
    p1.x=p2.x-10;
    p1.y+=8;
    p2=p1; p2.x-=fonth*4;
    p3=p2; p3.x-=fonth*8;

    for (i=0;i<m;i++,p1.y+=fonth,p2.y+=fonth,p3.y+=fonth) {
        DrawLabel(GraphR,p3,s0[i],2,2);
        DrawLabel(GraphR,p2,s1[i],2,2);
        DrawLabel(GraphR,p1,s2[i],2,2);
    }
}
// draw mp-skyplot ----------------------------------------------------------
void __fastcall TrtkplotMainWindow::DrawSkyMP(int level)
{
    AnsiString ObsTypeText=ObsType2->Text,s;
    TColor col;
    obsd_t *obs;
    double x,y,xp,yp,xs,ys,xl[2],yl[2],p[MAXSAT][2]={{0}},r;
    int i,j,sat,ind=ObsIndex;
    char *code=ObsTypeText.c_str()+1,id[32];

    trace(3,"DrawSnrS: level=%d\n",level);

    GraphS->GetLim(xl,yl);
    r=(xl[1]-xl[0]<yl[1]-yl[0]?xl[1]-xl[0]:yl[1]-yl[0])*0.45;

	//if (BtnShowMap->Down) {
	//    DrawSkyImage(level);
	//}
    if (BtnShowSkyplot->Down) {
        GraphS->DrawSkyPlot(0.0,0.0,CColor[1],CColor[2],CColor[0],r*2.0);
    }

	//if (!BtnSol1->Down||NObs<=0||SimObs) return;
    if(NObs<=0 || SimObs) return;

    GraphS->GetScale(xs,ys);

    for (sat=1;sat<=MAXSAT;sat++) {
        if (SatMask[sat-1]||!SatSel[sat-1]) continue;

        for (i=0;i<Obs.n;i++) {
            if (Obs.data[i].sat!=sat) continue;

            for (j=0;j<NFREQ+NEXOBS;j++) {
                if (strstr(code2obs(Obs.data[i].code[j],NULL),code)) break;
            }
            if (j>=NFREQ+NEXOBS) continue;
            if (El[i]<=0.0) continue;

            x=r*sin(Az[i])*(1.0-2.0*El[i]/PI);
            y=r*cos(Az[i])*(1.0-2.0*El[i]/PI);
            xp=p[sat-1][0];
            yp=p[sat-1][1];
            col=MpColor(!Mp[j]?0.0:Mp[j][i]);

            if ((x-xp)*(x-xp)+(y-yp)*(y-yp)>=xs*xs) {
                int siz=PlotStyle<2?MarkSize:1;
                GraphS->DrawMark(x,y,0,col,siz,0);
                GraphS->DrawMark(x,y,0,PlotStyle<2?col:CColor[3],siz,0);
                p[sat-1][0]=x;
                p[sat-1][1]=y;
            }
        }
    }
    if (BtnShowTrack->Down&&0<=ind&&ind<NObs) {

        for (i=IndexObs[ind];i<Obs.n&&i<IndexObs[ind+1];i++) {
            obs=&Obs.data[i];
            if (SatMask[obs->sat-1]||!SatSel[obs->sat-1]||El[i]<=0.0) continue;
            for (j=0;j<NFREQ+NEXOBS;j++) {
                if (strstr(code2obs(obs->code[j],NULL),code)) break;
            }
            if (j>=NFREQ+NEXOBS) continue;
            col=MpColor(!Mp[j]?0.0:Mp[j][i]);

            x=r*sin(Az[i])*(1.0-2.0*El[i]/PI);
            y=r*cos(Az[i])*(1.0-2.0*El[i]/PI);

            satno2id(obs->sat,id);
            GraphS->DrawMark(x,y,0,col,Disp->Font->Size*2+5,0);
            GraphS->DrawMark(x,y,1,CColor[2],Disp->Font->Size*2+5,0);
            GraphS->DrawText(x,y,s=id,CColor[0],0,0,0);
        }
    }
}
// draw dop and number-of-satellite plot ------------------------------------
void __fastcall TrtkplotMainWindow::DrawDOP(int level)
{
	AnsiString label;
	TPoint p1,p2;
    gtime_t time;
    double xp,xc,yc,xl[2],yl[2],azel[MAXSAT*2],*dop,*x,*y;
    int i,j,*ns,prn,n=0;
	int ind=ObsIndex,doptype=DopType->ItemIndex;

    trace(3,"DrawDop: level=%d\n",level);

    GraphR->XLPos=TimeLabel?6:1;
    GraphR->YLPos=1;
    GraphR->Week=Week;
    GraphR->GetLim(xl,yl);
	//yl[0]=0.0; yl[1]=MaxDop;
	yl[0]=0.0; yl[1]=10.0; //MaxDop PDO<7==bad
    GraphR->SetLim(xl,yl);
    GraphR->SetTick(0.0,0.0);

	bool btnfixhorizdown = (BtnShowTrack->Down)?0:1;
	if (0<=ind&&ind<NObs&&BtnShowTrack->Down&& btnfixhorizdown) {
		double xl[2],yl[2],off;
        GraphR->GetLim(xl,yl);
        off=Xcent*(xl[1]-xl[0])/2.0;
        xp=TimePos(Obs.data[IndexObs[ind]].time);
        GraphR->GetCent(xc,yc);
        GraphR->SetCent(xp-off,yc);
	}

	GraphR->DrawAxis(1,1);
    GraphR->GetPos(p1,p2);
    p1.x=Disp->Font->Size;
	p1.y=(p1.y+p2.y)/2;

	if(doptype<=0){
    	label.sprintf("DOP值 (高度角>=%.0f%s)",ElMask,CHARDEG);
	}
	else{
		AnsiString str = DopType->Items->Strings[doptype];
		label.sprintf("%s值 (高度角>=%.0f%s)",str,ElMask,CHARDEG);
	}
	GraphR->DrawText(p1,label,CColor[2],0,0,90);

	if(NObs<=0)	return;

    x  =new double[NObs];
    y  =new double[NObs];
    dop=new double[NObs*4];
    ns =new int   [NObs];

    for (i=0;i<NObs;i++) {
        ns[n]=0;
        for (j=IndexObs[i];j<Obs.n&&j<IndexObs[i+1];j++) {
            if (SatMask[Obs.data[j].sat-1]||!SatSel[Obs.data[j].sat-1]) continue;
            if (El[j]<ElMask*D2R) continue;
            if (ElMaskP&&El[j]<ElMaskData[(int)(Az[j]*R2D+0.5)]) continue;
            azel[  ns[n]*2]=Az[j];
            azel[1+ns[n]*2]=El[j];
            ns[n]++;
        }
        dops(ns[n],azel,ElMask*D2R,dop+n*4);
        x[n++]=TimePos(Obs.data[IndexObs[i]].time);
    }
	for (i=0;i<4;i++) {
		if(doptype!=0 && doptype!=i+1)
			continue;

		for (j=0;j<n;j++) y[j]=dop[i+j*4];

		if (!(PlotStyle%2)) {
			DrawPolyS(GraphR,x,y,n,CColor[3],0);
		}
		if (level&&PlotStyle<2) {
			for (j=0;j<n;j++) {
				if (y[j]==0.0) continue;
				GraphR->DrawMark(x[j],y[j],0,MColor[0][i+2],MarkSize,0);
			}
		}
	}
    if (BtnShowTrack->Down&&0<=ind&&ind<NObs) {
        GraphR->GetLim(xl,yl);
        xl[0]=xl[1]=TimePos(Obs.data[IndexObs[ind]].time);

        GraphR->DrawPoly(xl,yl,2,CColor[2],0);

        ns[0]=0;
        for (i=IndexObs[ind];i<Obs.n&&i<IndexObs[ind+1];i++) {
            if (SatMask[Obs.data[i].sat-1]||!SatSel[Obs.data[i].sat-1]) continue;
            if (El[i]<ElMask*D2R) continue;
            if (ElMaskP&&El[i]<ElMaskData[(int)(Az[i]*R2D+0.5)]) continue;
            azel[  ns[0]*2]=Az[i];
            azel[1+ns[0]*2]=El[i];
            ns[0]++;
        }
        dops(ns[0],azel,ElMask*D2R,dop);

        for (i=0;i<4;i++) {
            if ((doptype!=0&&doptype!=i+2)||dop[i]<=0.0) continue;
            GraphR->DrawMark(xl[0],dop[i],0,MColor[0][i+2],MarkSize*2+2,0);
        }
        if (doptype==0||doptype==1) {
            GraphR->DrawMark(xl[0],ns[0],0,MColor[0][1],MarkSize*2+2,0);
        }
		GraphR->DrawMark(xl[0],yl[1]-1E-6,0,CColor[2],5,0);
		if(!btnfixhorizdown) {
            GraphR->DrawMark(xl[0],yl[1]-1E-6,1,CColor[2],9,0);
        }
    }
    else {
        DrawDopStat(dop,ns,n);
    }
	if (Nav.n<=0&&Nav.ng<=0&&(doptype==0||doptype>=2)&&!SimObs) {
        GraphR->GetPos(p1,p2);
        p2.x-=10;
        p2.y-=3;
        DrawLabel(GraphR,p2,"No Navigation Data",2,1);
	}
    delete [] x;
    delete [] y;
    delete [] dop;
    delete [] ns;
}
// draw cnr-elevation-plot ----------------------------------------------
void __fastcall TrtkplotMainWindow::DrawCNREle(int level)
{
    AnsiString s,ObsTypeText=ObsType2->Text;
    AnsiString label[]={"载噪比 (dBHz)","多路径误差 (m)"};
    TPoint p1,p2;
    TColor *col[2],colp[2][MAXSAT];
    gtime_t time={0};
    double *x[2],*y[2],xl[2]={-0.001,90.0},yl[2][2]={{10.0,65.0},{-10.0,10.0}};
    double xp[2][MAXSAT],yp[2][MAXSAT],ave=0.0,rms=0.0;
    char *code=ObsTypeText.c_str()+1;
    int i,j,k,n[2],np[2]={0},sat,ind=ObsIndex,hh=(int)(Disp->Font->Size*1.5);
    int nrms=0;

    trace(3,"DrawSnrE: level=%d\n",level);

    yl[1][0]=-MaxMP; yl[1][1]=MaxMP;

	p1.x=0;p1.y=0; p2.x =Disp->Width; p2.y= Disp->Height;
	int h,tmargin,bmargin,rmargin,lmargin;
    tmargin=5;                             // top margin
    bmargin=(int)(Disp->Font->Size*1.5)+3; // bottom
    rmargin=8;                             // right
	lmargin=Disp->Font->Size*3+15;         // left
	p1.x+=lmargin; p1.y+=tmargin;
	p2.x-=rmargin; p2.y=p2.y-bmargin;
	p1.y=tmargin; p2.y=p1.y;
	h=(Disp->Height-tmargin-bmargin);
	p2.y+=h; p1.y+=h;
	p1.y=tmargin; p2.y=p1.y;
	h = Disp->Height-tmargin-bmargin;
	p2.y+=h;

	j=0;
	for (i=0;i<2;i++) {
		if(i==1) continue;
		GraphE[i]->SetPos(p1,p2);
        GraphE[i]->XLPos=i==j?1:0;
        GraphE[i]->YLPos=1;
        GraphE[i]->SetLim(xl,yl[i]);
        GraphE[i]->SetTick(0.0,0.0);
        GraphE[i]->DrawAxis(1,1);

        GraphE[i]->GetPos(p1,p2);
        p1.x=Disp->Font->Size;
        p1.y=(p1.y+p2.y)/2;
        GraphE[i]->DrawText(p1,label[i],CColor[2],0,0,90);
        if (i==j) {
            p2.x-=8; p2.y-=6;
			//GraphE[i]->DrawText(p2,"Elevation (" CHARDEG ")",CColor[2],2,1,0);
			GraphE[i]->DrawText(p2,"高度角 (" CHARDEG ")",CColor[2],2,1,0);
        }
    }
    if (0<=ind&&ind<NObs&&BtnShowTrack->Down) {
        time=Obs.data[IndexObs[ind]].time;
    }

	if(NObs>0) {
        for (i=0;i<2;i++) {
            x[i]=new double[NObs],
            y[i]=new double[NObs];
            col[i]=new TColor[NObs];
        }
        for (sat=1;sat<=MAXSAT;sat++) {
            if (SatMask[sat-1]||!SatSel[sat-1]) continue;

            for (j=n[0]=n[1]=0;j<Obs.n;j++) {
                if (Obs.data[j].sat!=sat) continue;

                for (k=0;k<NFREQ+NEXOBS;k++) {
                    if (strstr(code2obs(Obs.data[j].code[k],NULL),code)) break;
                }
                if (k>=NFREQ+NEXOBS) continue;
                if (El[j]<=0.0) continue;

                x[0][n[0]]=x[1][n[1]]=El[j]*R2D;

                y[0][n[0]]=Obs.data[j].SNR[k]*0.25;
                y[1][n[1]]=!Mp[k]?0.0:Mp[k][j];

                col[0][n[0]]=col[1][n[1]]=
                    El[j]>0.0&&El[j]<ElMask*D2R?MColor[0][0]:MColor[0][4];

                if (y[0][n[0]]>0.0) {
                    if (timediff(time,Obs.data[j].time)==0.0) {
                        xp[0][np[0]]=x[0][n[0]];
                        yp[0][np[0]]=y[0][n[0]];
                        colp[0][np[0]]=ObsColor(Obs.data+j,Az[j],El[j]);
                        if (np[0]<MAXSAT&&colp[0][np[0]]!=clBlack) np[0]++;
                    }
                    if (n[0]<NObs) n[0]++;
                }
                if (y[1][n[1]]!=0.0) {
                    if (El[j]>=ElMask*D2R) {
                        ave+=y[1][n[1]];
                        rms+=SQR(y[1][n[1]]);
                        nrms++;
                    }
                    if (timediff(time,Obs.data[j].time)==0.0) {
                        xp[1][np[1]]=x[1][n[1]];
                        yp[1][np[1]]=y[1][n[1]];
                        colp[1][np[1]]=ObsColor(Obs.data+j,Az[j],El[j]);
                        if (np[1]<MAXSAT&&colp[1][np[1]]!=clBlack) np[1]++;
                    }
                    if (n[1]<NObs) n[1]++;
                }
            }
            if (!level||!(PlotStyle%2)) {
				for (i=0;i<2;i++) {
					if(i==1) continue;
                    DrawPolyS(GraphE[i],x[i],y[i],n[i],CColor[3],0);
                }
            }
            if (level&&PlotStyle<2) {
				for (i=0;i<2;i++) {
					if(i==1) continue;
                    for (j=0;j<n[i];j++) {
                        GraphE[i]->DrawMark(x[i][j],y[i][j],0,col[i][j],MarkSize,0);
                    }
                }
            }
        }
        for (i=0;i<2;i++) {
            delete [] x[i];
            delete [] y[i];
            delete [] col[i];
        }

		if (BtnShowTrack->Down&&0<=ind&&ind<NObs && NObs>0 ) {

			for (i=0;i<2;i++) {
				if(i==1) continue;
                for (j=0;j<np[i];j++) {
                    GraphE[i]->DrawMark(xp[i][j],yp[i][j],0,CColor[0],MarkSize*2+8,0);
                    GraphE[i]->DrawMark(xp[i][j],yp[i][j],1,CColor[2],MarkSize*2+6,0);
                    GraphE[i]->DrawMark(xp[i][j],yp[i][j],0,colp[i][j],MarkSize*2+2,0);
                }
            }
        }
    }
    if (ShowStats) {
		for (i=0;i<2;i++)
			if(i==1) break;
		
        if (i<2) {
            GraphE[i]->GetPos(p1,p2);
            p1.x+=8; p1.y+=6;
            s.sprintf("MARKER: %s %s",Sta.name,Sta.marker);
            DrawLabel(GraphE[i],p1,s,1,2); p1.y+=hh;
            s.sprintf("REC: %s %s %s",Sta.rectype,Sta.recver,Sta.recsno);
            DrawLabel(GraphE[i],p1,s,1,2); p1.y+=hh;
            s.sprintf("ANT: %s %s",Sta.antdes,Sta.antsno);
            DrawLabel(GraphE[i],p1,s,1,2); p1.y+=hh;
		}

		if(false) {
            ave=ave/nrms;
            rms=sqrt(rms/nrms);
            GraphE[1]->GetPos(p1,p2);
            p1.x=p2.x-8; p1.y+=6;
            DrawLabel(GraphE[1],p1,s.sprintf("AVE=%.4fm RMS=%.4fm",ave,rms),2,2);
        }
    }
}
// draw MP-elevation-plot ----------------------------------------------
void __fastcall TrtkplotMainWindow::DrawMPEle(int level)
{
    //TSpeedButton *btn[]={BtnOn1,BtnOn2,BtnOn3};
    AnsiString s,ObsTypeText=ObsType2->Text;
	//AnsiString label[]={"SNR (dBHz)","Multipath (m)"};
    AnsiString label[]={"载噪比 (dBHz)","多路径误差 (m)"};
	TPoint p1,p2;
    TColor *col[2],colp[2][MAXSAT];
    gtime_t time={0};
    double *x[2],*y[2],xl[2]={-0.001,90.0},yl[2][2]={{10.0,65.0},{-10.0,10.0}};
    double xp[2][MAXSAT],yp[2][MAXSAT],ave=0.0,rms=0.0;
    char *code=ObsTypeText.c_str()+1;
    int i,j,k,n[2],np[2]={0},sat,ind=ObsIndex,hh=(int)(Disp->Font->Size*1.5);
    int nrms=0;

    trace(3,"DrawSnrE: level=%d\n",level);

    yl[1][0]=-MaxMP; yl[1][1]=MaxMP;

    p1.x=0;p1.y=0; p2.x =Disp->Width; p2.y= Disp->Height;
	int h,tmargin,bmargin,rmargin,lmargin;
    tmargin=5;                             // top margin
    bmargin=(int)(Disp->Font->Size*1.5)+3; // bottom
    rmargin=8;                             // right
	lmargin=Disp->Font->Size*3+15;         // left
	p1.x+=lmargin; p1.y+=tmargin;
	p2.x-=rmargin; p2.y=p2.y-bmargin;
	p1.y=tmargin; p2.y=p1.y;
	h=(Disp->Height-tmargin-bmargin);
	p2.y+=h; p1.y+=h;
	p1.y=tmargin; p2.y=p1.y;
	h = Disp->Height-tmargin-bmargin;
	p2.y+=h;

	//for (i=0;i<2;i++) if (btn[i]->Down) j=i;
	j=1;
    for (i=0;i<2;i++) {
		//if (!btn[i]->Down) continue;
		if(i==0) continue;
		GraphE[i]->SetPos(p1,p2);
        GraphE[i]->XLPos=i==j?1:0;
        GraphE[i]->YLPos=1;
        GraphE[i]->SetLim(xl,yl[i]);
        GraphE[i]->SetTick(0.0,0.0);
        GraphE[i]->DrawAxis(1,1);

        GraphE[i]->GetPos(p1,p2);
        p1.x=Disp->Font->Size;
        p1.y=(p1.y+p2.y)/2;
        GraphE[i]->DrawText(p1,label[i],CColor[2],0,0,90);
        if (i==j) {
			p2.x-=8; p2.y-=6;
			//GraphE[i]->DrawText(p2,"Elevation (" CHARDEG ")",CColor[2],2,1,0);
			GraphE[i]->DrawText(p2,"高度角 (" CHARDEG ")",CColor[2],2,1,0);
		}
	}
    if (0<=ind&&ind<NObs&&BtnShowTrack->Down) {
        time=Obs.data[IndexObs[ind]].time;
    }
	//if (NObs>0&&BtnSol1->Down) {
	if(NObs>0) {
        for (i=0;i<2;i++) {
            x[i]=new double[NObs],
            y[i]=new double[NObs];
            col[i]=new TColor[NObs];
        }
        for (sat=1;sat<=MAXSAT;sat++) {
            if (SatMask[sat-1]||!SatSel[sat-1]) continue;

            for (j=n[0]=n[1]=0;j<Obs.n;j++) {
                if (Obs.data[j].sat!=sat) continue;

                for (k=0;k<NFREQ+NEXOBS;k++) {
                    if (strstr(code2obs(Obs.data[j].code[k],NULL),code)) break;
                }
                if (k>=NFREQ+NEXOBS) continue;
                if (El[j]<=0.0) continue;

                x[0][n[0]]=x[1][n[1]]=El[j]*R2D;

                y[0][n[0]]=Obs.data[j].SNR[k]*0.25;
                y[1][n[1]]=!Mp[k]?0.0:Mp[k][j];

                col[0][n[0]]=col[1][n[1]]=
                    El[j]>0.0&&El[j]<ElMask*D2R?MColor[0][0]:MColor[0][4];

                if (y[0][n[0]]>0.0) {
                    if (timediff(time,Obs.data[j].time)==0.0) {
                        xp[0][np[0]]=x[0][n[0]];
                        yp[0][np[0]]=y[0][n[0]];
                        colp[0][np[0]]=ObsColor(Obs.data+j,Az[j],El[j]);
                        if (np[0]<MAXSAT&&colp[0][np[0]]!=clBlack) np[0]++;
                    }
                    if (n[0]<NObs) n[0]++;
                }
                if (y[1][n[1]]!=0.0) {
                    if (El[j]>=ElMask*D2R) {
                        ave+=y[1][n[1]];
                        rms+=SQR(y[1][n[1]]);
                        nrms++;
                    }
                    if (timediff(time,Obs.data[j].time)==0.0) {
                        xp[1][np[1]]=x[1][n[1]];
                        yp[1][np[1]]=y[1][n[1]];
                        colp[1][np[1]]=ObsColor(Obs.data+j,Az[j],El[j]);
                        if (np[1]<MAXSAT&&colp[1][np[1]]!=clBlack) np[1]++;
                    }
                    if (n[1]<NObs) n[1]++;
                }
            }
            if (!level||!(PlotStyle%2)) {
                for (i=0;i<2;i++) {
					//if (!btn[i]->Down) continue;
					if(i==0) continue;
					DrawPolyS(GraphE[i],x[i],y[i],n[i],CColor[3],0);
                }
            }
            if (level&&PlotStyle<2) {
                for (i=0;i<2;i++) {
					//if (!btn[i]->Down) continue;
					if(i==0) continue;
                    for (j=0;j<n[i];j++) {
                        GraphE[i]->DrawMark(x[i][j],y[i][j],0,col[i][j],MarkSize,0);
                    }
                }
            }
        }
        for (i=0;i<2;i++) {
            delete [] x[i];
            delete [] y[i];
            delete [] col[i];
        }
		//if (BtnShowTrack->Down&&0<=ind&&ind<NObs&&BtnSol1->Down) {
		if (BtnShowTrack->Down&&0<=ind&&ind<NObs && NObs>0 ) {

            for (i=0;i<2;i++) {
				//if (!btn[i]->Down) continue;
				if(i==0) continue;
                for (j=0;j<np[i];j++) {
                    GraphE[i]->DrawMark(xp[i][j],yp[i][j],0,CColor[0],MarkSize*2+8,0);
                    GraphE[i]->DrawMark(xp[i][j],yp[i][j],1,CColor[2],MarkSize*2+6,0);
                    GraphE[i]->DrawMark(xp[i][j],yp[i][j],0,colp[i][j],MarkSize*2+2,0);
                }
            }
        }
    }
    if (ShowStats) {
		for (i=0;i<2;i++) 
			//if (btn[i]->Down) break;
			if(i==0) break;
		
        if (i<2) {
            GraphE[i]->GetPos(p1,p2);
            p1.x+=8; p1.y+=6;
            s.sprintf("MARKER: %s %s",Sta.name,Sta.marker);
            DrawLabel(GraphE[i],p1,s,1,2); p1.y+=hh;
            s.sprintf("REC: %s %s %s",Sta.rectype,Sta.recver,Sta.recsno);
            DrawLabel(GraphE[i],p1,s,1,2); p1.y+=hh;
            s.sprintf("ANT: %s %s",Sta.antdes,Sta.antsno);
            DrawLabel(GraphE[i],p1,s,1,2); p1.y+=hh;
        }
		//if (btn[1]->Down&&nrms>0&&!BtnShowTrack->Down) {
		if(i==1 && nrms>0 && !BtnShowTrack->Down) {
            ave=ave/nrms;
            rms=sqrt(rms/nrms);
            GraphE[1]->GetPos(p1,p2);
            p1.x=p2.x-8; p1.y+=6;
            DrawLabel(GraphE[1],p1,s.sprintf("AVE=%.4fm RMS=%.4fm",ave,rms),2,2);
        }
    }
}
// draw plot ----------------------------------------------------------------
void __fastcall TrtkplotMainWindow::UpdateDisp(void)
{
	TCanvas *c=Disp->Canvas;
    TRect r=Disp->ClientRect;
    int level=Drag?0:1;

	trace(3,"UpdateDisp\n");
	if (Flush)
	{
		c->Pen  ->Color=CColor[0];
        c->Brush->Color=CColor[0];
        c->Pen  ->Style=psSolid;
        c->Brush->Style=bsSolid;
		c->FillRect(r);

		switch(PlotType){
			case	PLOT_VisSat: {
				if(NObs>0) {
					DrawVisSat(level);
				}
				else {
					PlotType=-1; UpdateDisp();
				}
				break;
			}
			case	PLOT_CNR: {
				if(NObs>0) {
					 DrawCNR(level);
				}
				else {
					PlotType=-1; UpdateDisp();
				}
				break;
			}
			case	PLOT_MP: {
				if(NObs>0) {
					DrawMP(level);
                }
				else{
					PlotType=-1; UpdateDisp();
				}
				break;
			}
			case	PLOT_Ele: {
				if(NObs>0)
					DrawEle(level);
				else{
                    PlotType=-1; UpdateDisp();
				}
				break;
			}
			case	PLOT_SkyCNR: {
				if(NObs>0)
					DrawSkyCNR(level);
				else {
                    PlotType=-1; UpdateDisp();
				}
				break;
			}
			case	PLOT_SkyMP: {
				if(NObs>0)
					DrawSkyMP(level);
				else {
                    PlotType = -1; UpdateDisp();
				}
				break;
			}
			case	PLOT_DOP: {
				if(NObs>0)
					DrawDOP(level);
				else {
                    PlotType = -1; UpdateDisp();
				}
				break;
			}
			case	PLOT_CNREle: {
				if(NObs>0)
					DrawCNREle(level);
				else {
                    PlotType = -1; UpdateDisp();
				}
				break;
			}
			case	PLOT_MPEle: {
				if(NObs>0)
					DrawMPEle(level);
				else {
                    PlotType = -1; UpdateDisp();
				}
				break;
			}
			default: {
				DrawTrk(level);
				//PlotType = -1;ShowMsg("");ShowLegend(NULL);ObsType->Items->Clear();
				//SatList->Items->Clear();Clear();
            }
		}
		Buff->SetSize(Disp->ClientWidth,Disp->ClientHeight);
        Buff->Canvas->CopyRect(r,c,r);
	}
	else
	{
    	c->CopyRect(r,Buff->Canvas,r);
	}
	Flush=0;
}

// update plot --------------------------------------------------------------
void __fastcall TrtkplotMainWindow::UpdatePlot(void)
{
	trace(3,"UpdatePlot\n");

	UpdateEnable();
	UpdateInfo();
	Refresh();

	//Refresh_GEView();
	//Refresh_GMView();
}
// refresh plot -------------------------------------------------------------
void __fastcall TrtkplotMainWindow::Refresh(void)
{
    trace(3,"Refresh\n");

    Flush=1;
    Disp->Invalidate();
}

// draw track-plot ----------------------------------------------------------
void __fastcall TrtkplotMainWindow::DrawTrk(int level)
{
    AnsiString label,header,s;
    TIMEPOS *pos,*pos1,*pos2,*vel;
    gtime_t time1={0},time2={0};
    sol_t *sol;
    TPoint p1,p2;
    TColor color;
    double xt,yt,sx,sy,opos[3];
    int i,j,index,sel=!BtnSol1->Down&&BtnSol2->Down?1:0,p=0;

    trace(3,"DrawTrk: level=%d\n",level);

#if 1
    if (ShowGLabel>=3) {
        GraphT->XLPos=7; GraphT->YLPos=7;
        GraphT->DrawCircles(ShowGLabel==4);
    }
    else if (ShowGLabel>=1) {
        GraphT->XLPos=2; GraphT->YLPos=4;
        GraphT->DrawAxis(ShowLabel,ShowGLabel==2);
    }
#endif

	/*if (ShowScale) {
		GraphT->GetPos(p1,p2);
		GraphT->GetTick(xt,yt);
		GraphT->GetScale(sx,sy);
		p2.x-=70;
		p2.y-=25;
		DrawMark(GraphT,p2,11,CColor[2],(int)(xt/sx+0.5),0);
		p2.y-=3;
		if      (xt<0.01  ) label.sprintf("%.0f mm",xt*1000.0);
		else if (xt<1.0   ) label.sprintf("%.0f cm",xt*100.0);
		else if (xt<1000.0) label.sprintf("%.0f m" ,xt);
		else                label.sprintf("%.0f km",xt/1000.0);
		DrawLabel(GraphT,p2,label,0,1);
	}*/

}
// draw mark with hemming ---------------------------------------------------
void __fastcall TrtkplotMainWindow::DrawMark(TGraph *g, TPoint p, int mark, TColor color,
    int size, int rot)
{
    g->DrawMark(p,mark,color,CColor[0],size,rot);
}
// draw label with hemming --------------------------------------------------
void __fastcall TrtkplotMainWindow::DrawLabel(TGraph *g, TPoint p, AnsiString label, int ha,
    int va)
{
    g->DrawText(p,label,CColor[2],CColor[0],ha,va,0);
}