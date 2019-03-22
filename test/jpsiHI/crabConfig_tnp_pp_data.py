from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.section_('General')
config.General.requestName = 'TNP_SingleMu_pp5TeVRun2017G_PromptReco_v1_AOD_20181030'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = './jpsi/tp_from_aod_simple_Data.py'
config.JobType.maxMemoryMB = 2500
config.JobType.outputFiles = ['tnpJPsi_Data_pp5TeVRun2017G_AOD.root']

config.section_('Data')
config.Data.inputDataset = '/SingleMuon/Run2017G-17Nov2017-v1/AOD'
config.Data.inputDBS = 'global'
config.Data.unitsPerJob = 10
config.Data.splitting = 'LumiBased'
config.Data.runRange = '306546-306657'
config.Data.lumiMask = '/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions17/5TeV/PromptReco/Cert_306546-306657_5TeV_PromptReco_Collisions17_JSON_MuonPhys.txt'
config.Data.outLFNDirBase = '/store/user/%s/TagAndProbe2017/%s' % (getUsernameFromSiteDB(), config.General.requestName)#'/store/group/phys_heavyions/%s/TagAndProbe2017/%s' % (getUsernameFromSiteDB(), config.General.requestName)
config.Data.publication = False

config.section_('Site')
#config.Site.blacklist = ['T1_US_FNAL']
config.Site.storageSite = 'T2_FR_GRIF_LLR'#config.Site.storageSite = 'T2_CH_CERN'
