cd c:/OFram/of/examples/addons/hdcxmlmicros/bin/data
python compile_report.py
cd c:/OFram/of/examples/addons/hdcxmlmicros/bin/data/report_document
pdflatex main.tex
rm maint.pdf
pdflatex main.tex
cp main.pdf c:/Users/Guillaume\ Dumas/Desktop/report.pdf
rm maint.pdf