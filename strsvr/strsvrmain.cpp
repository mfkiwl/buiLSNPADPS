//---------------------------------------------------------------------------

#include <vcl.h>
#include <inifiles.hpp>
#include <mmsystem.h>
#include <stdio.h>
#pragma hdrstop

#include "strsvrmain.h"
#include "confdlg.h"
#include "serioptdlg.h"
#include "fileoptdlg.h"
#include "tcpoptdlg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TstrsvrMainWindow *strsvrMainWindow;

#define TRACEFILE   "strsvrmain.trace"  // debug trace file
#define CLORANGE    (TColor)0x00AAFF
#define MIN(x,y)    ((x)<(y)?(x):(y))

static strsvr_t strsvr;

//---------------------------------------------------------------------------
__fastcall TstrsvrMainWindow::TstrsvrMainWindow(TComponent* Owner)
	: TForm(Owner)
{
	char file[1024]="strsvr.exe",*p;

    ::GetModuleFileName(NULL,file,sizeof(file));
	if (!(p=strrchr(file,'.')))
		p=file+strlen(file);

    strcpy(p,".ini");
    IniFile=file;

    DoubleBuffered=true;
}
//---------------------------------------------------------------------------
void __fastcall TstrsvrMainWindow::FormResize(TObject *Sender)
{
	this->Panel21->Width = this->Panel2->Width/2;
	this->Panel22->Width = this->Panel2->Width/2;
	this->Panel21->Height = this->Panel2->Height;
	this->Panel22->Height = this->Panel2->Height;

	this->Panel211->Width = this->Panel21->Width;
	this->Panel212->Width = this->Panel21->Width;
	this->Panel211->Height = 1*this->Panel21->Height/3;
	this->Panel212->Height = 2*this->Panel21->Height/3;

	this->GroupBoxDateConv->Width = 3*this->Panel211->Width/5;
	this->GroupBoxDownloadTips->Width = 2*this->Panel211->Width/5;
	this->GroupBoxDateConv->Height = this->Panel211->Height;
	this->GroupBoxDownloadTips->Height = this->Panel211->Height;
	this->GroupBoxDateConv->Left = 0;
	this->GroupBoxDateConv->Top = 0;
	this->GroupBoxDownloadTips->Left = this->GroupBoxDateConv->Width;
	this->GroupBoxDownloadTips->Top = 0;

	double AllH = 9.5*this->GroupBoxDateConv->Height/10;
	double ConstH = 0.5*this->GroupBoxDateConv->Height/10;
	double tempH = AllH/4;
	this->LabelYearMonthDay->Top = tempH/2 - this->LabelYearMonthDay->Height/2+ConstH;
	this->LabelGPSWeek->Top = tempH + tempH/2 - this->LabelGPSWeek->Height/2+ConstH;
	this->LabelDayOfYear->Top = 2*tempH + tempH/2 - this->LabelDayOfYear->Height/2 + ConstH;
	this->LabelMJD->Top = 3*tempH + tempH/2 - this->LabelMJD->Height/2 + ConstH;
	this->EditYearMonthDay->Top = tempH/2 - this->EditYearMonthDay->Height/2 + ConstH;
	this->EditGPSWeek->Top = tempH + tempH/2 - this->EditGPSWeek->Height/2 + ConstH;
	this->EditDayOfYear->Top = 2*tempH + tempH/2 - this->EditDayOfYear->Height/2 + ConstH;
	this->EditMJD->Top = 3*tempH + tempH/2 - this->EditMJD->Height/2 + ConstH;
	this->LabelYearMonthDay->Left = 8;
	this->LabelGPSWeek->Left = 8;
	this->LabelDayOfYear->Left = 8;
	this->LabelMJD->Left = 8;
	this->EditYearMonthDay->Left = this->LabelYearMonthDay->Left+this->LabelYearMonthDay->Width;
	this->EditGPSWeek->Left = this->LabelGPSWeek->Left + this->LabelGPSWeek->Width;
	this->EditDayOfYear->Left = this->LabelDayOfYear->Left + this->LabelDayOfYear->Width;
	this->EditMJD->Left = this->LabelMJD->Left + this->LabelMJD->Width;
	this->EditYearMonthDay->Width = this->GroupBoxDateConv->Width - this->LabelYearMonthDay->Width - this->LabelYearMonthDay->Left-8;
	this->EditGPSWeek->Width = this->GroupBoxDateConv->Width - this->LabelGPSWeek->Width - this->LabelGPSWeek->Left -8;
	this->EditDayOfYear->Width = this->GroupBoxDateConv->Width - this->LabelDayOfYear->Width - this->LabelDayOfYear->Left -8;
	this->EditMJD->Width = this->GroupBoxDateConv->Width - this->LabelMJD->Width - this->LabelMJD->Left -8;

	this->MemoDataDownload->Width = this->GroupBoxDownloadTips->Width -10;
	this->MemoDataDownload->Height = this->GroupBoxDownloadTips->Height -25;
	this->MemoDataDownload->Left = 5;
	this->MemoDataDownload->Top = 20;

	this->GroupBoxStreamRe->Width = this->Panel212->Width-1;
	this->GroupBoxStreamRe->Height = this->Panel212->Height-1;
	this->GroupBoxStreamRe->Left = 0;
	this->GroupBoxStreamRe->Top = 0;

	double streamAllH = 9.35*this->GroupBoxStreamRe->Height/10;
	double streamConstH = 0.65*this->GroupBoxStreamRe->Height/10;
	this->PanelStr1->Top = streamConstH;
	this->PanelStr2->Top = streamConstH;
	this->PanelStr3->Top = streamConstH;
	this->PanelStr4->Top = streamConstH;
	this->PanelStr5->Top = streamConstH;
	double tempVal1 = (this->GroupBoxStreamRe->Width - 6 - this->PanelStr1->Width -
					this->PanelStr2->Width - this->PanelStr3->Width - this->PanelStr4->Width -
					this->PanelStr5->Width)/4;
	this->PanelStr1->Left = 3;
	this->PanelStr2->Left = this->PanelStr1->Left + this->PanelStr1->Width + tempVal1;
	this->PanelStr3->Left = this->PanelStr2->Left + this->PanelStr2->Width + tempVal1;
	this->PanelStr4->Left = this->PanelStr3->Left + this->PanelStr3->Width + tempVal1;
	this->PanelStr5->Left = this->PanelStr4->Left + this->PanelStr4->Width + tempVal1;

	this->Progress->Width = 0.9*this->GroupBoxStreamRe->Width;
	this->Progress->Left =  0.05*this->GroupBoxStreamRe->Width;
	this->Progress->Top = this->PanelStr1->Top + this->PanelStr1->Height + 5;
	this->Message->Width = 0.93*this->GroupBoxStreamRe->Width;
	this->Message->Left = 0.035*this->GroupBoxStreamRe->Width;
	this->Message->Top = this->Progress->Top + this->Progress->Height + 5;
	double tempVal2 = (this->GroupBoxStreamRe->Width-this->BtnStart->Width-
						this->BtnStop->Width)/3;
	this->BtnStart->Left = tempVal2;
	this->BtnStop->Left = 2*tempVal2 + this->BtnStart->Width;
	//this->BtnOpt->Left = this->BtnStop->Left + this->BtnStop->Width + tempVal2;
	//this->BtnExit->Left = this->BtnOpt->Left + this->BtnOpt->Width + tempVal2;
	this->BtnStart->Top = this->Message->Top + this->Message->Height + 10;
	this->BtnStop->Top = this->Message->Top + this->Message->Height + 10;
	//this->BtnOpt->Top = this->Message->Top + this->Message->Height + 10;
	//this->BtnExit->Top = streamConstH + 7*streamAllH/8 - this->BtnExit->Height/2;
	this->CheckBoxHEX->Top = this->BtnStart->Top + this->BtnStart->Height + 5;
	this->CheckBoxHEX->Left = this->GroupBoxStreamRe->Width - this->CheckBoxHEX->Width;
	this->MemoStreamStr->Height = this->GroupBoxStreamRe->Height - this->CheckBoxHEX->Top -
								this->CheckBoxHEX->Height - 5;

	this->GroupBoxDataDownload->Width = this->Panel22->Width-1;
	this->GroupBoxDataDownload->Height = this->Panel22->Height-1;
	this->GroupBoxDataDownload->Left =0;
	this->GroupBoxDataDownload->Top = 0;

	double StrGridCol = this->StringGrid1->Width/5;
	int i=0;
	for(i=0;i<5;i++){
		this->StringGrid1->ColWidths[i] = StrGridCol;
	}

	this->edt_ServerAddress->Width = this->GroupBoxDataDownload->Width - this->edt_ServerAddress->Left - 8;
	this->btn_Connect->Left = this->GroupBoxDataDownload->Width - this->btn_Connect->Width - 8;
	this->btn_Download->Left =  this->GroupBoxDataDownload->Width - this->btn_Connect->Width - 8;
	double gtemp1 = (this->btn_Connect->Left - 8)/2.0;
	this->LabelPassWord->Left = gtemp1 + 8;
	this->edt_UserPassword->Left = this->LabelPassWord->Left + this->LabelPassWord->Width+10;
	this->edt_UserName->Width = this->LabelPassWord->Left - this->edt_UserName->Left - 10;
	this->edt_UserPassword->Width = this->btn_Connect->Left - this->edt_UserPassword->Left - 15;

	this->btn_Back->Left = this->btn_Download->Left - 15 - this->btn_Back->Width;
	this->btn_EnterDirectory->Left = this->btn_Back->Left - 10 - this->btn_EnterDirectory->Width;
	this->edt_CurrentDirectory->Width = this->btn_EnterDirectory->Left - this->edt_CurrentDirectory->Left - 8;
}
//---------------------------------------------------------------------------
void __fastcall TstrsvrMainWindow::EditYearMonthDayChange(TObject *Sender)
{
	AnsiString editStr = this->EditYearMonthDay->Text;
	editStr = editStr.Trim();

	if(editStr.Length()==0)
	{
		this->MemoDataDownload->Lines->Clear();
		this->EditGPSWeek->Text = "";
		this->EditDayOfYear->Text = "";
		this->EditMJD->Text = "";
		return;
	}

	bool tempb1 = iscontainchar(editStr.c_str(),editStr.Length());

	if(tempb1)
	{
		TStringList *lstLine = new TStringList;
		AnsiString str = StringReplace(editStr," ", "\r\n", TReplaceFlags() << rfReplaceAll);
		lstLine->Text = str;

		if(lstLine->Count == 3)
		{
			double year = StrToFloat(lstLine->Strings[0]);
			UnicodeString yearstr = lstLine->Strings[0];
			double month = StrToFloat(lstLine->Strings[1]);
			double day = StrToFloat(lstLine->Strings[2]);

			int a = 0,tempyear=(int)year;
			if((tempyear%4==0 && tempyear%100!=0)||(tempyear%400))
				a=1;
			else
				a=2;

			bool tempb2 = false;
			int tempmonth = (int)month;
			if(tempmonth>0 && tempmonth<13)
			{
				switch(tempmonth)
				{
					case 1: tempb2 = (year>=1980 && year<=2100 && day>0 && day<32); break;
					case 2:
						if(a==1)
							tempb2 = (year>=1980 && year<=2100 && day>0 && day<30);
						else if(a==2)
							tempb2 = (year>=1980 && year<=2100 && day>0 && day<29);
						break;
					case 3: tempb2 = (year>=1980 && year<=2100 && day>0 && day<32); break;
					case 4: tempb2 = (year>=1980 && year<=2100 && day>0 && day<31); break;
					case 5: tempb2 = (year>=1980 && year<=2100 && day>0 && day<32); break;
					case 6: tempb2 = (year>=1980 && year<=2100 && day>0 && day<31); break;
					case 7: tempb2 = (year>=1980 && year<=2100 && day>0 && day<32); break;
					case 8: tempb2 = (year>=1980 && year<=2100 && day>0 && day<32); break;
					case 9: tempb2 = (year>=1980 && year<=2100 && day>0 && day<31); break;
					case 10: tempb2 = (year>=1980 && year<=2100 && day>0 && day<32); break;
					case 11: tempb2 = (year>=1980 && year<=2100 && day>0 && day<31); break;
					case 12: tempb2 = (year>=1980 && year<=2100 && day>0 && day<32); break;
				}
			}

			//1980 1 1-2100 12 31
			if(tempb2)
			{
				double ep[6] = {year,month,day,0,0,0};
				gtime_t gt = epoch2time(ep);
				//GPS Week, day of week
				int week = 0;
				time2gpst(gt,&week);
				int wday = computedayweek(year,month,day);
				//day of year
				double doy = time2doy(gt);
				//mjd
				double ep0[6] = {2000,1,1,12,0,0};
				double mjd=51544.5+(timediff(gt,epoch2time(ep0)))/86400.0;

				this->EditGPSWeek->Text = IntToStr(week)+" "+IntToStr(wday);
				this->EditDayOfYear->Text = FloatToStr(year) + " " + IntToStr((int)floor(doy));
				this->EditMJD->Text = FloatToStr(mjd);
				this->MemoDataDownload->Text =  "sp3文件:"+ FormatFloat("0000",(double)week)+FormatFloat("0",(double)wday)+".sp3\r\no文件:aaaa" +
												FormatFloat("000",(int)floor(doy)) + "0."+ yearstr.SubString(yearstr.Length()-1,2) +"o";
				delete lstLine;
			}
			else
			{
				this->MemoDataDownload->Lines->Clear();
				this->EditGPSWeek->Text = "";
				this->EditDayOfYear->Text = "";
				this->EditMJD->Text = "";
				delete lstLine;
				return;
			}
		}
		else
		{
			this->MemoDataDownload->Lines->Clear();
			this->EditGPSWeek->Text = "";
			this->EditDayOfYear->Text = "";
			this->EditMJD->Text = "";
			delete lstLine;
			return;
		}
	}
	else
	{
		this->MemoDataDownload->Lines->Clear();
		this->EditGPSWeek->Text = "";
		this->EditDayOfYear->Text = "";
		this->EditMJD->Text = "";
		return;
	}
}
//---------------------------------------------------------------------------

