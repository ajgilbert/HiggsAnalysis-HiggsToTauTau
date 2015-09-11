#!/usr/bin/env python

import argparse
import os
import re
import sys
import json
import math
import itertools
import stat
import HiggsAnalysis.HiggsToTauTau.combine.utils as utils

OPTS = {
  'vanilla' : '--minimizerStrategy 0 --minimizerTolerance 0.1 --cminOldRobustMinimize 0',
  'prefitAsimovSToy' : '-M GenerateOnly --expectSignal 1 -t -1 --saveToys --saveWorkspace --noMCbonly 1',
  'prefitAsimovBToy' : '-M GenerateOnly --expectSignal 0 -t -1 --saveToys --saveWorkspace --noMCbonly 1',
  'robust' :    '--robustFit 1 --minimizerTolerance 0.1 --minimizerAlgo Minuit2 --minimizerStrategy 0 --minimizerAlgoForMinos Minuit2 --minimizerStrategyForMinos 0 --cminPreScan --cminPreFit 1 --X-rtd FITTER_DYN_STEP --cminFallbackAlgo "Minuit2,0:0.1" --cminFallbackAlgo "Minuit2,Minimize,0:0.1" --cminOldRobustMinimize 0',
  'robustL' :    '--robustFit 1 --minimizerTolerance 0.1 --minimizerAlgo Minuit2 --minimizerStrategy 0 --minimizerAlgoForMinos Minuit2 --minimizerStrategyForMinos 0 --cminPreScan --cminPreFit 1 --X-rtd FITTER_DYN_STEP --cminFallbackAlgo "Minuit2,0:0.1" --cminFallbackAlgo "Minuit2,Minimize,0:0.1" --cminOldRobustMinimize 0 --minimizerToleranceForMinos 0.001',
  'robustLNoScan' :    '--robustFit 1 --minimizerTolerance 0.1 --minimizerAlgo Minuit2 --minimizerStrategy 0 --minimizerAlgoForMinos Minuit2 --minimizerStrategyForMinos 0 --cminPreFit 1 --X-rtd FITTER_DYN_STEP --cminFallbackAlgo "Minuit2,0:0.1" --cminFallbackAlgo "Minuit2,Minimize,0:0.1" --cminOldRobustMinimize 0 --minimizerToleranceForMinos 0.001',
  'robustNew' : '--robustFit 1 --minimizerTolerance 0.1 --minimizerAlgo Minuit2 --minimizerStrategy 0 --minimizerAlgoForMinos Minuit2 --minimizerStrategyForMinos 0 --cminPreScan --cminPreFit 1 --cminFallbackAlgo "Minuit2,0:0.1" --cminFallbackAlgo "Minuit2,Minimize,0:0.1" --cminOldRobustMinimize 0 --X-rtd FITTER_NEW_CROSSING_ALGO --X-rtd FITTER_NEVER_GIVE_UP --X-rtd FITTER_BOUND --minimizerToleranceForMinos 0.1',
  'MLHesse': '--minimizerTolerance 0.1 --minimizerAlgo Minuit2 --minimizerStrategy 0 --cminFallbackAlgo "Minuit2,0:0.1" --cminFallbackAlgo "Minuit2,Minimize,0:0.1" --cminOldRobustMinimize 0 --out ./ --minos none --skipBOnlyFit --noMCbonly 1 --cminPreScan'
}

DRY_RUN=False

JOB_PREFIX="""#!/bin/sh
ulimit -s unlimited
cd %(CMSSW_BASE)s/src
export SCRAM_ARCH=%(SCRAM_ARCH)s
eval `scramv1 runtime -sh`
cd %(PWD)s
""" % ({
  'CMSSW_BASE' : os.environ['CMSSW_BASE'],
  'SCRAM_ARCH' : os.environ['SCRAM_ARCH'],
  'PWD'        : os.environ['PWD']
 })


def run(command):
  print command
  if not DRY_RUN: return os.system(command)

