# -*- coding: utf-8 -*-

# @Author: 郑海
# @Email : 147320920@qq.com
# @Time  : 2019/8/9 16:45
# @File  : 字体文件自定义字符转换.py


import os

enterOld = '''
GUI_CONST_STORAGE unsigned char acGUI_FontSongTi16_2199[ 32] = { /* code 2199, SOUTH WEST ARROW */
  ________,________,
  ________,________,
  ________,_____X__,
  ________,____X___,
  ________,___X____,
  ________,__X_____,
  ________,_X______,
  ________,X_______,
  ____X__X,________,
  ____X_X_,________,
  ____XX__,________,
  ___XXXXX,________,
  ___XX___,________,
  __X_____,________,
  ________,________,
  ________,________};
'''
enterNew = '''
GUI_CONST_STORAGE unsigned char acGUI_FontSongTi16_2199[ 32] = { /* code 2199, SOUTH WEST ARROW */
  ________,________,
  ________,________,
  ________,________,
  ________,________,
  ________,________,
  ________,______X_,
  ________,______X_,
  ____X___,______X_,
  ___XX___,______X_,
  __XXX___,______X_,
  _XXXXXXX,XXXXXXX_,
  __XXX___,________,
  ___XX___,________,
  ____X___,________,
  ________,________,
  ________,________};
'''




with open('./Font/SongTi16.c', 'r+') as f:
    fileContent = f.read()
    # print(fileContent)
    reWrite = False
    if fileContent.find(enterOld) >= 0:

        fileContent = fileContent.replace(enterOld, enterNew)
        reWrite = True
        print('回车键符号 替换')
    if reWrite:
        f.seek(0)
        f.write(fileContent)
        f.flush()
    else:
        print('不需要替换')
os.system('pause')


