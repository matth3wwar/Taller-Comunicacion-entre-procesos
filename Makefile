GCC = gcc
FLAGS =
MODULOS = taller_grupo.c
PROGRAMAS = taller_procesos

all: $(PROGRAMAS)

$(PROGRAMAS): $(MODULOS)
	$(GCC) $(MODULOS) -o $(PROGRAMAS)

clean:
	$(RM) -f $(PROGRAMAS)