class CombineBase:
  description = 'Just passes options through to combine'
  requires_root = False
  def __init__(self):
    pass
  def attach_intercept_args(self, group):
    pass
  def attach_args(self, group):
    group.add_argument('--opts', nargs='+', default=[], help='Add preset combine option groups')
    group.add_argument('--coalesce', '-c', type=int, default=1, help='comine this many jobs')
    group.add_argument('--split-points', type=int, default=0, help='If > 0 splits --algo grid jobs')
  def set_args(self, known, unknown):
    self.args = known
    self.passthru = unknown
    if hasattr(args, 'opts'):
      for opt in args.opts:
        self.passthru.append(OPTS[opt])
  def run(self, command, name=None):
    print command
    if self.args.gen_job:
      assert name is not None
      fname = 'job'+name+'.sh'
      with open(fname, "w") as text_file:
            text_file.write(JOB_PREFIX + 'eval ' + command)
      st = os.stat(fname)
      os.chmod(fname, st.st_mode | stat.S_IEXEC)
      #print JOB_PREFIX + command
    else:
      if not DRY_RUN: return os.system(command)
  def run_method(self):
    ## Put the method back in because we always take it out
    if hasattr(self.args, 'method'):
      self.passthru = ['-M', self.args.method] + self.passthru
      del self.args.method

    cmd_queue = []
    subbed_vars = {}

    if self.args.mass is not None:
      mass_vals = utils.split_vals(self.args.mass)
      subbed_vars['MASS'] = mass_vals

    if self.args.points is not None: self.passthru.extend(['--points', self.args.points])
    if (self.args.split_points is not None and
        self.args.split_points > 0 and
        self.args.points is not None):
      points = int(args.points)
      split = self.args.split_points
      start = 0
      ranges = [ ]
      while (start + (split-1)) <= points:
        ranges.append((start, start + (split-1)))
        start += split
      if start < points:
        ranges.append((start, points - 1))
      point_ranges = ['--firstPoint %i --lastPoint %i' % (r[0], r[1]) for r in ranges]
      subbed_vars['POINTS'] = point_ranges

    for i in itertools.product(*subbed_vars.values()): print i



class BasicCombine:
  description = 'Just passes options through to combine with special treatment for a few args'
  requires_root = False
  def __init__(self):
    pass
  def attach_intercept_args(self, group):
    group.add_argument('-m', '--mass')
    group.add_argument('--points')
    group.add_argument('--name', '-n', default='Test')
  def attach_args(self, group):
    group.add_argument('--opts', nargs='+', default=[], help='Add preset combine option groups')
    group.add_argument('--coalesce', '-c', type=int, default=1, help='comine this many jobs')
    group.add_argument('--split-points', type=int, default=0, help='If > 0 splits --algo grid jobs')
  def set_args(self, known, unknown):
    self.args = known
    self.passthru = unknown
    if hasattr(args, 'opts'):
      for opt in args.opts:
        self.passthru.append(OPTS[opt])
  def run(self, command, name=None):
    print command
    if self.args.gen_job:
      assert name is not None
      fname = 'job'+name+'.sh'
      with open(fname, "w") as text_file:
            text_file.write(JOB_PREFIX + 'eval ' + command)
      st = os.stat(fname)
      os.chmod(fname, st.st_mode | stat.S_IEXEC)
      #print JOB_PREFIX + command
    else:
      if not DRY_RUN: return os.system(command)
  def run_method(self):
    ## Put the method back in because we always take it out
    if hasattr(self.args, 'method'):
      self.passthru = ['-M', self.args.method] + self.passthru
      del self.args.method

    cmd_queue = []
    subbed_vars = {}

    if self.args.mass is not None:
      mass_vals = utils.split_vals(self.args.mass)
      subbed_vars['MASS'] = mass_vals
      self.passthru.extend(['-m', '%(MASS)s'])

    if self.args.points is not None: self.passthru.extend(['--points', self.args.points])
    if (self.args.split_points is not None and
        self.args.split_points > 0 and
        self.args.points is not None):
      points = int(args.points)
      split = self.args.split_points
      start = 0
      ranges = [ ]
      while (start + (split-1)) <= points:
        ranges.append((start, start + (split-1)))
        start += split
      if start < points:
        ranges.append((start, points - 1))
      point_ranges = ['--firstPoint %i --lastPoint %i' % (r[0], r[1]) for r in ranges]
      subbed_vars['POINTS'] = point_ranges
      self.passthru.extend(['%(POINTS)s', '-n', '.POINTS.%(POINTST)s.%(POINTEN)s'])


    proto = 'combine '+(' '.join(self.passthru))
    # print proto
    jobs = []
    for it in itertools.product(*subbed_vars.values()):
      keys = subbed_vars.keys()
      dict = {}
      for i, k in enumerate(keys): dict[k] = it[i]
      if 'POINTS' in dict:
        pointsplit = dict['POINTS'].split()
        dict['POINTST'] = pointsplit[1]
        dict['POINTEN'] = pointsplit[3]
      jobs.append(proto % dict)

    print jobs

    # if self.args.points is not None: self.passthru.extend(['--points', self.args.points])
    # taskqueue = []
    # namequeue = []
    # ## Now split the mass values
    # if self.args.mass is not None:
    #   mass_vals = utils.split_vals(self.args.mass)
    #   for m in mass_vals:
    #     taskqueue.append('combine %s -m %s' % (' '.join(self.passthru), m))
    #     namequeue.append(args.name)
    # else:
    #   taskqueue.append('combine %s' % (' '.join(self.passthru)))
    #   namequeue.append(args.name)

    # if (self.args.split_points is not None and
    #     self.args.split_points > 0 and
    #     self.args.points is not None):
    #   points = int(args.points)
    #   split = self.args.split_points
    #   start = 0
    #   ranges = [ ]
    #   while (start + (split-1)) <= points:
    #     ranges.append((start, start + (split-1)))
    #     start += split
    #   if start < points:
    #     ranges.append((start, points - 1))
    #   newqueue = [ ]
    #   newnamequeue = [ ]
    #   for name, task in itertools.izip(namequeue, taskqueue):
    #     for r in ranges:
    #       newqueue.append(task + ' --firstPoint %i --lastPoint %i' % (r[0], r[1]))
    #       newnamequeue.append(name + '_POINTS_%i_%i' % (r[0], r[1]))
    #   taskqueue = newqueue
    #   namequeue = newnamequeue
    # # add the name back
    # for name, task in itertools.izip(namequeue, taskqueue):
    #   task += ' -n %s' % name
    #   self.run(task, 'job'+name)


