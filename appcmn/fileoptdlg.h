//---------------------------------------------------------------------------

#ifndef fileoptdlgH
#define fileoptdlgH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.Dialogs.hpp>
//---------------------------------------------------------------------------
class TFileOptDialog : public TForm
{
__published:	// IDE-managed Components
	TButton *BtnCancel;
	TButton *BtnOk;
	TLabel *Label1;
	TEdit *FilePath;
	TSpeedButton *BtnFilePath;
	TOpenDialog *OpenDialog;
	TSaveDialog *SaveDialog;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall BtnOkClick(TObject *Sender);
	void __fastcall BtnFilePathClick(TObject *Sender);
private:	// User declarations
    void __fastcall UpdateEnable(void);

public:		// User declarations
	int Opt;
	AnsiString Path;
	__fastcall TFileOptDialog(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFileOptDialog *FileOptDialog;
//---------------------------------------------------------------------------
#endif
