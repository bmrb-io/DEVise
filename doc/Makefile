.SUFFIXES:	.tex .ps

.tex.ps:
	latex $*
	dvips $*

OUTPUT_FILES = api.tex sysman.ps userman.ps

all: $(OUTPUT_FILES)

clean:
	-rm $(OUTPUT_FILES)
