@echo off
title moecraft
echo  ʹ��ǰ�밲װ.NetFramework4.0���ϰ汾�����п�
echo ��1��   ע������
echo ��2��   ע������
choice /c:12
if errorlevel 2 goto unreg
if errorlevel 1 goto reg
pause
:reg
echo  ���ڿ�ʼ��cqlib.dll����ע��
set hh=%~dp0&if "!hh:~-1!"=="\" set hh=!hh:~,-1!
path %hh%
cd C:\Windows\Microsoft.NET\Framework\v4.0.30319
RegAsm.exe cqlib.dll
pause
goto exit
:unreg
echo  ���ڿ�ʼ��cqlib.dll����ע��
set hh=%~dp0&if "!hh:~-1!"=="\" set hh=!hh:~,-1!
path %hh%
cd C:\Windows\Microsoft.NET\Framework\v4.0.30319
RegAsm.exe /u cqpDemo.dll
pause
:exit