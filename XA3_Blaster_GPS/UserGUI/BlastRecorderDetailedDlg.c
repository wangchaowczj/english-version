#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },	
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回  
	{ BUTTON_CreateIndirect, "OK", GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//确定
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT0,     15, 40, 240,  25,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT1,     15, 65, 240,  25,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT2,     15, 90, 240,  25,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT3,     15, 115, 240,  25,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT4,     15, 140, 240,  25,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT5,     15, 165, 240,  25,  0,          0 },
	{ TEXT_CreateIndirect, "", GUI_ID_TEXT6,     15, 190, 240,  25,  0,          0 }		
};

static u8 DataPageindex = 0;

static void ShowData(WM_HWIN hWin)
{
	u8 FileName[17];
	u8 Buffer[50];
	u8* FilePath;
	char GBK[50];
	int U32Value;
	
	FilePath = (u8*)GetBlasterPath();
		
	memcpy(FileName, &BlastRecorder.FileName[BlastRecorder.Select][0],12);
	strcpy((char*)&FileName[12],".txt");
	
	switch(DataPageindex)
	{
		case 0:
			if(Op_Data.BlastRecorderDeviceUsages == DEVICE_USAGES_ALONE)
			{
				TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0), "Device Type:blaster box");//使用方式:控制盒
			}
			else
			{
				TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0), "Device Type:decoder");//使用方式:译码器
			}
						
			UTF8StringToGBK("软件版本", (char*)&GBK[1]);
			GBK[0] = Op_Data.BlastRecorderDeviceName;
			if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Buffer[0],50, FilePath, FileName))
			{
				Buffer[0] = ' ';
				Buffer[1] = 0x00;
			}
			sprintf((char*)&Op_Data.Temp[0], "SW Ver:%s", Buffer);//软件版本:
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1), (char*)Op_Data.Temp);
			
			UTF8StringToGBK("译码器ID", (char*)&GBK[1]);
			GBK[0] = Op_Data.BlastRecorderDeviceName;
			if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Buffer[0],50, FilePath, FileName))
			{
				Buffer[0] = ' ';
				Buffer[1] = 0x00;
			}
			sprintf((char*)&Op_Data.Temp[0], "Device ID:%s", Buffer);//设备编号:
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT2), (char*)Op_Data.Temp);						
			
			UTF8StringToGBK("注册雷管数", (char*)&GBK[1]);
			GBK[0] = Op_Data.BlastRecorderDeviceName;
			if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Buffer[0],50, FilePath, FileName))
			{
				Buffer[0] = '0';
				Buffer[1] = 0x00;
			}
			sscanf((char*)Buffer, "%d", (int*)&U32Value);
			Op_Data.BlastRecorderDeviceRegNumber = U32Value;

			sprintf((char*)&Op_Data.Temp[0], "DET QTY:%s", Buffer);//注册雷管数:
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT3), (char*)Op_Data.Temp);	

			
			UTF8StringToGBK("错误雷管数", (char*)&GBK[1]);
			GBK[0] = Op_Data.BlastRecorderDeviceName;
			if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Buffer[0],50, FilePath, FileName))
			{
				Buffer[0] = ' ';
				Buffer[1] = 0x00;
			}
			sprintf((char*)&Op_Data.Temp[0], "ERR DET Sum:%s", Buffer);//错误雷管数:
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT4), (char*)Op_Data.Temp);	
		
			UTF8StringToGBK("雷管类型", (char*)&GBK[1]);
			GBK[0] = Op_Data.BlastRecorderDeviceName;
			if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Buffer[0],50, FilePath, FileName))
			{
				Buffer[0] = ' ';
				Buffer[1] = 0x00;
			}
			sscanf((char*)&Buffer[0], "0x%02X", &U32Value);
			GetDetInfoString((char*)Op_Data.Temp, U32Value);
			sprintf((char*)&Op_Data.Temp[0], "DET Type:%s", Buffer);//原来这里被注释掉了 雷管类型：
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT5), (char*)Op_Data.Temp);
			

