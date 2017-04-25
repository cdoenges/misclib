@ECHO ON
REM Run PC-Lint


SET LINTPATH=Y:\PCLINT

REM %LINTPATH%\LINT-NT.EXE -i%LINTPATH% co-gcc.lnt env-vim.lnt *.c -summary >_lint.txt
%LINTPATH%\LINT-NT.EXE -i%LINTPATH% co-msc110.lnt env-vim.lnt *.c -summary >_lint.txt
