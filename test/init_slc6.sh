#!/usr/sh
module use -a /afs/desy.de/group/cms/modulefiles/
module load cmssw/slc6_amd64_gcc481
#voms-proxy-init --voms cms:/cms/dcms
module load crab
cmsenv
#ini dctools