//			UTF8StringToGBK("雷管测试后总线电压", (char*)&GBK[1]);
//			GBK[0] = Op_Data.BlastRecorderDeviceName;
//			if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Buffer[0],50, FilePath, FileName))
//			{
//				Buffer[0] = ' ';
//				Buffer[1] = 0x00;
//			}
//			sprintf((char*)&Op_Data.Temp[0], "测试后电压:%s", Buffer);
//			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT6), (char*)Op_Data.Temp);
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT6), "");
			break;
		case 1:
			UTF8StringToGBK("雷管测试后总线电压", (char*)&GBK[1]);
			GBK[0] = Op_Data.BlastRecorderDeviceName;
			if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Buffer[0],50, FilePath, FileName))
			{
				Buffer[0] = ' ';
				Buffer[1] = 0x00;
			}
			sprintf((char*)&Op_Data.Temp[0], "V After Test:%s", Buffer);//测试后电压:
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0), (char*)Op_Data.Temp);

			UTF8StringToGBK("雷管测试后总线电流", (char*)&GBK[1]);
			GBK[0] = Op_Data.BlastRecorderDeviceName;
			if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Buffer[0],50, FilePath, FileName))
			{
				Buffer[0] = ' ';
				Buffer[1] = 0x00;
			}
			sprintf((char*)&Op_Data.Temp[0], "I After Test:%s", Buffer);//测试后电流:
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT1), (char*)Op_Data.Temp);

			UTF8StringToGBK("网路充电后总线电压", (char*)&GBK[1]);
			GBK[0] = Op_Data.BlastRecorderDeviceName;
			if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Buffer[0],50, FilePath, FileName))
			{
				Buffer[0] = ' ';
				Buffer[1] = 0x00;
			}
			sprintf((char*)&Op_Data.Temp[0], "V After Charge:%s", Buffer);//充电后电压:
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT2), (char*)Op_Data.Temp);

			UTF8StringToGBK("网路充电后总线电流", (char*)&GBK[1]);
			GBK[0] = Op_Data.BlastRecorderDeviceName;
			if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Buffer[0],50, FilePath, FileName))
			{
				Buffer[0] = ' ';
				Buffer[1] = 0x00;
			}
			sprintf((char*)&Op_Data.Temp[0], "I After Charge:%s", Buffer);//充电后电流:
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT3), (char*)Op_Data.Temp);

			UTF8StringToGBK("起爆前总线电压", (char*)&GBK[1]);
			GBK[0] = Op_Data.BlastRecorderDeviceName;
			if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Buffer[0],50, FilePath, FileName))
			{
				Buffer[0] = ' ';
				Buffer[1] = 0x00;
			}
			sprintf((char*)&Op_Data.Temp[0], "V Before Blast:%s", Buffer);//起爆前电压:
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT4), (char*)Op_Data.Temp);

			UTF8StringToGBK("起爆前总线电流", (char*)&GBK[1]);
			GBK[0] = Op_Data.BlastRecorderDeviceName;
			if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Buffer[0],50, FilePath, FileName))
			{
				Buffer[0] = ' ';
				Buffer[1] = 0x00;
			}
			sprintf((char*)&Op_Data.Temp[0], "I Before Blast:%s", Buffer);//起爆前电流:
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT5), (char*)Op_Data.Temp);	


			UTF8StringToGBK("总线漏电流", (char*)&GBK[1]);
			GBK[0] = Op_Data.BlastRecorderDeviceName;
			if(ERROR == GetConfigFileString((u8*)GBK, (u8*)&Buffer[0],50, FilePath, FileName))
			{
				Buffer[0] = ' ';
				Buffer[1] = 0x00;
			}
			sprintf((char*)&Op_Data.Temp[0], "Leakage Current:%s", Buffer);//总线漏电流:
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT6), (char*)Op_Data.Temp);					
			break;
		default:
			break;
	}
}

/*对话框的回调函数*/
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	char temp[50];
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();

			ShowInfoTitle("Blast Info");	//起爆信息				
			GUI_SetFont(&SMALL_FONT);			
			
			ShowInfoHelp("Press the Arrow Key to scroll", NULL);	//方向键翻页
			break;
		case WM_INIT_DIALOG://初始化界面		
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_LEFT|TEXT_CF_VCENTER);			
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT1), TEXT_CF_LEFT|TEXT_CF_VCENTER);	
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT2), TEXT_CF_LEFT|TEXT_CF_VCENTER);
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT3), TEXT_CF_LEFT|TEXT_CF_VCENTER);	
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT4), TEXT_CF_LEFT|TEXT_CF_VCENTER);	
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT5), TEXT_CF_LEFT|TEXT_CF_VCENTER);	
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT6), TEXT_CF_LEFT|TEXT_CF_VCENTER);

			if(BlastRecorder.Select >= BlastRecorder.Number)
			{
				BlastRecorder.Select = 0;			
			}
			DataPageindex = 0;
			ShowData(hWin);				
			if(Op_Data.BlastRecorderDeviceRegNumber == 0)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
			}
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
			}			
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_500MS_ID, GUI_TIMER_500MS, 0);
		
			sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
			BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);				
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
			{
				case GUI_KEY_LEFT:
					if(Op_Data.BlastRecorderDeviceUsages == DEVICE_USAGES_MASTER)
					{//显示译码器列表
						ShowBlastRecorderSlaveInfoListDlg();
					}
					else
					{
						ShowBlastRecorderListViewDlg();
					}				
					break;
				case GUI_KEY_RIGHT:
					if(Op_Data.BlastRecorderDeviceRegNumber != 0)
					{
						ShowBlastRecorderDetInfoListDlg();
					}					
					break;
				case GUI_KEY_2:
				case GUI_KEY_4:
					if(DataPageindex == 0)
					{
						DataPageindex = 1;
					}
					else
					{
						DataPageindex--;
					}
					ShowData(hWin);					

					break;
				case GUI_KEY_6:
				case GUI_KEY_8:
					if(DataPageindex >= 1)
					{
						DataPageindex = 0;
					}
					else
					{
						DataPageindex++;
					}
					ShowData(hWin);					
					break;					
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_500MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_500MS);
					
					sprintf(temp, "%.1fV %duA", Op_Data.LinABVoltage/1000.0, Op_Data.LinABCurrent);
					BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON1), temp);				
					break;
				default:
					break;
			}		
			break;			
		default:
			WM_DefaultProc(pMsg);
	}
}

void ShowBlastRecorderDetailedDlg(void)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
}
