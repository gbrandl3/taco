sbin_PROGRAMS = S_Alarm 

S_Alarm_SOURCES =  getlastmessage.cpp

S_Alarm_SOURCES += motifalarm.cpp
S_Alarm_CPPFLAGS = @MOTIF_INCLUDES@ @X_CFLAGS@
S_Alarm_LDADD = @MOTIF_LIBS@ @X_LIBS@ -lXt -lX11

# S_Alarm_SOURCES = qtalarm.cpp 
# S_Alarm_CPPFLAGS = -I$(QTDIR)/include
# S_Alarm_LDADD = -L$(QTDIR)/lib -lqt-mt

