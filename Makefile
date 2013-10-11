all: MultithreadedQtOpenGL.pdf

MultithreadedQtOpenGL.pdf: MultithreadedQtOpenGL.tex
	pdflatex MultithreadedQtOpenGL.tex
	pdflatex MultithreadedQtOpenGL.tex

clean:
	rm -f *.aux *.fff *.log *.out *.pdf *.ttt
