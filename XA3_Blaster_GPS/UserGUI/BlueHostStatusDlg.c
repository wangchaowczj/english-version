#include "includes.h"
#include "GuiIncludes.h"

/*主界面的资源链表*/
static const GUI_WIDGET_CREATE_INFO Res[] = {
	{ WINDOW_CreateIndirect, "",    0,   0, 0,  240, 300, 0,          0 },
	{ BUTTON_CreateIndirect, "Back",GUI_ID_BUTTON0,     BUTTON_SMALL_LEFT_X,  BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//返回
	{ BUTTON_CreateIndirect, "", GUI_ID_BUTTON1,     BOTTOM_STATUS_X, BUTTON_SMALL_Y-20, BOTTOM_STATUS_WIDTH, BUTTON_SMALL_HEIGHT,  0,          0 },
	{ BUTTON_CreateIndirect,   "Connect",       GUI_ID_BUTTON2,     BUTTON_SMALL_RIGHT_X, BUTTON_SMALL_Y-20, BUTTON_SMALL_WIDTH,  BUTTON_SMALL_HEIGHT,  0,          0 },//连接
	{ TEXT_CreateIndirect, " ", GUI_ID_TEXT0,     0, 110, 240,  25,  0,          0 },
	{ LISTVIEW_CreateIndirect, NULL, GUI_ID_LISTVIEW0, 19, INFO_TITLE_LINE_Y+4, 212, 179, 0, 0 }	
};

static void ShowListViewData(WM_HWIN hWin)
{
	u8 i;
	u8 Number;
	WM_HWIN hItem;

	u8 Buffer[32];
	
	hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
	Number = LISTVIEW_GetNumRows(hItem);
	
	while(Number--)
	{
		LISTVIEW_DeleteRow(hItem, 0);
	}
	for(i=0; i < BlueSlaveList.Count; i++)
	{
		LISTVIEW_AddRow(hItem, NULL);
			
		sprintf((char*)Buffer,"%d", i+1);
		LISTVIEW_SetItemText(hItem, 0,i, (char*)Buffer);						

		sprintf((char*)Buffer,"%s", BlueSlaveList.Info[i].Name);
		LISTVIEW_SetItemText(hItem, 1,i, (char*)Buffer);
	}
}

/**
  *************************************************************************************
  * @brief  设置对话框的参数，对话框根据参数进行内空显示
  * @param  Param  对话框参数
  *         @arg @b 0 起爆流程用户名密码验证	
  *         @arg @b 1 修改用户名密码
  *         @arg @b 2 数据备份 
  *         @arg @b 3 数据合并   
  * @retval 无
  * @author ZH
  * @date   2014年11月20日
  * @note   
  *************************************************************************************  
  */
static void Callback(WM_MESSAGE * pMsg) 
{
	WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	char temp[SCAN_DATA_MAX_LEN+1];	
	static u8 DlyStatus= 0;//
	HEADER_Handle hHeader ;	
	
	switch (pMsg->MsgId) 
	{
		case WM_PAINT://绘制界面
			GUI_Clear();

			ShowInfoTitle("Connect Bluetooth");//连接蓝牙			

			
			if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
			{
				sprintf(&temp[0], "Decoder:%d/%d", GetSlaveStateCount(SLAVE_TRUE),SlaveInfo.Number);//在线译码器:
				sprintf(&temp[40], "Log DET:%u ERR DET:%u",SlaveInfo.RegCount, SlaveInfo.ErrorCount);//注册雷管:  		错误雷管:			
			}
			else
			{
				sprintf(&temp[0], "Log DET:%d ERR DET:%d",Det.RegCount, Det.ErrorCount);//注册雷管:  		错误雷管:	
				GetDetInfoString(&temp[40], Det.Type);							
			}
			ShowInfoHelp(&temp[0], &temp[40]);
			
			GUI_SetFont(&SMALL_FONT);				
			break;
		case WM_INIT_DIALOG://初始化界面
			TEXT_SetTextAlign(WM_GetDialogItem(hWin, GUI_ID_TEXT0), TEXT_CF_HCENTER|TEXT_CF_VCENTER);

			//表内容设置	
			hItem = WM_GetDialogItem(hWin, GUI_ID_LISTVIEW0);
			LISTVIEW_SetRowHeight(hItem, 16+1);	
			LISTVIEW_SetBkColor(hItem, 0, GUI_WHITE);//没有选中的行为为白底
			LISTVIEW_SetBkColor(hItem, 1, GUI_BLUE);//选中的行为蓝底
			LISTVIEW_SetGridVis(hItem, 1);	//带分隔	

			//表头设置
			hHeader = LISTVIEW_GetHeader(hItem);//获取表头句柄
			HEADER_SetHeight(hHeader, 16+6);//设置表头高度;
			HEADER_SetBkColor(hHeader, GUI_BLUE);//表头背景
			HEADER_SetTextColor(hHeader, GUI_WHITE);//表头字体颜色
			
			LISTVIEW_AddColumn(hItem, 40, "SN", GUI_TA_CENTER);//序号
			LISTVIEW_AddColumn(hItem, 212-40, "Bluetooth Name", GUI_TA_CENTER);//蓝牙名称

			HEADER_SetTextAlign(hHeader,0, GUI_TA_HCENTER|GUI_TA_VCENTER);
			HEADER_SetTextAlign(hHeader,1, GUI_TA_HCENTER|GUI_TA_VCENTER);
		
			DlyStatus = 0;//蓝牙模块主机初始化
			
			TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Initializing The Bluetooth...");//初始化蓝牙...
								
			//蓝牙模块主机初始化
			OSTaskDel(APP_TEMPORARY_TASK_PRIO);
			TaskCreate(APP_TEMPORARY_TASK_PRIO, (void*)1);
				
			WM_CreateTimer(WM_GetClientWindow(hWin), GUI_TIMER_100MS_ID, GUI_TIMER_100MS, 0);//查询方式判断蓝牙是否完成初始化
			
			WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
			WM_HideWindow(WM_GetDialogItem(hWin , GUI_ID_LISTVIEW0));
			
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
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON0)))
					{
						OSTaskDel(APP_TEMPORARY_TASK_PRIO);
						CLOSE_BLUE_POWER();
						switch(GetWindowParam())
						{
							case 0://起爆流程用户名密码验证
                                if(DEVICE_USAGES_MASTER == device_info.DeviceUsages)
                                {
                                    ShowMainMenuDlg(0);
                                }
                                else
                                {
                                    ShowNetTestReportDlg(2);
                                }
								break;
							case 1://修改用户名密码
								ShowSystemSetMenuDlg();
								break;
							case 2://雷管数据备份
								ShowDetBackupOperateTypeSelDlg();
								break;
							case 3://雷管数据合并
								ShowDetBackupOperateTypeSelDlg();
								break;								
							default:
								ShowMainMenuDlg(0);								
								break;
						}						
					}
					break;
				case GUI_KEY_RIGHT:
					if(WM_IsVisible(WM_GetDialogItem(hWin, GUI_ID_BUTTON2)))
					{
						WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));	
						if(DlyStatus == 3)
						{
							DlyStatus = 4;

							WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
							WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
							WM_HideWindow(WM_GetDialogItem(hWin , GUI_ID_LISTVIEW0));							

							TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Connecting Bluetooth...");//连接蓝牙设备...
							
							Op_Data.ConnectBlueIndex = LISTVIEW_GetSel(WM_GetDialogItem(hWin , GUI_ID_LISTVIEW0));//根据选择的密钥进行连接
							
							//蓝牙模块主机连接从机
							TaskCreate(APP_TEMPORARY_TASK_PRIO, (void*)3);						
						}																	
					}
					break;
				case GUI_KEY_2:
					if(DlyStatus == 3)
					{
						if(LISTVIEW_GetSel(WM_GetDialogItem(hWin , GUI_ID_LISTVIEW0)) == 0)
						{
							LISTVIEW_SetSel(WM_GetDialogItem(hWin , GUI_ID_LISTVIEW0), LISTVIEW_GetNumRows(WM_GetDialogItem(hWin , GUI_ID_LISTVIEW0)) -1);
						}
						else
						{
							LISTVIEW_SetSel(WM_GetDialogItem(hWin , GUI_ID_LISTVIEW0), LISTVIEW_GetSel(WM_GetDialogItem(hWin , GUI_ID_LISTVIEW0)) -1);
						}
					}
					break;
				case GUI_KEY_8:
					if(DlyStatus == 3)
					{
						if(LISTVIEW_GetSel(WM_GetDialogItem(hWin , GUI_ID_LISTVIEW0)) == (LISTVIEW_GetNumRows(WM_GetDialogItem(hWin , GUI_ID_LISTVIEW0)) -1))
						{
							LISTVIEW_SetSel(WM_GetDialogItem(hWin , GUI_ID_LISTVIEW0), 0);
						}
						else
						{
							LISTVIEW_SetSel(WM_GetDialogItem(hWin , GUI_ID_LISTVIEW0), LISTVIEW_GetSel(WM_GetDialogItem(hWin , GUI_ID_LISTVIEW0)) +1);
						}						
					}				
					break;					
			}
			break;
		case WM_TIMER:
			switch(WM_GetTimerId(pMsg->Data.v))
			{
				case GUI_TIMER_100MS_ID:
					WM_RestartTimer(pMsg->Data.v, GUI_TIMER_100MS);
					if(DlyStatus == 0)
					{//蓝牙模块主机初始化
						if(!TaskIsExist(APP_TEMPORARY_TASK_PRIO))
						{//已经初始化完成
							if((ErrorStatus)Op_Data.AppTemporaryTaskResult != ERROR)
							{//初始化成功
								DlyStatus = 1;
								TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Scanning The Bluetooth...");//扫描蓝牙设备...	
								
								//蓝牙模块主机扫描从机
								TaskCreate(APP_TEMPORARY_TASK_PRIO, (void*)2);							
							}
							else
							{								
								TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Bluetooth Abnormal!");//蓝牙模块异常!
								WM_DeleteTimer(pMsg->Data.v);
								DlyStatus = 0xFF;							
							}
						}
					}
					else if((DlyStatus == 1) || (DlyStatus == 2))
					{//蓝牙模块主机扫描从机
						if(!TaskIsExist(APP_TEMPORARY_TASK_PRIO))
						{//已经完成
							if ((GetWindowParam() == 0) || (GetWindowParam() == 1))
							{
								BlueSlaveFilter("BK");//过滤掉其他从机,只保留蓝牙密钥
							}
							else if (GetWindowParam() == 2)
							{
								BlueSlaveFilter("BOX_");//过滤掉其他从机,只保留控制盒
							}
							if(BlueSlaveList.Count == 0)
							{//没有找到蓝牙密钥
								if(DlyStatus == 1)
								{//没有搜到从机时，重试一次
									DlyStatus = 2;
																														
									//蓝牙模块主机扫描从机
									TaskCreate(APP_TEMPORARY_TASK_PRIO, (void*)2);							
								}
								else
								{//两次都没有搜到从机
									TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Not finding the bluetooth!");//没有找到蓝牙设备!
									WM_DeleteTimer(pMsg->Data.v);
									DlyStatus = 0xFF;
								}
							}
							else if(BlueSlaveList.Count == 1)
							{//只有一个从机时直接连接
								DlyStatus = 4;
								TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Connecting Bluetooth...");//连接蓝牙设备...
								
								Op_Data.ConnectBlueIndex = 0;
								
								//蓝牙模块主机连接从机
								TaskCreate(APP_TEMPORARY_TASK_PRIO, (void*)3);								
							}
							else
							{//有多个从机，显示从机列表
								DlyStatus = 3;
								
								WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT0));
								WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
								WM_ShowWindow(WM_GetDialogItem(hWin , GUI_ID_LISTVIEW0));//显示列表
								//显示数据
								ShowListViewData(hWin);
								LISTVIEW_SetSel(WM_GetDialogItem(hWin , GUI_ID_LISTVIEW0), 0);
							}
						}					
					}
					else if(DlyStatus == 3)
					{//显示从机列表
						;
					}
					else if(DlyStatus == 4)
					{//连接从机
						if(!TaskIsExist(APP_TEMPORARY_TASK_PRIO))
						{//已经完成
							WM_DeleteTimer(pMsg->Data.v);
							if((ErrorStatus)Op_Data.AppTemporaryTaskResult != ERROR)
							{//连接成功							
								switch(GetWindowParam())
								{
									case 0://起爆流程用户名密码验证										
										ShowBlastEnterDlg();
										break;
									case 1://修改用户名密码
										ShowDkeyPasswdModifyDlg();
										break;
									case 2://雷管数据备份
										ShowDetBlueBackupMenuDlg();
										break;	
									case 3://雷管数据合并
										ShowDetBlueInsertMenuDlg();
										break;										
									default:
										OSTaskDel(APP_TEMPORARY_TASK_PRIO);
										CLOSE_BLUE_POWER();									
										ShowMainMenuDlg(0);											
										break;
								}
							}
							else
							{								
								TEXT_SetText(WM_GetDialogItem(hWin , GUI_ID_TEXT0),"Connect Failed!");//蓝牙设备连接失败!
								DlyStatus = 0xFF;							
							}
						}						
					}					
					break;
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

void ShowBlueHostStatusDlg(u32 Param)
{
	CloseMainDialog();
	DlghWin.Dlg = GUI_CreateDialogBox(Res, GUI_COUNTOF(Res), &Callback, WM_HBKWIN, 0, TOP_TITLE_HEIGHT);
    SetWindowParam(Param);
}
