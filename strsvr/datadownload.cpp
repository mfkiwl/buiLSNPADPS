
#include "strsvrmain.h"

#pragma link "IdAllFTPListParsers"

//---------------------------------------------------------------------------
void __fastcall TstrsvrMainWindow::btn_ConnectClick(TObject *Sender)
{
	if(this->edt_ServerAddress->Text.Trim()=="")
		return;

	this->btn_Connect->Enabled = false;

	if(this->idftp_Client->Connected()){
		try{
			if(this->FTransferrignData)
				this->idftp_Client->Abort();
			//this->idftp_Client->Quit();
			//this->idftp_Client->Free();
			this->idftp_Client->Disconnect();
            this->edt_CurrentDirectory->Text = "";
		}
		__finally{
			this->btn_Connect->Caption = "连接";
			this->edt_CurrentDirectory->Text = "/";
			for(int i=1;i<this->StringGrid1->RowCount;i++)
				for(int j=0;j<this->StringGrid1->ColCount;j++)
					this->StringGrid1->Cells[j][i] = "";
			this->btn_Connect->Enabled = true;
		}
	}
	else {
		try{
			try{
            	this->idftp_Client->Passive = true;
				this->idftp_Client->Username = Trim(this->edt_UserName->Text);
				this->idftp_Client->Password = Trim(this->edt_UserPassword->Text);
				this->idftp_Client->Host = Trim(this->edt_ServerAddress->Text);
				//this->idftp_Client->ReadTimeout = 3000;
				//this->idftp_Client->ConnectTimeout = 3000;
				this->idftp_Client->Connect();

				this->ChageDir(".");
			}
			catch(Exception &ex){
				Application->MessageBoxA(L"连接失败-网络原因",L"提示",MB_OK);
			}
			catch(...){
				try{
					throw Exception("");
				}
				catch (Exception &ex){
					Application->MessageBoxA(L"连接失败-网络原因",L"提示",MB_OK);
				}
			}
        }
		__finally{
			this->btn_Connect->Enabled = true;
			if(this->idftp_Client->Connected()){
				this->btn_Connect->Caption = "断开连接";
			}
        }
	}
}
//---------------------------------------------------------------------------
void __fastcall TstrsvrMainWindow::ChageDir(String DirName)
{
	TStringList *LS = new TStringList();
	int i;
	try{
		try{
        	this->idftp_Client->ChangeDir(AnsiToUtf8(DirName));
			this->idftp_Client->TransferType = ftASCII;
			this->edt_CurrentDirectory->Text = Utf8ToAnsi(this->idftp_Client->RetrieveCurrentDir());
			this->idftp_Client->List(LS);

			this->StringGrid1->RowCount = idftp_Client->DirectoryListing->Count;

			for(i=0;i<this->idftp_Client->DirectoryListing->Count-1;i++){
				this->StringGrid1->Cells[0][i+1] = (AnsiString)(idftp_Client->DirectoryListing->Items[i]->FileName);
				this->StringGrid1->Cells[1][i+1] = IntToStr(idftp_Client->DirectoryListing->Items[i]->Size);
				if(this->idftp_Client->DirectoryListing->Items[i]->ItemType == ditDirectory){
					this->StringGrid1->Cells[2][i+1] = "文件夹";
				}
				else{
					this->StringGrid1->Cells[2][i+1] = "文件";
				}
				this->StringGrid1->Cells[3][i+1] = DateTimeToStr(idftp_Client->DirectoryListing->Items[i]->ModifiedDate);
				this->StringGrid1->Cells[4][i+1] = idftp_Client->DirectoryListing->Items[i]->PermissionDisplay;
			}
		}
		catch(Exception &e){
        	ShowMessage(e.Message);
		}
		catch(...){
        	ShowMessage("异常错误");
		}
	}
	__finally{
		LS->Free();
	}
}
//---------------------------------------------------------------------------

void __fastcall TstrsvrMainWindow::btn_EnterDirectoryClick(TObject *Sender)
{
	if(!this->idftp_Client->Connected()){
		return;
	}
	this->ChageDir(this->edt_CurrentDirectory->Text);
}
//---------------------------------------------------------------------------

void __fastcall TstrsvrMainWindow::btn_BackClick(TObject *Sender)
{
	if(!this->idftp_Client->Connected()){
		return;
	}
	this->ChageDir("..");
}
//---------------------------------------------------------------------------

void __fastcall TstrsvrMainWindow::btn_DownloadClick(TObject *Sender)
{
	UnicodeString strName;

	if(!this->idftp_Client->Connected()){
		return;
	}
	this->btn_Download->Enabled = false;

	int nCol = this->StringGrid1->Col,nRow=this->StringGrid1->Row;
	//TPoint pt = StringGrid1->ScreenToClient(Mouse->CursorPos);
	//StringGrid1->MouseToCell(pt.X,pt.Y,nCol,nRow);


	if(nCol >-1 && nRow > 0){
		strName = idftp_Client->DirectoryListing->Items[nRow-1]->FileName;
		if(idftp_Client->DirectoryListing->Items[nRow-1]->ItemType == ditDirectory){
			return;
		}
		else{
			this->dlgSave_File->FileName = Utf8ToAnsi(strName);
			if(dlgSave_File->Execute()){
				this->idftp_Client->TransferType = ftBinary;
				this->FBytesToTransfer = this->idftp_Client->Size(strName);
				if(FileExists(dlgSave_File->FileName)){
					switch(MessageDlg("文件已经存在，是否要继续下载？",  mtConfirmation, mbYesNoCancel, 0)) {
						case mrCancel:
							return;
						case mrYes:
							FBytesToTransfer = FBytesToTransfer - FileSizeByName(strName);
						case mrNo:
							this->idftp_Client->Get(strName,dlgSave_File->FileName,false);
						default:
							return;
					}
				}
				else{
					this->idftp_Client->Get(strName,dlgSave_File->FileName,false);
				}
			}
		}
	}
	else{
        this->btn_Download->Enabled = true;
		return;
    }

	ShowMessage("下载完成");
	this->btn_Download->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TstrsvrMainWindow::StringGrid1DblClick(TObject *Sender)
{
	if(!this->idftp_Client->Connected()){
		return;
	}

	int nCol = this->StringGrid1->Col,nRow=this->StringGrid1->Row;
	//TPoint pt = StringGrid1->ScreenToClient(Mouse->CursorPos);
	//StringGrid1->MouseToCell(pt.X,pt.Y,nCol,nRow);

	if(nCol >-1 && nRow > 0){
		if(!this->idftp_Client->Connected()){
			return;
		}
		if(this->idftp_Client->DirectoryListing->Items[nRow-1]->ItemType == ditDirectory){
			this->ChageDir((AnsiString)(idftp_Client->DirectoryListing->Items[nRow-1]->FileName));
		}
	}
	else
		return;
}
//---------------------------------------------------------------------------
