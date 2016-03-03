@ECHO OFF
REM Runs the unit tests with a coverage report on the Windows debug build.
REM Note: you must have built the debug build before calling this.
REM

PUSHD Debug
opencppcoverage.exe --modules misclibLIB.Windows --modules misclibTest.exe --excluded_modules f:\dd -- misclibTest.exe
POPD
