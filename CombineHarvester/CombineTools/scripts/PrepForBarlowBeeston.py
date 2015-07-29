#!/usr/bin/env python

import combineharvester as ch
import glob
import re


def StripBBB(syst):
  matches = re.match('.*bin_\d+', syst.name())
  # if matches: print syst.name()
  return bool(matches)


cmb = ch.CombineHarvester()


for card in glob.glob('output/htt/125.1/htt*.txt'):
  cmb.QuickParseDatacard(card, "$MASS/$ANALYSIS_$CHANNEL_$BINID_$ERA.txt")

for card in glob.glob('output/htt/125.1/vhtt*.txt'):
  cmb.QuickParseDatacard(card, "$MASS/$ANALYSIS_$BINID_$ERA.txt")

before = len(cmb.syst_name_set())

cmb.FilterSysts(lambda x : StripBBB(x))

after = len(cmb.syst_name_set())
print 'Removed: ' + str(before - after)
# for s in cmb.syst_name_set():
#   print s

writer_htt = ch.CardWriter('$TAG/$MASS/$ANALYSIS_$CHANNEL_$BINID_$ERA.txt',
                           '$TAG/common/$ANALYSIS_$CHANNEL.input_$ERA.root')
writer_htt.SetVerbosity(1)
writer_htt.WriteCards('output/bbb-test/no-bbb', cmb.cp().analysis(['htt']))

writer_vhtt = ch.CardWriter('$TAG/$MASS/$ANALYSIS_$BINID_$ERA.txt',
                            '$TAG/common/$ANALYSIS.input_$ERA.root')
writer_vhtt.SetVerbosity(1)
writer_vhtt.WriteCards('output/bbb-test/no-bbb', cmb.cp().analysis(['vhtt']))

bbb = ch.BinByBinFactory()
bbb.SetAddThreshold(0.0).SetMergeThreshold(1.0).SetFixNorm(True)
bbb.MergeAndAdd(cmb.cp().era(['7TeV']), cmb)
bbb.MergeAndAdd(cmb.cp().era(['8TeV']), cmb)

new_cmb = cmb.cp()
new_cmb.FilterSysts(lambda x : not StripBBB(x))
print 'New: ' + str(len(new_cmb.syst_name_set()))
# for s in new_cmb.syst_name_set():
#   print s

writer_htt = ch.CardWriter('$TAG/$MASS/$ANALYSIS_$CHANNEL_$BINID_$ERA.txt',
                           '$TAG/common/$ANALYSIS_$CHANNEL.input_$ERA.root')
writer_htt.SetVerbosity(1)
writer_htt.WriteCards('output/bbb-test/full-bbb', cmb.cp().analysis(['htt']))

writer_vhtt = ch.CardWriter('$TAG/$MASS/$ANALYSIS_$BINID_$ERA.txt',
                            '$TAG/common/$ANALYSIS.input_$ERA.root')
writer_vhtt.SetVerbosity(1)
writer_vhtt.WriteCards('output/bbb-test/full-bbb', cmb.cp().analysis(['vhtt']))