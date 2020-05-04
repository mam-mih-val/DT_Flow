#ifndef HADES_constants_H
#define HADES_constants_H 1
namespace HADES_constants
{
enum eTriggers
{ // all bits evaluate good
  kGoodVertexClust =
      0,            // 1: 1     good cluster vertex   (chi2>0, z > -65 (apr12), -160 (Jul14+aug14)) (requires 1 track)
  kGoodVertexCand,  // 2: 2     good candidate vertex (chi2>0, z > -65 (apr12), -160 (Jul14+aug14)) (requires at least 2
                    // reconstructed particles)
  kGoodSTART,       // 3: 4     good StartHit         (object exists (+ corrflag > -1 if HParticleStart2HitF was used)
  kNoPileUpSTART,   // 4: 8     no pileup in START (object exists + no second cluster)
  kNoPileUpMETA,    // 5: 16    no pileup in META  (no objects tof<0 or tof>60, threshold 0 (pionbeam)  5 apr12
  kNoPileUpMDC,     // 6: 32    no pileup in MDC   (objects t1>200 (inner or t1>400 (outer)), threshold 7 (pionbeam) 35
                    // (apr12))
  kNoFlashMDC,      // 7: 64    flash event in MDC
  kGoodMDCMult,     // 8: 128   good sectors inside normal mult
  kGoodMDCMIPSMult, // 9: 256   good sectors inside normal mult for MIPS
  kGoodLepMult,     // 10: 512   good sectors inside normal mult
  kGoodTRIGGER,     // 11: 1024  phys trigger (pt1 jul14/aug14, pt3 apr12)
  kGoodSTART2,      // 12: 2048  good StartHit         (object exists (+ corrflag ==2 if HParticleStart2HitF was used)
  kNoVETO,          // 13: 4096  no VETO hit +- 10ns arround START
  kGoodSTARTVETO,   // 14: 8192  no starthit >15ns <350ns uncorrelated with veto
  kGoodSTARTMETA,   // 15: 16384 no starthit >80ns <350ns  correlated with metahits
  kPT1,
  kPT2,
  kPT3,
  kNtriggers
};

enum eDetectors
{
  kMDC_in = 0,
  kMDC_out,
  kMDC_all,
  kMETA,
  kNdetectors
};

enum eTOFdetectors
{
  kRPC = 0,
  kTOF,
  kNTOFdetectors
};

enum eCentralityEstimators
{
  kNhitsTOF = 0,
  kNhitsTOF_cut,
  kNhitsRPC,
  kNhitsRPC_cut,
  kNhitsTOF_RPC,
  kNhitsTOF_RPC_cut,
  kNtracks,
  kNselectedTracks,
  kFWSumChargeSpec,
  kFWSumChargeZ,
  kNCentralityEstimators
};
}

#endif