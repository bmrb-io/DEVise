.SUFFIXES:	.tex .ps

.tex.ps:
	latex $*
	dvips $*

OUTPUT_FILES = api.ps sysman.ps userman.ps

all: $(OUTPUT_FILES)

clean:
	-rm $(OUTPUT_FILES)
