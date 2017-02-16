OBJ=main.o amptsub.o art1f.o \
hijing1.383_ampt.o hipyset1.35.o zpc.o linana.o
F77=gfortran

ampt: $(OBJ)
	$(F77) -o ampt -O $(OBJ)

main.o: main.f
	$(F77)  -c -O main.f

amptsub.o: amptsub.f
	$(F77)  -c -O amptsub.f

art1f.o: art1f.f
	$(F77)  -c -O art1f.f

hijing1.383_ampt.o: hijing1.383_ampt.f
	$(F77)  -c -O hijing1.383_ampt.f

hipyset1.35.o: hipyset1.35.f
	$(F77)  -c -O hipyset1.35.f

zpc.o: zpc.f
	$(F77)  -c -O zpc.f

linana.o: linana.f
	$(F77)  -c -O linana.f