class SpecialCombine(BasicCombine):
  def __init__(self):
    BasicCombine.__init__(self)
  def attach_intercept_args(self, group):
    pass    
  def attach_args(self, group):
    BasicCombine.attach_args(self, group)
  def run_method(self):
    pass

class PrintWorkspace(SpecialCombine):
  description = 'Load a Workspace and call Print()'
  requires_root = True
  def __init__(self):
    SpecialCombine.__init__(self)
  def attach_args(self, group):
    group.add_argument('input', help='The input specified as FILE:WORKSPACE')
    ws_in = args.input.split(':')
    f  = ROOT.TFile(ws_in[0])
    ws = f.Get(ws_in[1])
    ws.Print()

class PrintSingles(SpecialCombine):
  description = 'Print the output of MultimDitFit --algo singles'
  requires_root = True
  def __init__(self):
    SpecialCombine.__init__(self)
  def attach_args(self, group):
    group.add_argument('input', help='The input file')
    group.add_argument('-P', '--POIs', help='The params that were scanned (in scan order)')
  def run_method(self):
    POIs = args.POIs.split(',')
    res = get_singles_results(args.input, POIs, POIs)
    for p in POIs:
      val = res[p][p]
      print '%s = %.3f -%.3f/+%.3f' % (p, val[1], val[1] - val[0], val[2] - val[1])


class RenameDataSet(SpecialCombine):
  description = 'Change the name of a dataset in an existing workspace' 
  requires_root = True
  def __init__(self):
    SpecialCombine.__init__(self)
  def attach_args(self, group):
    group.add_argument('input', help='The input specified as FILE:WORKSPACE:DATASET or FILE:WORKSPACE')
    group.add_argument('output', help='The output specified as FILE:WORKSPACE:DATASET or FILE:WORKSPACE')
    group.add_argument('-d','--data', help='Source data from other file, either FILE:WORKSPACE:DATA or FILE:DATA')
  def run_method(self):
    ws_in = args.input.split(':')
    print '>> Input:  ' + str(ws_in)
    ws_out = args.output.split(':')
    print '>> Output: ' + str(ws_out)
    f = ROOT.TFile(ws_in[0])
    ws = f.Get(ws_in[1])
    if len(ws_in) == 3:
      data = ws.data(ws_in[2])
    else:
      ws_d = args.data.split(':')
      print '>> Data: ' + str(ws_d)
      f_d = ROOT.TFile(ws_d[0])
      if len(ws_d) == 2:
        data = f_d.Get(ws_d[1])
      else:
        data = f_d.Get(ws_d[1]).data(ws_d[2])
      getattr(ws,'import')(data)
    ws.SetName(ws_out[1])
    if len(ws_out) == 3:
      data.SetName(ws_out[2])
    ws.writeToFile(ws_out[0])


