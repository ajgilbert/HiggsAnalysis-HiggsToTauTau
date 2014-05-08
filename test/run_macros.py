from optparse import OptionParser, OptionGroup
from HiggsAnalysis.HiggsToTauTau.LimitsConfig import configuration
import os

## set up the option parser
parser = OptionParser(usage="usage: %prog [options]",
                      description="Script to run all macros to create postfit plots. Macros have to be prodiced before hand and are expected to be located in the test directory.")
## direct options
parser.add_option("-a", "--analysis", dest="analysis", default="sm", type="choice", help="Type of analysis (sm or mssm). Lower case is required. [Default: sm]", choices=["sm", "mssm"])
parser.add_option("-p", "--periods", dest="periods", default="7TeV 8TeV", type="string", help="List of run periods for which the datacards are to be copied. [Default: \"7TeV 8TeV\"]")
parser.add_option("--add-mutau-soft", dest="add_mutau_soft", action="store_true", default=False, help="Add the soft categories to the mt channel [Default: False]")
parser.add_option("-c", "--config", dest="config", default="", type="string", help="Additional configuration file to be used for the channels, periods and categories. [Default: '']")
## check number of arguments; in case print usage
(options, args) = parser.parse_args()
if len(args) > 0 :
    parser.print_usage()
    exit(1)

#import configuration 
config=configuration(options.analysis, options.config, options.add_mutau_soft)

log = {
    'sm' :
    {    
    ("em", "0") : ["false",],
    ("em", "1") : ["false",],
    ("em", "2") : ["false",],
    ("em", "3") : ["false",],
    ("em", "4") : ["false",],
    ("em", "5") : ["false",],
    ("em", "6") : ["false",],
    ("em", "7") : ["false",],
    ("mt", "0") : ["false",], 
    ("mt", "1") : ["false",],
    ("mt", "2") : ["false",],
    ("mt", "3") : ["false",],
    ("mt", "4") : ["false",],
    ("mt", "5") : ["false",],
    ("mt", "6") : ["false",],
    ("mt", "7") : ["false",],
    ("mt", "10") : ["false"],
    ("mt", "11") : ["false"], 
    ("mt", "12") : ["false"],
    ("mt", "13") : ["false"],
    ("mt", "15") : ["false"],
    ("mt", "16") : ["false"],
    ("et", "0") : ["false" ],
    ("et", "1") : ["false" ],
    ("et", "2") : ["false" ],
    ("et", "3") : ["false" ],
    ("et", "4") : ["false" ],
    ("et", "5") : ["false",],
    ("et", "6") : ["false",],
    ("et", "7") : ["false",],
    ("tt", "0") : ["false",],
    ("tt", "1") : ["false",],
    ("tt", "2") : ["false",],
    ("mm", "0") : ["true"],
    ("mm", "1") : ["true"], 
    ("mm", "2") : ["true"],
    ("mm", "3") : ["true"],
    ("mm", "4") : ["true"],
    ("mm", "6") : ["false"],
    ("mm", "7") : ["false"],
    ("ee", "0") : ["true"],
    ("ee", "1") : ["true"], 
    ("ee", "2") : ["true"],
    ("ee", "3") : ["true"],
    ("ee", "4") : ["true"],
    },
    'mssm' :
    {   
    ("em", "8") : ["false", "true"],
    ("em", "9") : ["false", "true"],
    ("mt", "8") : ["false", "true"],
    ("mt", "9") : ["false", "true"],
    ("mt", "10") : ["false", "true"],
    ("mt", "11") : ["false", "true"], 
    ("mt", "12") : ["false", "true"],
    ("mt", "13") : ["false", "true"],
    ("mt", "14") : ["false", "true"],
    ("et", "8") : ["false", "true"],
    ("et", "9") : ["false", "true"],
    ("et", "10") : ["false", "true"],
    ("et", "11") : ["false", "true"], 
    ("et", "12") : ["false", "true"],
    ("et", "13") : ["false", "true"],
    ("et", "14") : ["false", "true"],
    ("tt", "8") : ["false", "true"],
    ("tt", "9") : ["false", "true"],
    ("tt", "10") : ["false", "true"],
    ("tt", "11") : ["false", "true"], 
    ("tt", "12") : ["false", "true"],
    ("tt", "13") : ["false", "true"],
    ("tt", "14") : ["false", "true"],
    ("mm", "8") : ["false", "true"],
    ("mm", "9") : ["false", "true"],
    }
    }

