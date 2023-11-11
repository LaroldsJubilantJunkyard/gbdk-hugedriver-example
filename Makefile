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
PROJECTNAME    = gbdk_hugedriver_music

BINS_BASIC	    = $(PROJECTNAME)_basic.gb
BINS_PAUSING	    = $(PROJECTNAME)_pausing.gb
BINS_SWITCHING	    = $(PROJECTNAME)_switching.gb
BINS_RESTARTING	    = $(PROJECTNAME)_restarting.gb
BINS_FADING	    = $(PROJECTNAME)_fading.gb
BINS_EFFECTS	    = $(PROJECTNAME)_effects.gb
BINS_BANKED	    = $(PROJECTNAME)_banked.gb

ASMSOURCES := $(wildcard *.s)

all:	$(BINS_BASIC) $(BINS_PAUSING) $(BINS_EFFECTS) $(BINS_RESTARTING) $(BINS_FADING) $(BINS_BANKED) $(BINS_SWITCHING)

clean:
	rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm


compile.bat: Makefile
	@echo "REM Automatically generated from Makefile" > compile.bat
	@make -sn | sed y/\\//\\\\/ | sed s/mkdir\ \-p/mkdir/ | grep -v make >> compile.bat

# We'll manually make our .o file for song_number3.o
# we need to add includes for the hugedriver includes and library
# use -Wf-bo2 to place it in bank 2
song_number3.o: 
	$(LCC) $(LCCFLAGS) -Ihugedriver/include -Wl-lhugedriver/gbdk/hUGEDriver.lib -Wf-bo2 -c -o song_number3.o song_number3.c


$(BINS_BASIC):	
	$(LCC) $(LCCFLAGS) -Ihugedriver/include -Wl-lhugedriver/gbdk/hUGEDriver.lib -o $@ main.basic.c song_number3.c
$(BINS_PAUSING):	
	$(LCC) $(LCCFLAGS) -Ihugedriver/include -Wl-lhugedriver/gbdk/hUGEDriver.lib -o $@ main.pausing.c song_number3.c
$(BINS_EFFECTS):	 
	$(LCC) $(LCCFLAGS) -Ihugedriver/include -Wl-lhugedriver/gbdk/hUGEDriver.lib -o $@ main.effects.c song_number3.c
$(BINS_SWITCHING): song_number3.o
	$(LCC) $(LCCFLAGS)  -autobank -Wm-yoA -Wm-yt3 -Ihugedriver/include -Wl-lhugedriver/gbdk/hUGEDriver.lib -o $@ main.switching.c sample_song.c song_number2.c  song_number3.o
$(BINS_RESTARTING):	 
	$(LCC) $(LCCFLAGS) -Ihugedriver/include -Wl-lhugedriver/gbdk/hUGEDriver.lib -o $@ main.restarting.c song_number3.c
$(BINS_FADING):	
	$(LCC) $(LCCFLAGS) -Ihugedriver/include -Wl-lhugedriver/gbdk/hUGEDriver.lib -o $@ main.fading.c song_number3.c
$(BINS_BANKED):	
	$(LCC) $(LCCFLAGS)  -Wm-yoA -Wm-yt3 -Ihugedriver/include -Wl-lhugedriver/gbdk/hUGEDriver.lib -o $@ main.banked.c song_number2.c


