@echo off
where "C:\Program Files\CMake\bin\;C:\Program Files (x86)\CMake\bin\;%PATH%:cmake.exe" > tmp
set /P cmake=<tmp
del tmp
for /d %%a in (*) do if exist %%a\CMakeLists.txt (
  echo build %%a ...
  if not exist %%a\Build md %%a\Build
  cd %%a\Build
  "%cmake%" .. -A Win32
  cd ..\..
  echo .
)
pause