max = {
    'sm' :
    {    
    ("em", "0") :  ["-1."],
    ("em", "1") :  ["-1."],
    ("em", "2") :  ["-1."],
    ("em", "3") :  ["-1."],
    ("em", "4") :  ["-1."],
    ("em", "5") :  ["-1."],
    ("em", "6") :  ["-1."],
    ("em", "7") :  ["-1."],
    ("mt", "0") :  ["-1."],
    ("mt", "1") :  ["-1."],
    ("mt", "2") :  ["-1."],
    ("mt", "3") :  ["-1."],
    ("mt", "4") :  ["-1."],
    ("mt", "5") :  ["-1."],
    ("mt", "6") :  ["-1."],
    ("mt", "7") :  ["-1."],
    ("mt", "10") : ["-1."],
    ("mt", "11") : ["-1."], 
    ("mt", "12") : ["-1."],
    ("mt", "13") : ["-1."],
    ("mt", "15") : ["-1."],
    ("mt", "16") : ["-1."],
    ("et", "0") :  ["-1."],
    ("et", "1") :  ["-1."],
    ("et", "2") :  ["-1."],
    ("et", "3") :  ["-1."],
    ("et", "4") :  ["-1."],
    ("et", "5") :  ["-1."],
    ("et", "6") :  ["-1."],
    ("et", "7") :  ["-1."],
    ("tt", "0") :  ["-1."],
    ("tt", "1") :  ["-1."],
    ("tt", "2") :  ["-1."],
    ("mm", "0") :  ["5e9"],
    ("mm", "1") :  ["-1"],
    ("mm", "2") :  ["-1"],
    ("mm", "3") :  ["-1"],
    ("mm", "4") :  ["-1"],
    ("mm", "6") :  ["-1."],
    ("mm", "7") :  ["-1."],
    ("ee", "0") :  ["-1"],
    ("ee", "1") :  ["-1"],
    ("ee", "2") :  ["-1"],
    ("ee", "3") :  ["-1"],
    ("ee", "4") :  ["-1"],
    },
    
    'mssm' :   
    {    
    ("em", "8") :  ["-1.", "-1"],
    ("em", "9") :  ["-1.", "-1"],
    ("mt", "8") :  ["-1.", "-1"],
    ("mt", "9") :  ["-1.", "-1"],
    ("mt", "10") : ["-1.", "-1"],
    ("mt", "11") : ["-1.", "-1"], 
    ("mt", "12") : ["-1.", "-1"],
    ("mt", "13") : ["-1.", "-1"],
    ("mt", "14") : ["-1.", "-1"],
    ("et", "8") :  ["-1.", "-1"],
    ("et", "9") :  ["-1.", "-1"],
    ("et", "10") : ["-1.", "-1"],
    ("et", "11") : ["-1.", "-1"], 
    ("et", "12") : ["-1.", "-1"],
    ("et", "13") : ["-1.", "-1"],
    ("et", "14") : ["-1.", "-1"],
    ("tt", "8") :  ["-1.", "-1"],
    ("tt", "9") :  ["-1.", "-1"],
    ("tt", "10") : ["-1.", "-1"],
    ("tt", "11") : ["-1.", "-1"], 
    ("tt", "12") : ["-1.", "-1"],
    ("tt", "13") : ["-1.", "-1"],
    ("tt", "14") : ["-1.", "-1"],
    ("mm", "8") :  ["-1.", "-1"],
    ("mm", "9") :  ["-1.", "-1"],
    }
    }

