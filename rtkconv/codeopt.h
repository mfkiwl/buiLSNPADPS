//---------------------------------------------------------------------------

#ifndef codeoptH
#define codeoptH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>

#include "rtklib.h"

//---------------------------------------------------------------------------
class TCodeOptDialog : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TCheckBox *G01;
	TCheckBox *G02;
	TCheckBox *G03;
	TCheckBox *G04;
	TCheckBox *G05;
	TCheckBox *G14;
	TCheckBox *G15;
	TCheckBox *G16;
	TCheckBox *G17;
	TCheckBox *G18;
	TCheckBox *G24;
	TCheckBox *G25;
	TCheckBox *G26;
	TCheckBox *G19;
	TCheckBox *G06;
	TCheckBox *G20;
	TCheckBox *G07;
	TCheckBox *G21;
	TCheckBox *G22;
	TCheckBox *G08;
	TCheckBox *G23;
	TGroupBox *GroupBox2;
	TCheckBox *R01;
	TCheckBox *R02;
	TCheckBox *R14;
	TCheckBox *R19;
	TCheckBox *R44;
	TCheckBox *R45;
	TCheckBox *R46;
	TGroupBox *GroupBox3;
	TCheckBox *E01;
	TCheckBox *E10;
	TCheckBox *E11;
	TCheckBox *E12;
	TCheckBox *E13;
	TCheckBox *E24;
	TCheckBox *E25;
	TCheckBox *E26;
	TCheckBox *E27;
	TCheckBox *E28;
	TCheckBox *E29;
	TCheckBox *E30;
	TCheckBox *E31;
	TCheckBox *E37;
	TCheckBox *E38;
	TCheckBox *E32;
	TCheckBox *E39;
	TCheckBox *E33;
	TCheckBox *E34;
	TGroupBox *GroupBox4;
	TCheckBox *J01;
	TCheckBox *J07;
	TCheckBox *J08;
	TCheckBox *J13;
	TCheckBox *J12;
	TCheckBox *J24;
	TCheckBox *J25;
	TCheckBox *J26;
	TCheckBox *J35;
	TCheckBox *J36;
	TCheckBox *J16;
	TCheckBox *J17;
	TCheckBox *J18;
	TCheckBox *J33;
	TGroupBox *GroupBox5;
	TCheckBox *S01;
	TCheckBox *S24;
	TCheckBox *S25;
	TCheckBox *S26;
	TGroupBox *GroupBox6;
	TCheckBox *C47;
	TCheckBox *C42;
	TCheckBox *C27;
	TCheckBox *C28;
	TCheckBox *C29;
	TCheckBox *C48;
	TCheckBox *C12;
	TCheckBox *C43;
	TCheckBox *C33;
	TButton *BtnCancel;
	TButton *BtnOk;
	TButton *BtnSetAll;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall BtnOkClick(TObject *Sender);
	void __fastcall BtnSetAllClick(TObject *Sender);

private:	// User declarations
    void __fastcall UpdateEnable(void);

public:		// User declarations
	int NavSys,FreqType;
	int gpscodeopt[21],glocodeopt[7],galcodeopt[19],qzsscodeopt[14],cmpcodeopt[9],sbascodeopt[4];
    AnsiString gpscodes[21],glocodes[7],galcodes[19],qzsscodes[14],cmpcodes[9],sbascodes[4];
	__fastcall TCodeOptDialog(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TCodeOptDialog *CodeOptDialog;
//---------------------------------------------------------------------------
#endif
