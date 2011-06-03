@echo on
rem Copies required DLL files into output folder.

setlocal
set COPY=xcopy /D /S /C /I /H /R /Y 
set FILTER=find /v "File(s) copied"

set API_BIN=%1
set OUTDIR=%2
set CONFIG=%3

echo %CONFIG%

if %CONFIG%==Debug GOTO Debug
if %CONFIG%==Release dh

echo Unknown build configuration %CONFIG%
exit /b -1

:Debug
echo %FILTER%
echo %OUTDIR%
echo %API_BIN%

echo %COPY% %API_BIN%\*.* %OUTDIR% | %FILTER%
%COPY% %API_BIN%\*.* %OUTDIR% | %FILTER%
exit /b 0

:Release
%COPY% %API_BIN%\*.* %OUTDIR%\ | %FILTER%
exit /b 0