class CovMatrix(SpecialCombine):
  description = 'Build a fit covariance matrix from scan results'
  requires_root = True
  def m_init__(self):
    SpecialCombine.__init__(self)
  def attach_args(self, group):
    group.add_argument('-i', '--input', nargs='+', default=[], help='The input file containing the MultiDimFit singles mode output')
    group.add_argument('-o', '--output', help='The output name in the format file:prefix')
    group.add_argument('-P', '--POIs', help='The params that were scanned (in scan order)')
    group.add_argument('--POIs-from-set', help='Extract from file:workspace:set instead')
    group.add_argument('--compare', help='Compare to RooFitResult')
  def run_method(self):
    POIs = [] 
    if args.POIs is not None:
      POIs = args.POIs.split(',')
    if args.POIs_from_set is not None:
      ws_in = args.POIs_from_set.split(':')
      print ws_in
      POIs = list_from_workspace(ws_in[0], ws_in[1], ws_in[2])
    res = { }
    if len(args.input) == 1:
      res.update(get_singles_results(args.input, POIs, POIs))
    elif len(args.input) > 1:
      assert len(args.input) == len(POIs)
      for i in range(len(POIs)):
        res.update(get_singles_results(args.input[i], [POIs[i]], POIs))
    for p in POIs:
      val = res[p][p]
      print '%s = %.3f -%.3f/+%.3f' % (p, val[1], val[1] - val[0], val[2] - val[1])
    print res
    cor = ROOT.TMatrixDSym(len(POIs))
    cov = ROOT.TMatrixDSym(len(POIs))
    for i,p in enumerate(POIs):
      cor[i][i] = ROOT.Double(1.) # diagonal correlation is 1 
      cov[i][i] = ROOT.Double(pow((res[p][p][2] - res[p][p][0])/2.,2.)) # symmetrized error
    for i,ip in enumerate(POIs):
      for j,jp in enumerate(POIs):
        if i == j: continue
        val_i = ((res[ip][jp][2] - res[ip][jp][0])/2.)/math.sqrt(cov[j][j])
        val_j = ((res[jp][ip][2] - res[jp][ip][0])/2.)/math.sqrt(cov[i][i])
        correlation = (val_i+val_j)/2. # take average correlation?
        #correlation = min(val_i,val_j, key=abs) # take the max?
        cor[i][j] = correlation
        cor[j][i] = correlation
        covariance = correlation * math.sqrt(cov[i][i]) * math.sqrt(cov[j][j])
        cov[i][j] = covariance
        cov[j][i] = covariance
    compare = args.compare is not None
    if compare:
      f_in = args.compare.split(':')
      f = ROOT.TFile(f_in[0])
      fitres = f.Get(f_in[1])
      fitres_cov = ROOT.TMatrixDSym(len(POIs))
      fitres_cov_src = fitres.covarianceMatrix()
      fitres_cor = ROOT.TMatrixDSym(len(POIs))
      fitres_cor_src = fitres.correlationMatrix()
      ipos = []
      for p in POIs:
        ipos.append(fitres.floatParsFinal().index(p))
      for i,ip in enumerate(POIs):
        for j,jp in enumerate(POIs):
          fitres_cor[i][j] = ROOT.Double(fitres_cor_src[ipos[i]][ipos[j]])
          fitres_cov[i][j] = ROOT.Double(fitres_cov_src[ipos[i]][ipos[j]])
    print 'My correlation matrix:'
    cor.Print()
    if compare:
      print 'RooFitResult correlation matrix:'
      fitres_cor.Print()
    print 'My covariance matrix:'
    cov.Print()
    if compare:
      print 'RooFitResult covariance matrix:'
      fitres_cov.Print()
    if args.output is not None:
      out = args.output.split(':')
      fout = ROOT.TFile(out[0], 'RECREATE')
      prefix = out[1]
      fout.WriteTObject(cor, prefix+'_cor')
      h_cor = self.fix_TH2(ROOT.TH2D(cor), POIs)
      fout.WriteTObject(h_cor, prefix+'_h_cor')
      fout.WriteTObject(cov, prefix+'_cov')
      h_cov = self.fix_TH2(ROOT.TH2D(cov), POIs)
      fout.WriteTObject(h_cov, prefix+'_h_cov')
      if compare:
        fout.WriteTObject(fitres_cor, prefix+'_comp_cor')
        h_cor_compare = self.fix_TH2(ROOT.TH2D(fitres_cor), POIs)
        fout.WriteTObject(h_cor_compare, prefix+'_comp_h_cor')
        fout.WriteTObject(fitres_cov, prefix+'_comp_cov')
        h_cov_compare = self.fix_TH2(ROOT.TH2D(fitres_cov), POIs)
        fout.WriteTObject(h_cov_compare, prefix+'_comp_h_cov')
  def fix_TH2(self, h, labels):
    h_fix = h.Clone()
    for y in range(1, h.GetNbinsY()+1):
      for x in range(1, h.GetNbinsX()+1):
        h_fix.SetBinContent(x, y, h.GetBinContent(x, h.GetNbinsY() + 1 - y))
    for x in range(1, h_fix.GetNbinsX()+1):
      h_fix.GetXaxis().SetBinLabel(x, labels[x-1])
    for y in range(1, h_fix.GetNbinsY()+1):
      h_fix.GetYaxis().SetBinLabel(y, labels[-y])
    return h_fix

      



