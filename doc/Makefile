.SUFFIXES:	.tex .ps

# .tex file needs to be latex'd twice so that internal
# document references (page numbers, section numbers, etc.)
# can be resolved

.tex.ps:
	latex $*
	latex $*
	dvips $*

OUTPUT_FILES = api.ps sysman.ps userman.ps 3d-doc.ps

all: $(OUTPUT_FILES)

clean:
	-rm $(OUTPUT_FILES)
