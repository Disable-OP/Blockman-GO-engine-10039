@echo off

for /d %%a in (*) do if exist %%a\CMakeLists.txt (
  if exist %%a\Build  rd /q /s %%a\Build
)