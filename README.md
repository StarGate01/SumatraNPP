# SumatraNPP
Embeds Sumatra PDF viewer into Notepad++ - ideal for working with LaTeX documents.

You'll need my patched version of SumatraPDF 32 bit (3 Lines Changed): https://github.com/StarGate01/sumatrapdf

##Features:
 - Embeds SumatraPDF as a dockable dialog into Notepad++
 - Open corresponding pdf file for tex file
 - Open arbitrary pdf file
 - Jump to the corresponding line in the other document by double-clicking 
    - Forward and inverse Search
    - Note: compile with -synctex=1
 - Low memory- and cpu usage
 - Note: This does not contain a compiler.
 - Note: As of now, only single-tex-file documents are supported. But I'm sure you could hack something fitting together.
 
##Example setup:
Note: This is just my setup. You can use many other configurations.
 - Install ```Notepad++``` to ```C:\Program Files (x68)\Notepad++\```
 - Install ```MiKTeX``` somewhere and ensure it can be found in ```%PATH%```
 - Install ```SumatraNPP x86``` to ```C:\Program Files (x86)\SumatraPDF\```
 - Replace ```SumatraPDF.exe``` with patched version ```(sumatrapdf\rel\SumatraPDF-no-MUPDF.exe)```
 - Add ```"C:\Program Files (x86)\SumatraPDF"``` to ```%PATH%``` variable
 - Install ```SumatraNPP``` and ```NPPExec Plugins``` in Notepad++
    - Copy ```SumatraNPP.dll``` to ```C:\Program Files (x86)\Notepad++\plugins\```
    - Use Plugin Manager to install ```NPPExec```
 - Install ```MinGW```, make sure to include ```make``` utility, ensure ```make``` is in ```%PATH%``` ```(C:\MinGW\msys\1.0\bin)```
 - Configure ```NPPExec```:
```
NPP_SAVE
cd $(CURRENT_DIRECTORY)
make build
NPP_MENUCOMMAND Plugins/SumatraNPP/Open corresponding PDF
NPP_MENUCOMMAND Plugins/SumatraNPP/Forward search
```
 - Write a makefile:
```
SCRFILE = filename.tex

.PHONY: build clean

build: $(subst .tex,.pdf,$(SCRFILE))

%.pdf: %.tex
	pdflatex.exe -synctex=1 -shell-escape $<
	
clean:
	rm $(subst .tex,.aux,$(SCRFILE))
	rm $(subst .tex,.log,$(SCRFILE))
	rm $(subst .tex,.out,$(SCRFILE))
	rm $(subst .tex,.pdf,$(SCRFILE))
	rm $(subst .tex,.synctex,$(SCRFILE))
```
 - Map ```NPPExec``` command into menu and/or to keyboard shortcut
 - Use Notepad++ plugin ```"Customize Toolbar"``` to put build command into toolbar 
