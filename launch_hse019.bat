@echo off
set PATH=C:\msys64\mingw64\bin;%PATH%
echo Launching HSE-019 Autonomous Room...
.\build\hse_workspace_app.exe --project .\experiments\hse019\room.hsc
pause
