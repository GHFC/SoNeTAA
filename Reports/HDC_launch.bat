@echo off
echo Starting...
cd c:\OFram\of\examples\addons\hdcxmlmicros\bin\data\
python experimentaldg.py
cd c:\OFram\of\examples\addons\hdcxmlmicros\bin\
hdcxmlmicros.exe
cd c:\OFram\of\examples\addons\hdcxmlmicros\bin\data\
python ExtractPhase.py
sh compile.sh