void __fastcall TstrsvrMainWindow::EditGPSWeekChange(TObject *Sender)
{
	AnsiString editStr = this->EditGPSWeek->Text;
    editStr = editStr.Trim();
    if(editStr.Length()==0)
	{
		this->MemoDataDownload->Lines->Clear();
		this->EditYearMonthDay->Text = "";
		this->EditDayOfYear->Text = "";
		this->EditMJD->Text = "";
		return;
	}

	bool tempb1 = iscontainchar(editStr.c_str(),editStr.Length());

	if(tempb1)
	{
    	TStringList *lstLine = new TStringList;
		AnsiString str = StringReplace(editStr," ", "\r\n", TReplaceFlags() << rfReplaceAll);
		lstLine->Text = str;

        if(lstLine->Count == 2)
		{
			double gpsweek = StrToFloat(lstLine->Strings[0]);
			double dayweek = StrToFloat(lstLine->Strings[1]);
			if(gpsweek >= 0 && gpsweek<=6312  && dayweek >=0 && dayweek <=6)
			{
				double sec = gpsweek*86400*7 + dayweek*86400;
				double ep0[6] = {1980,1,6,0,0,0};
				gtime_t t0 = epoch2time(ep0);
				gtime_t gt;
				gt.time = t0.time + sec; gt.sec = t0.sec;
				double ep[6] = {0.0};
				time2epoch(gt,ep);
				this->EditYearMonthDay->Text =  FloatToStr(ep[0]) +" " +  FloatToStr(ep[1]) + " " + FloatToStr(ep[2]);

            	delete lstLine;
			}
			else
			{
            	this->MemoDataDownload->Lines->Clear();
				this->EditYearMonthDay->Text = "";
				this->EditDayOfYear->Text = "";
				this->EditMJD->Text = "";
				delete lstLine;
				return;
            }
		}
		else
		{
			this->MemoDataDownload->Lines->Clear();
			this->EditYearMonthDay->Text = "";
			this->EditDayOfYear->Text = "";
			this->EditMJD->Text = "";
			delete lstLine;
			return;
		}
	}
	else
	{
		this->MemoDataDownload->Lines->Clear();
		this->EditYearMonthDay->Text = "";
		this->EditDayOfYear->Text = "";
		this->EditMJD->Text = "";
		return;
	}
}
//---------------------------------------------------------------------------

