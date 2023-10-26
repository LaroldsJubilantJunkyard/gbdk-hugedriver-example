#
# Simple Makefile that compiles all .c and .s files in the same folder
#

# If you move this project you can change the directory 
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
GBDK_HOME = C:/gbdk/

LCC = $(GBDK_HOME)bin/lcc 

# You can uncomment the line below to turn on debug output
# LCCFLAGS += -debug # Uncomment to enable debug output
# LCCFLAGS += -v     # Uncomment for lcc verbose output

# You can set the name of the .gb ROM file here
PROJECTNAME    = Example

BINS	    = $(PROJECTNAME).gb
CSOURCES   := main.c sample_song.c song_number2.c
ASMSOURCES := $(wildcard *.s)

all:	$(BINS)

compile.bat: Makefile
	@echo "REM Automatically generated from Makefile" > compile.bat
	@make -sn | sed y/\\//\\\\/ | sed s/mkdir\ \-p/mkdir/ | grep -v make >> compile.bat

# We'll manually make our .o file for song_number3.o
# we need to add includes for the hugedriver includes and library
# use -Wf-bo2 to place it in bank 2
song_number3.o: 
	$(LCC) $(LCCFLAGS) -Ihugedriver/include -Wl-lhugedriver/gbdk/hUGEDriver.lib -Wf-bo2 -c -o song_number3.o song_number3.c

# Compile and link all source files in a single call to LCC
# we'll specify 4 rom banks
# we need to add includes for the hugedriver includes and library
$(BINS):	$(CSOURCES) $(ASMSOURCES)  song_number3.o # We'll add our song_number3.o file instead of .c
	$(LCC) $(LCCFLAGS) -autobank -Wm-yoA -Wm-yt3 -Ihugedriver/include -Wl-lhugedriver/gbdk/hUGEDriver.lib -o $@ $(CSOURCES) song_number3.o

clean:
	rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm

