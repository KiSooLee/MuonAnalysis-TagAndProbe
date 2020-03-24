import FWCore.ParameterSet.Config as cms

import sys
args =sys.argv[1:]
if len(args) < 2: scenario = "0"
else: 
   scenario = args[1]
print("Will run scenario " + scenario) 
# scenario: 1 int bin, 2 pT, 3-4 pT in abseta bins, 5 abseta, 6 eta, 7 centrality, 0 (or no parameter) run all


process = cms.Process("TagProbe")
process.load('FWCore.MessageService.MessageLogger_cfi')
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )    
#PDFName = "cbFixedNGausExp"
PDFName = "cbFixedNGausPol3"


VEFFICIENCYSET =cms.VPSet(
# Order: 0 total, 1 pT, 2-5 pT fits in abseta bins, 6 abseta, 7 eta, 8-9 centrality   NOTE: IS NOT SAME AS SCENARIO
    cms.PSet(
        MuId_1bin = cms.PSet(
            EfficiencyCategoryAndState = cms.vstring("SoftID","true"),
            UnbinnedVariables = cms.vstring("mass"),
            BinnedVariables = cms.PSet(
                pt = cms.vdouble(0.8, 25.0),
                eta = cms.vdouble(-2.4, 2.4),
                tag_nVertices    = cms.vdouble(0.9,1.1),
                TM = cms.vstring("true"),
                #isNotMuonSeeded = cms.vstring("true"),
                #Glb = cms.vstring("true"),
                #PF = cms.vstring("true"),
            ),
         BinToPDFmap = cms.vstring(PDFName)
        )
    ),
    cms.PSet(
       MuId_pt = cms.PSet(
           EfficiencyCategoryAndState = cms.vstring("SoftID","true"),
           UnbinnedVariables = cms.vstring("mass"),
           BinnedVariables = cms.PSet(
               pt = cms.vdouble(0.8, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 8.0, 10.5, 14.0, 18.0, 25.0),
               eta = cms.vdouble(-2.4,2.4),
               tag_nVertices    = cms.vdouble(0.9,1.1),
               TM = cms.vstring("true"),
               #isNotMuonSeeded = cms.vstring("true"),
               #Glb = cms.vstring("true"),
               #PF = cms.vstring("true"),
           ),
           BinToPDFmap = cms.vstring(PDFName)
       )
    ),
    cms.PSet(         
         MuId_abseta00_10 = cms.PSet(
             EfficiencyCategoryAndState = cms.vstring("SoftID","true"),
             UnbinnedVariables = cms.vstring("mass"),
             BinnedVariables = cms.PSet(
                pt = cms.vdouble(3.3, 4.5, 5.5, 6.5, 8.0, 10.5, 14.0, 18.0, 25.0),
                abseta = cms.vdouble(0.0, 1.0),
                tag_nVertices    = cms.vdouble(0.9,1.1),
                TM = cms.vstring("true"),
                #isNotMuonSeeded = cms.vstring("true"),
                #Glb = cms.vstring("true"),
                #PF = cms.vstring("true"),
             ),
             BinToPDFmap = cms.vstring(PDFName)
        )
    ),
    cms.PSet(
        MuId_abseta10_15 = cms.PSet(
            EfficiencyCategoryAndState = cms.vstring("SoftID","true"),
            UnbinnedVariables = cms.vstring("mass"),
            BinnedVariables = cms.PSet(
                pt = cms.vdouble(1.2, 3.3, 4.5, 5.5, 6.5, 8.0, 10.5, 14.0, 18.0, 25.0),
                abseta = cms.vdouble(1.0, 1.5),
                tag_nVertices    = cms.vdouble(0.9,1.1),
                TM = cms.vstring("true"),
                #isNotMuonSeeded = cms.vstring("true"),
                #Glb = cms.vstring("true"),
                #PF = cms.vstring("true"),
            ),
            BinToPDFmap = cms.vstring(PDFName)
        )
    ), 
    cms.PSet(
         MuId_abseta15_20 = cms.PSet(
             EfficiencyCategoryAndState = cms.vstring("SoftID","true"),
             UnbinnedVariables = cms.vstring("mass"),
             BinnedVariables = cms.PSet(
                pt = cms.vdouble(0.8, 2.0, 3.5, 4.5, 6.5, 10.0, 14.0, 25.0),
                abseta = cms.vdouble(1.5,2.0),
                tag_nVertices    = cms.vdouble(0.9,1.1),
                TM = cms.vstring("true"),
                #isNotMuonSeeded = cms.vstring("true"),
                #Glb = cms.vstring("true"),
                #PF = cms.vstring("true"),
             ),
             BinToPDFmap = cms.vstring(PDFName)
        )
    ), 
    cms.PSet(         
         MuId_abseta20_24 = cms.PSet(
             EfficiencyCategoryAndState = cms.vstring("SoftID","true"),
             UnbinnedVariables = cms.vstring("mass"),
             BinnedVariables = cms.PSet(
                pt = cms.vdouble(0.8, 2.0, 3.5, 5.5, 12.0, 25.0),
                abseta = cms.vdouble(0.8,2.1),
                tag_nVertices    = cms.vdouble(0.9,1.1),
                TM = cms.vstring("true"),
                #isNotMuonSeeded = cms.vstring("true"),
                #Glb = cms.vstring("true"),
                #PF = cms.vstring("true"),
             ),
             BinToPDFmap = cms.vstring(PDFName)
        )
    ),  
 
    cms.PSet(
        MuId_absetadep = cms.PSet(
            EfficiencyCategoryAndState = cms.vstring("SoftID","true"),
            UnbinnedVariables = cms.vstring("mass"),
            BinnedVariables = cms.PSet(
                pt = cms.vdouble(0.8, 25.0),
                abseta = cms.vdouble(0,0.9,1.2,1.6,2.1,2.4),
                tag_nVertices    = cms.vdouble(0.9,1.1),
                TM = cms.vstring("true"),
                #isNotMuonSeeded = cms.vstring("true"),
                #Glb = cms.vstring("true"),
                #PF = cms.vstring("true"),
            ),
            BinToPDFmap = cms.vstring(PDFName)
        )
    ), 
    cms.PSet(      
        MuId_etadep = cms.PSet(
            EfficiencyCategoryAndState = cms.vstring("SoftID","true"),
            UnbinnedVariables = cms.vstring("mass"),
            BinnedVariables = cms.PSet(
                pt = cms.vdouble(0.8, 25.0),
                eta = cms.vdouble(-2.4,-2.1,-1.6,-1.2,-0.9,-0.6,-0.3,0,0.3,0.6,0.9,1.2,1.6,2.1,2.4),
                tag_nVertices    = cms.vdouble(0.9,1.1),
                TM = cms.vstring("true"),
                #isNotMuonSeeded = cms.vstring("true"),
                #Glb = cms.vstring("true"),
                #PF = cms.vstring("true"),
            ),
            BinToPDFmap = cms.vstring(PDFName)
        )
    ), 
    cms.PSet(
        MuId_centdep = cms.PSet(
            EfficiencyCategoryAndState = cms.vstring("SoftID","true"),
            UnbinnedVariables = cms.vstring("mass"),
            BinnedVariables = cms.PSet(
                pt = cms.vdouble(0.8, 25.0),
                eta = cms.vdouble(-2.4,2.4),
                tag_nVertices    = cms.vdouble(0.9,1.1),
                tag_hiNtracks = cms.vdouble(0, 35, 60, 90, 120, 155, 190, 400),
                TM = cms.vstring("true"),
                #isNotMuonSeeded = cms.vstring("true"),
                #Glb = cms.vstring("true"),
                #PF = cms.vstring("true"),
            ),
            BinToPDFmap = cms.vstring(PDFName)
        )
    ), 
    cms.PSet(
        MuId_centdepHF = cms.PSet(
            EfficiencyCategoryAndState = cms.vstring("SoftID","true"),
            UnbinnedVariables = cms.vstring("mass"),
            BinnedVariables = cms.PSet(
                pt = cms.vdouble(0.0 ,25.0),
                eta = cms.vdouble(-2.4,2.4),
                tag_nVertices    = cms.vdouble(0.9,1.1),
                tag_hiHF = cms.vdouble(0,30,50,75,100,125,150,175,400),
                TM = cms.vstring("true"),
                #isNotMuonSeeded = cms.vstring("true"),
                #Glb = cms.vstring("true"),
                #PF = cms.vstring("true"),
            ),
            BinToPDFmap = cms.vstring(PDFName)
        )
    ), 
    cms.PSet(
        MuId_nVerticesDep = cms.PSet(
            EfficiencyCategoryAndState = cms.vstring("SoftID","true"),
            UnbinnedVariables = cms.vstring("mass"),
            BinnedVariables = cms.PSet(
                pt = cms.vdouble(0.8, 25.0),
                eta = cms.vdouble(-2.4, 2.4),
                tag_nVertices    = cms.vdouble(0.5, 1.5, 2.5, 3.5, 5.5, 9.5),
                TM = cms.vstring("true"),
                #isNotMuonSeeded = cms.vstring("true"),
                #Glb = cms.vstring("true"),
                #PF = cms.vstring("true"),
            ),
         BinToPDFmap = cms.vstring(PDFName)
        )
    ),
)


#Actual selection 
if scenario == "1": EFFICIENCYSET = cms.PSet(VEFFICIENCYSET[0])
if scenario == "2": EFFICIENCYSET = cms.PSet(VEFFICIENCYSET[1])
if scenario == "3": EFFICIENCYSET = cms.PSet(VEFFICIENCYSET[2], VEFFICIENCYSET[3])
if scenario == "4": EFFICIENCYSET = cms.PSet(VEFFICIENCYSET[4], VEFFICIENCYSET[5])
if scenario == "5": EFFICIENCYSET = cms.PSet(VEFFICIENCYSET[6])
if scenario == "6": EFFICIENCYSET = cms.PSet(VEFFICIENCYSET[7])
if scenario == "7": EFFICIENCYSET = cms.PSet(VEFFICIENCYSET[8], VEFFICIENCYSET[9])
if scenario == "8": EFFICIENCYSET = cms.PSet(VEFFICIENCYSET[10])
if scenario == "0": EFFICIENCYSET = cms.PSet(VEFFICIENCYSET[0], VEFFICIENCYSET[1], VEFFICIENCYSET[2], VEFFICIENCYSET[3], VEFFICIENCYSET[4], VEFFICIENCYSET[5],VEFFICIENCYSET[6], VEFFICIENCYSET[7], VEFFICIENCYSET[8], VEFFICIENCYSET[9])#,VEFFICIENCYSET[12])





process.TagProbeFitTreeAnalyzer = cms.EDAnalyzer("TagProbeFitTreeAnalyzer",
    # IO parameters:
    InputFileNames = cms.vstring("file:/eos/cms/store/group/phys_heavyions/okukral/TagAndProbe2016/LowPt/tnpJPsi_Data_bothDir_v5_MuId.root"),
    InputDirectoryName = cms.string("tpTree"),
    InputTreeName = cms.string("fitter_tree"),
    OutputFileName = cms.string("tnp_Ana_RD_MuId_pPb_%s.root" % (scenario)),
    #number of CPUs to use for fitting
    NumCPU = cms.uint32(12),
    # specifies whether to save the RooWorkspace containing the data for each bin and
    # the pdf object with the initial and final state snapshots
    binnedFit = cms.bool(True),
    binsForFit = cms.uint32(50),
    binsForMassPlots = cms.uint32(50),
    SaveWorkspace = cms.bool(False),
    
    # defines all the real variables of the probes available in the input tree and intended for use in the efficiencies
    Variables = cms.PSet(
                         mass             = cms.vstring("Tag-Probe Mass", "2.7", "3.4", "GeV/c^{2}"), # mass range syst: 2.6-3.6
                         pt               = cms.vstring("Probe p_{T}", "0.0", "30", "GeV/c"),
                         eta              = cms.vstring("Probe #eta", "-2.4", "2.4", ""),
                         abseta           = cms.vstring("Probe |#eta|", "0", "2.4", ""),
                         tag_hiNtracks    = cms.vstring("N Tracks", "0", "400", ""),
                         #tag_hiHF         = cms.vstring("HF", "0", "500", ""),
                         tag_nVertices    = cms.vstring("PU - nVertices", "0", "10", ""),
                         #tag_pt           = cms.vstring("tag_pt", "0", "1000", "GeV/c"),
    ),
    # defines all the discrete variables of the probes available in the input tree and intended for use in the efficiency calculations
    Categories = cms.PSet(
                        #Glb = cms.vstring("Glb", "dummy[true=1,false=0]"),
                        #PF = cms.vstring("PF", "dummy[true=1,false=0]"),
                        SoftID = cms.vstring("SoftID", "dummy[true=1,false=0]"),
                        TM = cms.vstring("TM", "dummy[true=1,false=0]"),
                        #isNotMuonSeeded = cms.vstring("isNotMuonSeeded", "dummy[true=1,false=0]"),
                      
    ),

    # defines all the PDFs that will be available for the efficiency calculations; uses RooFit's "factory" syntax;
    # each pdf needs to define "signal", "backgroundPass", "backgroundFail" pdfs, "efficiency[0.9,0,1]" and "signalFractionInPassing[0.9]" are used for initial values  
    PDFs = cms.PSet(
	VoigtExp = cms.vstring(
		"Voigtian::signal(mass, mean[91,85,95], width[3,1,10], sigma[3,1,10])",
		"Exponential::backgroundPass(mass, lp[0,-5,5])",
		"Exponential::backgroundFail(mass, lf[0,-5,5])",
		"efficiency[0.9,0,1]",
		"signalFractionInPassing[0.9]"
	),
	BWResCBExp = cms.vstring(
		"BreitWigner::bw(mass, m0[91.2,81.2,101.2], width[2.495,1,10])",
		"RooCBShape::res(mass, peak[0], sigma[1.7,0.01,10], alpha[1.8,0,3], n[0.8,0,10])",
		"FCONV::signal(mass, bw, res)",
		"Exponential::backgroundPass(mass, lp[0,-5,5])",
		"Exponential::backgroundFail(mass, lf[0,-5,5])",
		"efficiency[0.9,0.5,1]",
		"signalFractionInPassing[0.9]",
	),
	BWResCBCheb = cms.vstring(
		"BreitWigner::bw(mass, m0[91.2,81.2,101.2], width[2.495,1,10])",
		"RooCBShape::res(mass, peak[0], sigma[1.7,0.01,10], alpha[1.8,0,3], n[0.8,0,10])",
		"FCONV::signal(mass, bw, res)",
		"Chebychev::backgroundPass(mass, {c1p[0,-10,10], c2p[0,-10,10], c3p[0,-10,10]})",
		"Chebychev::backgroundFail(mass, {c1f[0,-10,10], c2f[0,-10,10], c3f[0,-10,10]})",
		"efficiency[0.9,0.5,1]",
		"signalFractionInPassing[0.9]",
	),
        #nominal:
    cbPlusPol1 = cms.vstring(
        "CBShape::signal(mass, mean[3.08,3.00,3.3], sigma[0.03, 0.01, 0.10], alpha[1.85, 0.1, 50], n[1.7, 0.2, 50])",
        "Chebychev::backgroundPass(mass, {cPass[0.,-1.1,1.1]})",
        "Chebychev::backgroundFail(mass, {cFail[0.,-1.1,1.1]})",
        "efficiency[0.9,0,1]",
        "signalFractionInPassing[0.9]"
    ),
        #background syst:
    cbPlusPol2 = cms.vstring(
        "CBShape::signal(mass, mean[3.08,3.00,3.2], sigma[0.03, 0.01, 0.10], alpha[1.85, 0.1, 50], n[1.7, 0.2, 50])",
        "Chebychev::backgroundPass(mass, {cPass[0.,-1.1,1.1], cPass2[0.,-1.1,1.1]})",
        "Chebychev::backgroundFail(mass, {cFail[0.,-1.1,1.1], cFail2[0.,-1.1,1.1]})",
        "efficiency[0.9,0,1]",
        "signalFractionInPassing[0.9]"
    ),
        #signal syst:
    cbGausPlusPol1 = cms.vstring(
        "CBShape::signal1(mass, mean[3.08,3.00,3.3], sigma1[0.03, 0.01, 0.10], alpha[1.85, 0.1, 50], n[1.7, 0.2, 50])",
        "RooFormulaVar::sigma2('@0*@1',{fracS[1.8,1.2,2.4],sigma1})",
        "Gaussian::signal2(mass, mean, sigma2)",
        "SUM::signal(frac[0.8,0.5,1.]*signal1,signal2)",
        "Chebychev::backgroundPass(mass, {cPass[0.,-1.1,1.1]})",
        "Chebychev::backgroundFail(mass, {cFail[0.,-1.1,1.1]})",
        "efficiency[0.9,0,1]",
        "signalFractionInPassing[0.9]"
    ),
    cbGausExp = cms.vstring(
        "CBShape::signal1(mass, mean[3.08,3.00,3.3], sigma1[0.03, 0.01, 0.10], alpha[1.85, 0.1, 50], n[1.7, 0.2, 50])",
        "RooFormulaVar::sigma2('@0*@1',{fracS[1.8,1.2,2.4],sigma1})",
        "Gaussian::signal2(mass, mean, sigma2)",
        "SUM::signal(frac[0.8,0.5,1.]*signal1,signal2)",
        "Exponential::backgroundPass(mass, lp[0,-5,5])",
        "Exponential::backgroundFail(mass, lf[0,-5,5])",
        "efficiency[0.9,0,1]",
        "signalFractionInPassing[0.9]"
    ),
    cbFixedNGausExp = cms.vstring( #n fixed to average value in the MC abseta fits
        "CBShape::signal1(mass, mean[3.08,3.00,3.3], sigma1[0.03, 0.01, 0.10], alpha[1.85, 0.1, 50], n[1.3])",
        "RooFormulaVar::sigma2('@0*@1',{fracS[1.8,1.2,2.4],sigma1})",
        "Gaussian::signal2(mass, mean, sigma2)",
        "SUM::signal(frac[0.8,0.5,1.]*signal1,signal2)",
        "Exponential::backgroundPass(mass, lp[0,-5,5])",
        "Exponential::backgroundFail(mass, lf[0,-5,5])",
        "efficiency[0.9,0,1]",
        "signalFractionInPassing[0.9]"
    ),
    cbFixedNGausPol3 = cms.vstring( #n fixed to average value in the MC abseta fits
        "CBShape::signal1(mass, mean[3.08,3.00,3.3], sigma1[0.03, 0.01, 0.10], alpha[1.85, 0.1, 40], n[1.3])",
        "RooFormulaVar::sigma2('@0*@1',{fracS[1.8,1.2,2.4],sigma1})",
        "Gaussian::signal2(mass, mean, sigma2)",
        "SUM::signal(frac[0.8,0.5,1.]*signal1,signal2)",
        "Chebychev::backgroundPass(mass, {cPass[0.,-1.1,1.1], cPass2[0.,-0.3,0.3], cPass3[0.,-0.1,0.1]})",
        "Chebychev::backgroundFail(mass, {cFail[0.,-1.1,1.1], cFail2[0.,-0.3,0.3], cFail3[0.,-0.1,0.1]})",
        "efficiency[0.9,0,1]",
        "signalFractionInPassing[0.9]"
    ),
    cbFixedNGausExp3 = cms.vstring( #n fixed to average value in the MC abseta fits
        "CBShape::signal1(mass, mean[3.08,3.00,3.3], sigma1[0.03, 0.01, 0.10], alpha[1.85, 0.1, 50], n[1.3])",
        "RooFormulaVar::sigma2('@0*@1',{fracS[1.8,1.2,2.4],sigma1})",
        "Gaussian::signal2(mass, mean, sigma2)",
        "SUM::signal(frac[0.8,0.5,1.]*signal1,signal2)",
        "Exponential::backgroundPass(RooFormulaVar::mass3p('@1*@0 + @2*@0*@0 + @2*@0*@0*@0',{mass, lp1[0,-20,20], lp2[0,-20,20], lp3[0,-20,20]}), One[1.0])",
        "Exponential::backgroundFail(RooFormulaVar::mass3f('@1*@0 + @2*@0*@0 + @2*@0*@0*@0',{mass, lf1[0,-20,20], lf2[0,-20,20], lf3[0,-20,20]}), One[1.0])",
        "efficiency[0.9,0,1]",
        "signalFractionInPassing[0.9]"
    ),
    VoigtPol3 = cms.vstring( 
        "Voigtian::signal(mass, mean[3.08,3.00,3.3], width[0.03, 0.01, 0.20], sigma[0.03, 0.01, 0.20])",
        "Chebychev::backgroundPass(mass, {cPass[0.,-1.1,1.1], cPass2[0.,-1.1,1.1], cPass3[0.,-1.1,1.1]})",
        "Chebychev::backgroundFail(mass, {cFail[0.,-1.1,1.1], cFail2[0.,-1.1,1.1], cFail3[0.,-1.1,1.1]})",
        "efficiency[0.9,0,1]",
        "signalFractionInPassing[0.9]"
    ),

    ),

    # defines a set of efficiency calculations, what PDF to use for fitting and how to bin the data;
    # there will be a separate output directory for each calculation that includes a simultaneous fit, side band subtraction and counting. 
    Efficiencies = EFFICIENCYSET 
)

process.fitness = cms.Path(
    process.TagProbeFitTreeAnalyzer
)