class Impacts(SpecialCombine):
  description = 'Calculate nuisance parameter impacts' 
  requires_root = True
  def __init__(self):
    SpecialCombine.__init__(self)
  def attach_intercept_args(self, group):
    group.add_argument('-m', '--mass', required=True)
    group.add_argument('-d', '--datacard', required=True)
    group.add_argument('--redefineSignalPOIs')
    group.add_argument('--name', '-n', default='Test')
  def attach_args(self, group):
    SpecialCombine.attach_args(self, group)
    group.add_argument('--offset', default=0, type=int,
        help='Start the loop over parameters with this offset (default: %(default)s)')
    group.add_argument('--advance', default=1, type=int,
        help='Advance this many parameters each step in the loop (default: %(default)s')
    group.add_argument('--named', metavar='PARAM1,PARAM2,...',
        help=('By default the list of nuisance parameters will be loaded from the input workspace. '
              'Use this option to specify a different list'))
    group.add_argument('--doInitialFit', action='store_true',
        help=('Find the crossings of all the POIs. Must have the output from this before running with --doFits'))
    group.add_argument('--splitInitial', action='store_true',
        help=('In the initial fits generate separate jobs for each POI'))
    group.add_argument('--doFits', action='store_true',
        help=('Actually run the fits for the nuisance parameter impacts, otherwise just looks for the results'))
    group.add_argument('--output', '-o',
        help=('write output json to a file'))
  def run_method(self):
    offset      = self.args.offset
    advance     = self.args.advance
    passthru    = self.passthru 
    mh          = self.args.mass
    ws          = self.args.datacard
    name        = self.args.name if self.args.name is not None else ''
    named = []
    if args.named is not None:
      named = args.named.split(',')
    # Put intercepted args back
    passthru.extend(['-m', mh])
    passthru.extend(['-d', ws])
    pass_str = ' '.join(passthru)
    paramList = []
    if args.redefineSignalPOIs is not None:
      poiList = args.redefineSignalPOIs.split(',')
    else:
      poiList = list_from_workspace(ws, 'w', 'ModelConfig_POI')
    #print 'Have nuisance parameters: ' + str(paramList)
    print 'Have POIs: ' + str(poiList)
    poistr = ','.join(poiList)
    if args.doInitialFit:
      if args.splitInitial:
        for poi in poiList:
          self.run('combine -M MultiDimFit -n _initialFit_%(name)s_POI_%(poi)s --algo singles --redefineSignalPOIs %(poistr)s --floatOtherPOIs 1 --saveInactivePOI 1 -P %(poi)s %(pass_str)s --altCommit' % vars(), '_initialFit_%(name)s_POI_%(poi)s' % vars())
      else:
        self.run('combine -M MultiDimFit -n _initialFit_%(name)s --algo singles --redefineSignalPOIs %(poistr)s %(pass_str)s --altCommit' % vars(), '_initialFit_%(name)s')
      sys.exit(0)
    initialRes = get_singles_results('higgsCombine_initialFit_%(name)s.MultiDimFit.mH%(mh)s.root' % vars(), poiList, poiList)
    if len(named) > 0:
      paramList = named
    else:
      paramList = list_from_workspace(ws, 'w', 'ModelConfig_NuisParams')
    print 'Have nuisance parameters: ' + str(len(paramList))
    prefit = prefit_from_workspace(ws, 'w', paramList)
    res = { }
    res["POIs"] = []
    res["params"] = []
    for poi in poiList:
      res["POIs"].append({"name" : poi, "fit" : initialRes[poi][poi]})
    counter = offset
    missing = [ ]
    while counter < len(paramList):
      pres = { }
      param = paramList[counter]
      print 'Doing param ' + str(counter) + ': ' + param
      if args.doFits:
        self.run('combine -M MultiDimFit -n _paramFit_%(name)s_%(param)s --algo singles --redefineSignalPOIs %(param)s,%(poistr)s -P %(param)s --floatOtherPOIs 1 --saveInactivePOI 1 %(pass_str)s --altCommit' % vars(), '_paramFit_%(param)s' % vars())
      if not args.dry_run:
        paramScanRes = get_singles_results('higgsCombine_paramFit_%(name)s_%(param)s.MultiDimFit.mH%(mh)s.root' % vars(), [param], poiList + [param])
        if paramScanRes is None:
          missing.append((counter,param))
          counter = counter + advance
          continue
        pres.update({"name" : param, "fit" : paramScanRes[param][param], "prefit" : prefit[param]})
        for p in poiList:
          pres.update({p : paramScanRes[param][p], 'impact_'+p : (paramScanRes[param][p][2] - paramScanRes[param][p][0])/2.})
        res['params'].append(pres)
        counter = counter + advance
        
    
    jsondata = json.dumps(res, sort_keys=True, indent=2, separators=(',', ': '))
    print jsondata
    if args.output is not None:
      with open(args.output, 'w') as out_file:
        out_file.write(jsondata)
    if len(missing) > 0:
      print 'Missing inputs: ' + str(missing)

