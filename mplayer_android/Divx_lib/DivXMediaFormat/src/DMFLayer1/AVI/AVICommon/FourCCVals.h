/*!
    @file
@verbatim
$Id: FourCCVals.h 60224 2009-05-19 17:33:44Z nlu $

Copyright (c) 2007 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
*/

#ifndef _FOURCCVALS_H_
#define _FOURCCVALS_H_

#ifdef __cplusplus
extern "C" {
#endif

extern char cLIST[4];
extern char cDMNU[4];
extern char cJUNK[4];
extern char cMENU[4];
extern char cTRTB[4];
extern char cMDIA[4];
extern char cWMEN[4];
extern char cLGMN[4];
extern char cBMEN[4];

extern char cPLAY[4];
extern char cSTAC[4];
extern char cASAC[4];
extern char cPLAY0[4];
extern char cMNTX[4];
extern char cBNTX[4];
extern char cMNTK[4];

extern char cMEDS[4];
extern char cMEDT[4];

extern char cTITL[4];
extern char cCHAP[4];

extern char cMREC[4];

extern char cRIFF[4];

extern char cMRIF[4];

extern char cAVI[4];
extern char cAVIX[4];
extern char cMETA[4];

extern char cSTRL[4];

extern char cSTRF[4];

extern char cSTRN[4];

extern char cSTRH[4];

extern char cSTRD[4];

extern char cHDRL[4];

extern char cAVIH[4];

extern char cMOVI[4];

extern char cIDX1[4];

extern char cINDX[4];

extern char cIX00[4];

extern char c1IDX[4];

extern char cVIDS[4];

extern char cAUDS[4];

extern char cBLANK[4];

extern char cDMTA[4];
extern char cDMGR[4];
extern char cDMND[4];

extern char cDXRL[4];

extern char cINFO[4];
extern char cIDFV[4];
extern char cISFT[4];
extern char cIDPN[4];

extern char cODML[4];
extern char cDMLH[4];

extern char cDXSB[4];
extern char cDXSA[4];

extern char cREC[4];

extern char cH264[4];
extern char cAVC1[4];

#define FourCCToI32(a)  *((uint32_t *)(a))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _FOURCCVALS_H_ */
