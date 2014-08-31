# Makefile for
# sample.exe

GCPPC=icc
GLINK=link386

#production
GCPPFLAGS=-c -W3 -Gm+ -O- -Q
GLCCFLAGS=-B"/PM:PM /NOD" -Q

#debug
#GCPPFLAGS=-c -W3 -Gm -O- -Ti -Q 
#GLCCFLAGS=-B"/PM:PM /DEBUG /NOD" -Q

all: sample 
sample: sample.exe

sample.exe: sample.obj sample.def sample.res
	$(GCPPC) $(GLCCFLAGS) sample.obj\
	mavcl.lib os2386.lib cppooc3i.lib cppom30i.lib \
	sample.def -Fesample.exe
	rc sample.res sample.exe

sample.obj: sample.cpp
	$(GCPPC) $(GCPPFLAGS) sample.cpp

sample.res: sample.rc
	rc -r sample.rc