def register_method(parser, method_dict, method_class):
  class_name = method_class.__name__
  parser.description += '  %-20s : %s\n' % (class_name, method_class.description)
  method_dict[class_name] = method_class
  
parser = argparse.ArgumentParser(
    prog='combineTool.py',
    add_help=False,
    formatter_class=argparse.RawDescriptionHelpFormatter
    )

parser.description = 'Available methods:\n\n'
methods = {}
register_method(parser, methods, BasicCombine)
register_method(parser, methods, PrintWorkspace)
register_method(parser, methods, RenameDataSet)
register_method(parser, methods, Impacts)
register_method(parser, methods, CovMatrix)
register_method(parser, methods, PrintSingles)

parser.add_argument('-M', '--method')
parser.add_argument('--dry-run', action='store_true', help='Commands are echoed to the screen but not run')
parser.add_argument('--gen-job', action='store_true', help='Commands are echoed to the screen but not run')


(args, unknown) = parser.parse_known_args()

if args.method is None:
  parser.print_help()
  sys.exit(0)

DRY_RUN = args.dry_run

method = methods[args.method]() if args.method in methods else BasicCombine()

# Importing ROOT is quite slow, so only import if the chosen method requires it
if method.__class__.requires_root:
  #print 'Importing ROOT'
  import ROOT
  ROOT.PyConfig.IgnoreCommandLineOptions = True
  ROOT.gSystem.Load('libHiggsAnalysisCombinedLimit')

# One group of options that are specific to the chosen method 
tool_group = parser.add_argument_group('%s options' % method.__class__.__name__, 'options specific to this method')
# And another group for combine options that will be intercepted 
intercept_group = parser.add_argument_group('combine options', 'standard combine options that will be re-interpreted')

# Let the chosen method create the arguments in both groups
method.attach_intercept_args(intercept_group)
method.attach_args(tool_group)

# Now we can add the normal help option
parser.add_argument('-h', '--help', action='help')
 
(args, unknown) = parser.parse_known_args()

# Print these for debugging
#print args
#print unknown

method.set_args(args, unknown)
method.run_method()



