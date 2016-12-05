CXX=g++
NOWARN=-Wno-unused
CXXFLAGS=-Wall -Werror -g -std=c++14 -D_DEBUG $(NOWARN)
LDFLAGS=-lX11 -lGL

OBJDIR=build
SRCDIR=src

SRC=src/linux_relwarb.cpp \
	src/relwarb.cpp
OBJ=$(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o, $(SRC))
EXE=relwarb

all: $(EXE)

$(EXE): buildrepo $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -r $(OBJDIR)

mrproper: clean
	rm $(EXE)

buildrepo:
	@$(call make-repo)

define make-repo
	mkdir -p $(OBJDIR)
endef