min = {
    'sm' :   
    {
    ("em", "0") : ["0."],
    ("em", "1") : ["0."],
    ("em", "2") : ["0."],
    ("em", "3") : ["0."],
    ("em", "4") : ["0."],
    ("em", "5") : ["0."],
    ("em", "6") : ["0."],
    ("em", "7") : ["0."],
    ("mt", "0") : ["0."],
    ("mt", "1") : ["0."], 
    ("mt", "2") : ["0."],
    ("mt", "3") : ["0."],
    ("mt", "4") : ["0."],
    ("mt", "5") : ["0."],
    ("mt", "6") : ["0."],
    ("mt", "7") : ["0."],
    ("mt", "10") : ["0."],
    ("mt", "11") : ["0."], 
    ("mt", "12") : ["0."],
    ("mt", "13") : ["0."],
    ("mt", "15") : ["0."],
    ("mt", "16") : ["0."],
    ("et", "0") : ["0."],
    ("et", "1") : ["0."],
    ("et", "2") : ["0."],
    ("et", "3") : ["0."],
    ("et", "4") : ["0."],
    ("et", "5") : ["0."],
    ("et", "6") : ["0."],
    ("et", "7") : ["0."],
    ("tt", "0") : ["0."],
    ("tt", "1") : ["0."],
    ("tt", "2") : ["0."],
    ("mm", "0") : ["1e-1"],
    ("mm", "1") : ["1e-1"],
    ("mm", "2") : ["1e-1"],
    ("mm", "3") : ["1e-1"],
    ("mm", "4") : ["1e-2"],
    ("mm", "6") : ["0."],
    ("mm", "7") : ["0."],
    ("ee", "0") : ["1e-2"],
    ("ee", "1") : ["1e-1"],
    ("ee", "2") : ["1e-2"],
    ("ee", "3") : ["1e-1"],
    ("ee", "4") : ["1e-1"],
    }, 
    
    'mssm' :   
    {
    ("em", "8") : ["0.", "3e-3"],
    ("em", "9") : ["0.", "3e-3"],
    ("mt", "8") : ["0.", "1e-4"],
    ("mt", "9") : ["0.", "1e-4"],
    ("mt", "10") : ["0.", "3e-3"],
    ("mt", "11") : ["0.", "3e-3"], 
    ("mt", "12") : ["0.", "3e-3"],
    ("mt", "13") : ["0.", "3e-3"],
    ("mt", "14") : ["0.", "3e-3"],
    ("et", "8") : ["0.", "1e-4"],
    ("et", "9") : ["0.", "1e-4"],
    ("et", "10") : ["0.", "3e-3"],
    ("et", "11") : ["0.", "3e-3"], 
    ("et", "12") : ["0.", "3e-3"],
    ("et", "13") : ["0.", "3e-3"],
    ("et", "14") : ["0.", "3e-3"],
    ("tt", "8") : ["0.", "3e-3"],
    ("tt", "9") : ["0.", "1e-4"],
    ("tt", "10") : ["0.", "3e-3"],
    ("tt", "11") : ["0.", "3e-3"], 
    ("tt", "12") : ["0.", "3e-3"],
    ("tt", "13") : ["0.", "3e-3"],
    ("tt", "14") : ["0.", "3e-3"],
    ("mm", "8") : ["0.", "1e-3"],
    ("mm", "9") : ["0.", "1e-3"],
    }
    }

for chn in config.channels :
    for per in config.periods :
        for cat in config.categories[chn][per] :
            for sca in ["true", "false"] :
                for i in range(len(log[options.analysis][chn,cat])) :
                    if chn == "hbb" :
                        bash_script = "root -l -b -q {CHN}_{CAT}_{PER}.C+\(\"{SCA}\",\"{LOG}\",{MIN},{MAX}\)".format(
                            SCA=sca,
                            LOG=log[options.analysis][(chn,cat)][i],
                            MIN=min[options.analysis][(chn,cat)][i],
                            MAX=max[options.analysis][(chn,cat)][i],
                            CHN=chn,
                            CAT=cat,
                            PER=per
                            )
                    else :
                        bash_script = "root -l -b -q htt_{CHN}_{CAT}_{PER}.C+\(\"{SCA}\",\"{LOG}\",{MIN},{MAX}\)".format(
                            SCA=sca,
                            LOG=log[options.analysis][(chn,cat)][i],
                            MIN=min[options.analysis][(chn,cat)][i],
                            MAX=max[options.analysis][(chn,cat)][i],
                            CHN=chn,
                            CAT=cat,
                            PER=per
                            )
                    os.system(bash_script)