void __fastcall TstrsvrMainWindow::EditDayOfYearChange(TObject *Sender)
{
	AnsiString editStr = this->EditDayOfYear->Text;
	editStr = editStr.Trim();
    if(editStr.Length()==0)
	{
		this->MemoDataDownload->Lines->Clear();
		this->EditYearMonthDay->Text = "";
		this->EditGPSWeek->Text = "";
		this->EditMJD->Text = "";
		return;
	}

	bool tempb1 = iscontainchar(editStr.c_str(),editStr.Length());

	if(tempb1)
	{
    	TStringList *lstLine = new TStringList;
		AnsiString str = StringReplace(editStr," ", "\r\n", TReplaceFlags() << rfReplaceAll);
		lstLine->Text = str;

		if(lstLine->Count == 2)
		{
        	double year = StrToFloat(lstLine->Strings[0]);
			double doy = StrToFloat(lstLine->Strings[1]);

			int tempyear = year;
			bool tempb2 = false;
			if((tempyear%4==0 && tempyear%100!=0)||(tempyear%400))
				tempb2 = (year>=1980 && year<=2100 && doy >0 && doy<367);
			else
				tempb2 = (year>=1980 && year<=2100 && doy >0 && doy<366);

			if(tempb2)
			{
				double dtime = (doy-1.0)*86400.0;
				double ep0[6] = {year,1.0,1.0,0.0,0.0,0.0};
				gtime_t gt0 = epoch2time(ep0);
				gtime_t gt;
				gt.time = gt0.time + gt0.sec + dtime;
				double ep[6] = {0.0};
				time2epoch(gt,ep);

				this->EditYearMonthDay->Text =  FloatToStr(ep[0]) +" " +  FloatToStr(ep[1]) + " " + FloatToStr(ep[2]);
				delete lstLine;
			}
			else
			{
             	this->MemoDataDownload->Lines->Clear();
				this->EditYearMonthDay->Text = "";
				this->EditGPSWeek->Text = "";
				this->EditMJD->Text = "";
				delete lstLine;
				return;
			}
		}
		else
		{
			this->MemoDataDownload->Lines->Clear();
			this->EditYearMonthDay->Text = "";
			this->EditGPSWeek->Text = "";
			this->EditMJD->Text = "";
			delete lstLine;
			return;
		}
	}
	else
	{
		this->MemoDataDownload->Lines->Clear();
		this->EditYearMonthDay->Text = "";
		this->EditGPSWeek->Text = "";
		this->EditMJD->Text = "";
		return;
	}
}
//---------------------------------------------------------------------------
void __fastcall TstrsvrMainWindow::FormShow(TObject *Sender)
{
	double StrGridCol = this->StringGrid1->Width/5;
	int i=0;
	for(i=0;i<5;i++){
		this->StringGrid1->ColWidths[i] = StrGridCol;
	}
    this->BtnStop->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TstrsvrMainWindow::FormCreate(TObject *Sender)
{
	this->DoubleBuffered = true;
	//this->idntfrz1->IdleTimeOut = 50;
	//this->idntfrz1->OnlyWhenIdle = false;

	this->StringGrid1->Cells[0][0] = "文件名";
	this->StringGrid1->Cells[1][0] = "文件大小";
	this->StringGrid1->Cells[2][0] = "文件类型";
	this->StringGrid1->Cells[3][0] = "最近修改";
	this->StringGrid1->Cells[4][0] = "权限";

	AnsiString s;
    int argc=0,autorun=0,tasktray=0;
    char *p,*argv[32],buff[1024];

    strsvrinit(&strsvr,3);

    //Caption=s.sprintf("%s ver.%s",PRGNAME,VER_RTKLIB);

    strcpy(buff,GetCommandLine());

    for (p=buff;*p&&argc<32;p++) {
        if (*p==' ') continue;
        if (*p=='"') {
            argv[argc++]=p+1;
            if (!(p=strchr(p+1,'"'))) break;
        }
        else {
            argv[argc++]=p;
            if (!(p=strchr(p+1,' '))) break;
        }
        *p='\0';
    }
    for (int i=1;i<argc;i++) {
        if (!strcmp(argv[i],"-i")&&i+1<argc) IniFile=argv[++i];
    }
    LoadOpt();

    for (int i=1;i<argc;i++) {
        if      (!strcmp(argv[i],"-t")&&i+1<argc) Caption=argv[++i];
        else if (!strcmp(argv[i],"-auto")) autorun=1;
        else if (!strcmp(argv[i],"-tray")) tasktray=1;
    }
    SetTrayIcon(0);

    /*if (tasktray) {
		Application->ShowMainForm=false;
		TrayIcon->Visible=true;
	}*/
    if (autorun) {
        SvrStart();
	}
}
// start stream server ------------------------------------------------------
void __fastcall TstrsvrMainWindow::SvrStart(void)
{
    strconv_t *conv[3]={0};
    static char str[4][1024];
    int itype[]={
		//STR_SERIAL,STR_TCPCLI,STR_TCPSVR,STR_NTRIPCLI,STR_FILE,STR_FTP,STR_HTTP
		STR_SERIAL,STR_NTRIPCLI
    };
    int otype[]={
		//STR_NONE,STR_SERIAL,STR_TCPCLI,STR_TCPSVR,STR_NTRIPSVR,STR_FILE
		STR_NONE,STR_FILE
    };
	int ip[]={0,1,1,1,2,3,3};
    int ip_o[] = {2};
	int strs[4]={0},opt[7]={0},n;
    char *paths[4],*cmd=NULL,filepath[1024],buff[1024];
    char *ant[3]={"","",""},*rcv[3]={"","",""},*p;
    FILE *fp;

    if (TraceLevel>0) {
        traceopen(TRACEFILE);
        tracelevel(TraceLevel);
    }
    for (int i=0;i<4;i++) paths[i]=str[i];

    strs[0]=itype[Input->ItemIndex];
    strs[1]=otype[Output1->ItemIndex];
	//strs[2]=otype[Output2->ItemIndex];
	//strs[3]=otype[Output3->ItemIndex];

    strcpy(paths[0],Paths[0][ip[Input->ItemIndex]].c_str());
    strcpy(paths[1],!Output1->ItemIndex?"":Paths[1][ip_o[Output1->ItemIndex-1]].c_str());
	//strcpy(paths[2],!Output2->ItemIndex?"":Paths[2][ip[Output2->ItemIndex-1]].c_str());
	//strcpy(paths[3],!Output3->ItemIndex?"":Paths[3][ip[Output3->ItemIndex-1]].c_str());

	if (Input->ItemIndex==0) {
		if (CmdEna[0]) cmd=strsvrMainWindow->Cmds[0].c_str();
	}
	else if (Input->ItemIndex==1||Input->ItemIndex==3) {
		if (CmdEnaTcp[0]) cmd=strsvrMainWindow->CmdsTcp[0].c_str();
	}
    for (int i=0;i<5;i++) {
        opt[i]=SvrOpt[i];
    }
    opt[5]=NmeaReq?SvrOpt[5]:0;
    opt[6]=FileSwapMargin;

    for (int i=1;i<4;i++) {
		if (strs[i]!=STR_FILE) continue;
        strcpy(filepath,paths[i]);
        if (strstr(filepath,"::A")) continue;
        if ((p=strstr(filepath,"::"))) *p='\0';
        if (!(fp=fopen(filepath,"r"))) continue;
        fclose(fp);
		ConfDialog->Label2->Caption=filepath;
        if (ConfDialog->ShowModal()!=mrOk) return;
    }
    strsetdir(LocalDirectory.c_str());
    strsetproxy(ProxyAddress.c_str());

    for (int i=0;i<3;i++) {
        if (!ConvEna[i]) continue;
        if (!(conv[i]=strconvnew(ConvInp[i],ConvOut[i],ConvMsg[i].c_str(),
                                 StaId,StaSel,ConvOpt[i].c_str()))) continue;
        strcpy(buff,AntType.c_str());
        for (p=strtok(buff,","),n=0;p&&n<3;p=strtok(NULL,",")) ant[n++]=p;
        strcpy(conv[i]->out.sta.antdes,ant[0]);
        strcpy(conv[i]->out.sta.antsno,ant[1]);
        conv[i]->out.sta.antsetup=atoi(ant[2]);
        strcpy(buff,RcvType.c_str());
        for (p=strtok(buff,","),n=0;p&&n<3;p=strtok(NULL,",")) rcv[n++]=p;
        strcpy(conv[i]->out.sta.rectype,rcv[0]);
        strcpy(conv[i]->out.sta.recver ,rcv[1]);
        strcpy(conv[i]->out.sta.recsno ,rcv[2]);
        matcpy(conv[i]->out.sta.pos,AntPos,3,1);
        matcpy(conv[i]->out.sta.del,AntOff,3,1);
    }
    // stream server start
    if (!strsvrstart(&strsvr,opt,strs,paths,conv,cmd,AntPos)) return;

    StartTime=utc2gpst(timeget());
	//Panel1    ->Enabled=false;
	PanelStr1->Enabled = false;  PanelStr2->Enabled = false;PanelStr3->Enabled = false;
	PanelStr4->Enabled = false;PanelStr5->Enabled = false;
    BtnStart  ->Enabled=false;
    BtnStop   ->Enabled=true;
    BtnOpt    ->Enabled=false;
    //BtnExit   ->Enabled=false;
	//MenuStart ->Enabled=false;
	//MenuStop  ->Enabled=true;
    //MenuExit  ->Enabled=false;
    SetTrayIcon(1);
}
// set task-tray icon -------------------------------------------------------
void __fastcall TstrsvrMainWindow::SetTrayIcon(int index)
{
	//TIcon *icon=new TIcon;
	//ImageList->GetIcon(index,icon);
	//TrayIcon->Icon=icon;
    //delete icon;
}
// undate enable of widgets -------------------------------------------------
void __fastcall TstrsvrMainWindow::UpdateEnable(void)
{
	//BtnCmd->Enabled=Input->ItemIndex<2||Input->ItemIndex==3;
    LabelOutput1->Font->Color=Output1->ItemIndex>0?clBlack:clGray;
	//LabelOutput2->Font->Color=Output2->ItemIndex>0?clBlack:clGray;
	//LabelOutput3->Font->Color=Output3->ItemIndex>0?clBlack:clGray;
    Output1Byte ->Font->Color=Output1->ItemIndex>0?clBlack:clGray;
	//Output2Byte ->Font->Color=Output2->ItemIndex>0?clBlack:clGray;
	//Output3Byte ->Font->Color=Output3->ItemIndex>0?clBlack:clGray;
	Output1Bps  ->Font->Color=Output1->ItemIndex>0?clBlack:clGray;
	//Output2Bps  ->Font->Color=Output2->ItemIndex>0?clBlack:clGray;
	//Output3Bps  ->Font->Color=Output3->ItemIndex>0?clBlack:clGray;
    BtnOutput1->Enabled=Output1->ItemIndex>0;
	//BtnOutput2->Enabled=Output2->ItemIndex>0;
	//BtnOutput3->Enabled=Output3->ItemIndex>0;
    //BtnConv1  ->Enabled=BtnOutput1->Enabled;
	//BtnConv2  ->Enabled=BtnOutput2->Enabled;
	//BtnConv3  ->Enabled=BtnOutput3->Enabled;
}
// load options -------------------------------------------------------------
void __fastcall TstrsvrMainWindow::LoadOpt(void)
{
    TIniFile *ini=new TIniFile(IniFile);
    AnsiString s;
    int optdef[]={10000,10000,1000,32768,10,0};

    Input  ->ItemIndex=ini->ReadInteger("set","input",       0);
    Output1->ItemIndex=ini->ReadInteger("set","output1",     0);
	//Output2->ItemIndex=ini->ReadInteger("set","output2",     0);
    //Output3->ItemIndex=ini->ReadInteger("set","output3",     0);
    TraceLevel        =ini->ReadInteger("set","tracelevel",  0);
    NmeaReq           =ini->ReadInteger("set","nmeareq",     0);
    FileSwapMargin    =ini->ReadInteger("set","fswapmargin",30);
    StaId             =ini->ReadInteger("set","staid"       ,0);
    StaSel            =ini->ReadInteger("set","stasel"      ,0);
    AntType           =ini->ReadString ("set","anttype",    "");
    RcvType           =ini->ReadString ("set","rcvtype",    "");

    for (int i=0;i<6;i++) {
        SvrOpt[i]=ini->ReadInteger("set",s.sprintf("svropt_%d",i),optdef[i]);
    }
    for (int i=0;i<3;i++) {
        AntPos[i]=ini->ReadFloat("set",s.sprintf("antpos_%d",i),0.0);
        AntOff[i]=ini->ReadFloat("set",s.sprintf("antoff_%d",i),0.0);
    }
    for (int i=0;i<3;i++) {
        ConvEna[i]=ini->ReadInteger("conv",s.sprintf("ena_%d",i), 0);
        ConvInp[i]=ini->ReadInteger("conv",s.sprintf("inp_%d",i), 0);
        ConvOut[i]=ini->ReadInteger("conv",s.sprintf("out_%d",i), 0);
        ConvMsg[i]=ini->ReadString ("conv",s.sprintf("msg_%d",i),"");
        ConvOpt[i]=ini->ReadString ("conv",s.sprintf("opt_%d",i),"");
    }
    for (int i=0;i<2;i++) {
        CmdEna   [i]=ini->ReadInteger("serial",s.sprintf("cmdena_%d",i),1);
        CmdEnaTcp[i]=ini->ReadInteger("tcpip" ,s.sprintf("cmdena_%d",i),1);
    }
    for (int i=0;i<4;i++) for (int j=0;j<4;j++) {
        Paths[i][j]=ini->ReadString("path",s.sprintf("path_%d_%d",i,j),"");
    }
    for (int i=0;i<2;i++) {
        Cmds[i]=ini->ReadString("serial",s.sprintf("cmd_%d",i),"");
        for (char *p=Cmds[i].c_str();*p;p++) {
            if ((p=strstr(p,"@@"))) strncpy(p,"\r\n",2); else break;
        }
    }
    for (int i=0;i<2;i++) {
        CmdsTcp[i]=ini->ReadString("tcpip",s.sprintf("cmd_%d",i),"");
        for (char *p=CmdsTcp[i].c_str();*p;p++) {
            if ((p=strstr(p,"@@"))) strncpy(p,"\r\n",2); else break;
        }
    }
    for (int i=0;i<MAXHIST;i++) {
        TcpHistory[i]=ini->ReadString("tcpopt",s.sprintf("history%d",i),"");
    }
    for (int i=0;i<MAXHIST;i++) {
        TcpMntpHist[i]=ini->ReadString("tcpopt",s.sprintf("mntphist%d",i),"");
    }
    StaPosFile    =ini->ReadString("stapos","staposfile",    "");
    ExeDirectory  =ini->ReadString("dirs",  "exedirectory",  "");
    LocalDirectory=ini->ReadString("dirs",  "localdirectory","");
    ProxyAddress  =ini->ReadString("dirs",  "proxyaddress",  "");
    delete ini;

    UpdateEnable();
}
//---------------------------------------------------------------------------

void __fastcall TstrsvrMainWindow::FormClose(TObject *Sender, TCloseAction &Action)
{
	//SaveOpt();
}
//---------------------------------------------------------------------------

void __fastcall TstrsvrMainWindow::BtnStartClick(TObject *Sender)
{
	SvrStart();
}
//---------------------------------------------------------------------------
void __fastcall TstrsvrMainWindow::BtnStopClick(TObject *Sender)
{
	SvrStop();
}
// stop stream server -------------------------------------------------------
void __fastcall TstrsvrMainWindow::SvrStop(void)
{
    char *cmd=NULL;

    if (Input->ItemIndex==0) {
        if (CmdEna[1]) cmd=Cmds[1].c_str();
    }
    else if (Input->ItemIndex==1||Input->ItemIndex==3) {
        if (CmdEnaTcp[1]) cmd=CmdsTcp[1].c_str();
    }
    strsvrstop(&strsvr,cmd);

    EndTime=utc2gpst(timeget());
	//Panel1    ->Enabled=true;
    PanelStr1->Enabled = true;  PanelStr2->Enabled = true;PanelStr3->Enabled = true;
	BtnStart  ->Enabled=true;
    BtnStop   ->Enabled=false;
    BtnOpt    ->Enabled=true;
    //BtnExit   ->Enabled=true;
	//MenuStart ->Enabled=true;
	//MenuStop  ->Enabled=false;
	//MenuExit  ->Enabled=true;
    SetTrayIcon(0);

    for (int i=0;i<3;i++) {
        if (ConvEna[i]) strconvfree(strsvr.conv[i]);
    }
    if (TraceLevel>0) traceclose();
}
//---------------------------------------------------------------------------
void __fastcall TstrsvrMainWindow::BtnOptClick(TObject *Sender)
{
	/*for (int i=0;i<6;i++) SvrOptDialog->SvrOpt[i]=SvrOpt[i];
    for (int i=0;i<3;i++) SvrOptDialog->AntPos[i]=AntPos[i];
    for (int i=0;i<3;i++) SvrOptDialog->AntOff[i]=AntOff[i];
    SvrOptDialog->TraceLevel=TraceLevel;
    SvrOptDialog->NmeaReq=NmeaReq;
    SvrOptDialog->FileSwapMargin=FileSwapMargin;
    SvrOptDialog->StaPosFile=StaPosFile;
    SvrOptDialog->ExeDirectory=ExeDirectory;
    SvrOptDialog->LocalDirectory=LocalDirectory;
    SvrOptDialog->ProxyAddress=ProxyAddress;
    SvrOptDialog->StaId=StaId;
    SvrOptDialog->StaSel=StaSel;
    SvrOptDialog->AntType=AntType;
    SvrOptDialog->RcvType=RcvType;

    if (SvrOptDialog->ShowModal()!=mrOk) return;

    for (int i=0;i<6;i++) SvrOpt[i]=SvrOptDialog->SvrOpt[i];
    for (int i=0;i<3;i++) AntPos[i]=SvrOptDialog->AntPos[i];
    for (int i=0;i<3;i++) AntOff[i]=SvrOptDialog->AntOff[i];
    TraceLevel=SvrOptDialog->TraceLevel;
    NmeaReq=SvrOptDialog->NmeaReq;
    FileSwapMargin=SvrOptDialog->FileSwapMargin;
    StaPosFile=SvrOptDialog->StaPosFile;
    ExeDirectory=SvrOptDialog->ExeDirectory;
    LocalDirectory=SvrOptDialog->LocalDirectory;
    ProxyAddress=SvrOptDialog->ProxyAddress;
    StaId=SvrOptDialog->StaId;
    StaSel=SvrOptDialog->StaSel;
    AntType=SvrOptDialog->AntType;
	RcvType=SvrOptDialog->RcvType;*/
}
//---------------------------------------------------------------------------
void __fastcall TstrsvrMainWindow::BtnInputClick(TObject *Sender)
{
	switch(Input->ItemIndex){
		case 0: SerialOpt(0,0); break;  //Serial
		//case 1: TcpOpt(0,1); break;   //TCP Client
		//case 2: TcpOpt(0,0); break;   //TCP Server
		//case 3: TcpOpt(0,3); break;    //NTRIP Client
        case 1: TcpOpt(0,1); break;     //NTRIP Client
		//case 4: FileOpt(0,0); break;   //File
		//case 5: FtpOpt(0,0); break;    //FTP
		//case 6: FtpOpt(0,1); break;    //HTTP
	}
}
// set serial options -------------------------------------------------------
void __fastcall TstrsvrMainWindow::SerialOpt(int index, int opt)
{
    SerialOptDialog->Path=Paths[index][0];
    SerialOptDialog->Opt=opt;
    if (SerialOptDialog->ShowModal()!=mrOk) return;
    Paths[index][0]=SerialOptDialog->Path;
}
// set tcp/ip options -------------------------------------------------------
void __fastcall TstrsvrMainWindow::TcpOpt(int index, int opt)
{
    TcpOptDialog->Path=Paths[index][1];
    TcpOptDialog->Opt=opt;
    for (int i=0;i<MAXHIST;i++) TcpOptDialog->History[i]=TcpHistory[i];
    for (int i=0;i<MAXHIST;i++) TcpOptDialog->MntpHist[i]=TcpMntpHist[i];
	if (TcpOptDialog->ShowModal()!=mrOk)
		return;
    Paths[index][1]=TcpOptDialog->Path;
    for (int i=0;i<MAXHIST;i++) TcpHistory[i]=TcpOptDialog->History[i];
    for (int i=0;i<MAXHIST;i++) TcpMntpHist[i]=TcpOptDialog->MntpHist[i];
}
//---------------------------------------------------------------------------

void __fastcall TstrsvrMainWindow::BtnOutput1Click(TObject *Sender)
{
	switch (Output1->ItemIndex) {
		//case 1: SerialOpt(1,0); break;  //Serial
		//case 2: TcpOpt(1,1); break;     //TCP Client
		//case 3: TcpOpt(1,0); break;     //TCP Server
		//case 4: TcpOpt(1,2); break;     //NTRIP Server
		//case 5: FileOpt(1,1); break;      //File
		case 1: FileOpt(1,1); break;
	}
}
// set file options ---------------------------------------------------------
void __fastcall TstrsvrMainWindow::FileOpt(int index, int opt)
{
    FileOptDialog->Path=Paths[index][2];
    FileOptDialog->Opt=opt;
    if (FileOptDialog->ShowModal()!=mrOk) return;
    Paths[index][2]=FileOptDialog->Path;
}
//---------------------------------------------------------------------------
void __fastcall TstrsvrMainWindow::InputChange(TObject *Sender)
{
	UpdateEnable();
}
//---------------------------------------------------------------------------
void __fastcall TstrsvrMainWindow::Output1Change(TObject *Sender)
{
	UpdateEnable();
}
// number to comma-separated number -----------------------------------------
static void num2cnum(int num, char *str)
{
    char buff[256],*p=buff,*q=str;
    int i,n;
    n=sprintf(buff,"%u",(unsigned int)num);
    for (i=0;i<n;i++) {
        *q++=*p++;
        if ((n-i-1)%3==0&&i<n-1) *q++=',';
    }
    *q='\0';
}
//---------------------------------------------------------------------------
void __fastcall TstrsvrMainWindow::Timer1Timer(TObject *Sender)
{
	TColor color[]={clRed,clBtnFace,CLORANGE,clGreen,clLime};
	//TPanel *e0[]={IndInput,IndOutput1,IndOutput2,IndOutput3};
	//TLabel *e1[]={InputByte,Output1Byte,Output2Byte,Output3Byte};
	TLabel *e1[] = {InputByte,Output1Byte};
	//TLabel *e2[]={InputBps,Output1Bps,Output2Bps,Output3Bps};
	TLabel *e2[] = {InputBps,Output1Bps};
    AnsiString s;
    gtime_t time=utc2gpst(timeget());
    int stat[4]={0},byte[4]={0},bps[4]={0};
    char msg[MAXSTRMSG*4]="",s1[256],s2[256];
    double ctime,t[4];

    strsvrstat(&strsvr,stat,byte,bps,msg);
	//for (int i=0;i<4;i++) {
	for(int i=0;i<2;i++) {
        num2cnum(byte[i],s1);
        num2cnum(bps[i],s2);
		//e0[i]->Color=color[stat[i]+1];
        e1[i]->Caption=s1;
        e2[i]->Caption=s2;
    }
    Progress->Position=!stat[0]?0:MIN(100,(int)(fmod(byte[0]/500.0,110.0)));

    time2str(time,s1,0);
    //Time->Caption=s.sprintf("%s GPST",s1);

	//if (Panel1->Enabled) {
	if(PanelStr1->Enabled && PanelStr2->Enabled && PanelStr3->Enabled&&
		PanelStr4->Enabled&& PanelStr5->Enabled) {
        ctime=timediff(EndTime,StartTime);
    }
    else {
        ctime=timediff(time,StartTime);
    }
    ctime=floor(ctime);
    t[0]=floor(ctime/86400.0); ctime-=t[0]*86400.0;
    t[1]=floor(ctime/3600.0 ); ctime-=t[1]*3600.0;
    t[2]=floor(ctime/60.0   ); ctime-=t[2]*60.0;
    t[3]=ctime;
	//ConTime->Caption=s.sprintf("%.0fd %02.0f:%02.0f:%02.0f",t[0],t[1],t[2],t[3]);

    num2cnum(byte[0],s1); num2cnum(bps[0],s2);
    //TrayIcon->Hint=s.sprintf("%s bytes %s bps",s1,s2);
    SetTrayIcon(stat[0]<=0?0:(stat[0]==3?2:1));

	Message->Caption=msg;
}
//---------------------------------------------------------------------------

void __fastcall TstrsvrMainWindow::Timer2Timer(TObject *Sender)
{
	unsigned char *msg;
    int len;

    lock(&strsvr.lock);

    len=strsvr.npb;
    if (len>0&&(msg=(unsigned char *)malloc(len))) {
        memcpy(msg,strsvr.pbuf,len);
        strsvr.npb=0;
    }

	unlock(&strsvr.lock);

	if (len<=0||!msg)
		return;

	char buff[200+16],*p=buff,c;
	for(int i=0;i<len;i++){
		if(CheckBoxHEX->Checked)
			sprintf(p,"%02X",msg[i]);
		else
			sprintf(p,"%c",msg[i]);

		this->MemoStreamStr->Text = this->MemoStreamStr->Text + (UnicodeString)p +" ";
		if(MemoStreamStr->Text.Length()>800)
        	MemoStreamStr->Text= "";
	}
	//Console->AddMsg(msg,len);

    free(msg);
}
//---------------------------------------------------------------------------

