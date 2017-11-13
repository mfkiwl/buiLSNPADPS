//---------------------------------------------------------------------------

#ifndef serioptdlgH
#define serioptdlgH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
//---------------------------------------------------------------------------
class TSerialOptDialog : public TForm
{
__published:	// IDE-managed Components
	TComboBox *Parity;
	TComboBox *StopBits;
	TComboBox *FlowCtr;
	TLabel *Label8;
	TLabel *Label5;
	TLabel *Label4;
	TComboBox *BitRate;
	TComboBox *ByteSize;
	TLabel *Label2;
	TLabel *Label1;
	TComboBox *Port;
	TLabel *Label3;
	TButton *BtnCancel;
	TButton *BtnOk;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall BtnOkClick(TObject *Sender);

private:	// User declarations
	void __fastcall UpdatePortList(void);

public:		// User declarations
	AnsiString Path,Cmds[2];
	int Opt,CmdEna[2];

	__fastcall TSerialOptDialog(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSerialOptDialog *SerialOptDialog;
//---------------------------------------------------------------------------
#endif
