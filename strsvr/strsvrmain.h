//---------------------------------------------------------------------------

#ifndef strsvrmainH
#define strsvrmainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>

#include "rtklib.h"
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <IdAntiFreezeBase.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdExplicitTLSClientServerBase.hpp>
#include <IdFTP.hpp>
#include <IdGlobal.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.IdAntiFreeze.hpp>

#define MAXHIST		10

//---------------------------------------------------------------------------
class TstrsvrMainWindow : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel2;
	TPanel *Panel21;
	TPanel *Panel22;
	TPanel *Panel211;
	TPanel *Panel212;
	TGroupBox *GroupBoxDateConv;
	TGroupBox *GroupBoxDownloadTips;
	TGroupBox *GroupBoxStreamRe;
	TGroupBox *GroupBoxDataDownload;
	TEdit *EditMJD;
	TLabel *LabelMJD;
	TEdit *EditDayOfYear;
	TLabel *LabelDayOfYear;
	TEdit *EditGPSWeek;
	TEdit *EditYearMonthDay;
	TLabel *LabelGPSWeek;
	TLabel *LabelYearMonthDay;
	TMemo *MemoDataDownload;
	TProgressBar *Progress;
	TLabel *Message;
	TBitBtn *BtnOpt;
	TBitBtn *BtnStop;
	TBitBtn *BtnStart;
	TPanel *PanelStr1;
	TPanel *PanelStr2;
	TLabel *LabelOutput1;
	TLabel *LabelInput;
	TLabel *Label5;
	TComboBox *Output1;
	TComboBox *Input;
	TLabel *Label6;
	TPanel *PanelStr3;
	TSpeedButton *BtnOutput1;
	TSpeedButton *BtnInput;
	TLabel *Label7;
	TPanel *PanelStr4;
	TPanel *PanelStr5;
	TLabel *Output1Byte;
	TLabel *InputByte;
	TLabel *Label8;
	TLabel *Output1Bps;
	TLabel *InputBps;
	TLabel *Label9;
	TPanel *PanelDataDownload;
	TStringGrid *StringGrid1;
	TLabel *LabelAddress;
	TEdit *edt_ServerAddress;
	TEdit *edt_UserName;
	TLabel *LabelUserName;
	TEdit *edt_UserPassword;
	TLabel *LabelPassWord;
	TButton *btn_Connect;
	TButton *btn_Download;
	TEdit *edt_CurrentDirectory;
	TLabel *LabelCurDir;
	TSpeedButton *btn_EnterDirectory;
	TSpeedButton *btn_Back;
	TSaveDialog *dlgSave_File;
	TIdFTP *idftp_Client;
	TTimer *Timer1;
	TTimer *Timer2;
	TMemo *MemoStreamStr;
	TCheckBox *CheckBoxHEX;
	void __fastcall FormResize(TObject *Sender);
	void __fastcall EditYearMonthDayChange(TObject *Sender);
	void __fastcall EditGPSWeekChange(TObject *Sender);
	void __fastcall EditDayOfYearChange(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall btn_ConnectClick(TObject *Sender);
	void __fastcall btn_EnterDirectoryClick(TObject *Sender);
	void __fastcall btn_BackClick(TObject *Sender);
	void __fastcall btn_DownloadClick(TObject *Sender);
	void __fastcall StringGrid1DblClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall BtnStartClick(TObject *Sender);
	void __fastcall BtnInputClick(TObject *Sender);
	void __fastcall BtnStopClick(TObject *Sender);
	void __fastcall BtnOptClick(TObject *Sender);
	void __fastcall BtnOutput1Click(TObject *Sender);
	void __fastcall InputChange(TObject *Sender);
	void __fastcall Output1Change(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall Timer2Timer(TObject *Sender);

private:	// User declarations
	bool FTransferrignData;
	long FBytesToTransfer;
	bool FAbortTransfer;
	double STime;
	double FAverageSpeed;

	AnsiString IniFile;
	AnsiString Paths[4][4],Cmds[2],CmdsTcp[2];
	AnsiString TcpHistory[MAXHIST],TcpMntpHist[MAXHIST];
	AnsiString StaPosFile,ExeDirectory,LocalDirectory,SwapInterval;
	AnsiString ProxyAddress;
	AnsiString ConvMsg[3],ConvOpt[3],AntType,RcvType;
	int ConvEna[3],ConvInp[3],ConvOut[3],StaId,StaSel;
	int TraceLevel,SvrOpt[6],CmdEna[2],CmdEnaTcp[2],NmeaReq,FileSwapMargin;
	double AntPos[3],AntOff[3];
	gtime_t StartTime,EndTime;

	void __fastcall ChageDir(String DirName);

    void __fastcall FileOpt(int index, int opt);
	void __fastcall SerialOpt(int index, int opt);
	void __fastcall TcpOpt(int index, int opt);
    void __fastcall SvrStop(void);
	void __fastcall SvrStart(void);
	void __fastcall LoadOpt(void);
	void __fastcall SetTrayIcon(int index);
    void __fastcall UpdateEnable(void);

public:		// User declarations
	__fastcall TstrsvrMainWindow(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TstrsvrMainWindow *strsvrMainWindow;
//---------------------------------------------------------------------------
#endif
