########################################################################
#                 Makefile                                             #
#								                                       #
########################################################################

CXX        = g++
LD         = 

########################################################################

all: CreateData vg32sappe obj2sappe off2sappe scalepoints

CreateData: CreateData.cpp
	${CXX}  -o CreateData CreateData.cpp ${LDFLAGS} ${CXXFLAGS}

vg32sappe: vg32sappe.cpp
	${CXX}  -o vg32sappe vg32sappe.cpp ${LDFLAGS} ${CXXFLAGS}
	
obj2sappe: obj2sappe.cpp
	${CXX}  -o obj2sappe obj2sappe.cpp ${LDFLAGS} ${CXXFLAGS}
	
off2sappe: off2sappe.cpp
	${CXX}  -o off2sappe off2sappe.cpp ${LDFLAGS} ${CXXFLAGS}

scalepoints: scalepoints.cpp
	${CXX}  -o scalepoints scalepoints.cpp ${LDFLAGS} ${CXXFLAGS}


%.o: %.cpp
	${CXX}  -c ${@:.o=.cpp} ${CXXFLAGS}

%: %.o  
	${CXX}  -o $@ $@.o ${LDFLAGS}


clean:
	rm -f core core.* *.o *~ CreateData vg32sappe obj2sappe off2sappe scalepoints


########################################################################
