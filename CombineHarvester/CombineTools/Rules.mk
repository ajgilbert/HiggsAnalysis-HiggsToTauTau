SUBDIRS 	:=
LIB_DEPS 	:=
LIB_EXTRA := -lRooStats -lboost_python -L$(shell scramv1 tool tag python LIBDIR) -l$(shell scramv1 tool tag python LIB) -lPyROOT
PY_MODULES := combineharvester
PY_SRC_combineharvester := CombineHarvester_Python.pycc

$(d)/interface/GitVersion.h: $(TOP)/../.git/logs/HEAD
	@echo -e "Updating $@"
	@echo -e "namespace ch { inline std::string GitVersion() { return \""$(shell git describe --dirty)"\"; } }\n" > $@

clean_$(d)/interface/GitVersion.h :
	rm -f $(subst clean_,,$@)

clean_$(d) : clean_$(d)/interface/GitVersion.h

dir_$(d) : | $(d)/interface/GitVersion.h
tree_$(d) : | $(d)/interface/GitVersion.h
all_proxy :: | $(d)/interface/GitVersion.h
