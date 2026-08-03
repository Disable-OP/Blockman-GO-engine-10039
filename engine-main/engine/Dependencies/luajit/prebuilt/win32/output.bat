@echo off 
for /R %%s in (.,*) do ( 
echo %%s 
luajit.exe -b %%s %%s
) 
pause 