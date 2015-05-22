/*!
    @file
@verbatim
$Id: MKVStructures.h 58500 2009-02-18 19:45:46Z jbraness $

Copyright (c) 2008 DivX, Inc. All rights reserved.

This software is the confidential and proprietary information of
DivX, Inc. and may be used only in accordance with the terms of
your license from DivX, Inc.
@endverbatim
**/

#ifndef _MKVSTRUCTURES_H_
#define _MKVSTRUCTURES_H_

#include "DivXInt.h"
#include "DMFBlockType.h"
#include "MKVParserElements.h"
#include "EdtdDocumentSession.h"
#include "DF3/DF3Common/MKVTypes.h"
#include "./DF3/DF3Common/DMFOutputStream.h"

/* structure forward declarations */
typedef struct _MKVWC_Video_t_ MKVWC_Video_t;
typedef struct _MKVWC_ContentCompression_t_ MKVWC_ContentCompression_t;
typedef struct _MKVWC_ContentEncryption_t_ MKVWC_ContentEncryption_t;
typedef struct _MKVWC_Audio_t_ MKVWC_Audio_t;
typedef struct _MKVWC_Seek_t_ MKVWC_Seek_t;
typedef struct _MKVWC_H264PrivateData_t_ MKVWC_H264PrivateData_t;
typedef struct _MKVWC_LaceInfo_t_ MKVWC_LaceInfo_t;
typedef struct _MKVWC_ContentEncoding_t_ MKVWC_ContentEncoding_t;
typedef struct _MKVWC_EBMLHeader_t_ MKVWC_EBMLHeader_t;
typedef struct _MKVWC_BlockGroup_t_ MKVWC_BlockGroup_t;
typedef struct _MKVWC_Dirty_t_ MKVWC_Dirty_t;

/* function pointer type for object writers */
typedef DivXError (*ObjectWriter_fp)(void*, DMFOutputStreamHandle, void*, EdtdDocumentSessionHandle cacheDocSessionHandle, EdtdDocumentHandle cacheDocHandle, EdtdDocumentSessionHandle docSessionHandle, uint64_t*);
typedef DivXError (*IsDirty_fp)(MKVELEMENT_ID_e, uint32_t);
typedef DivXError (*SetDirty_fp)(MKVELEMENT_ID_e, uint32_t);
typedef DivXError (*MustWrite_fp)(MKVELEMENT_ID_e, uint32_t);


#define ELEMENT_BASE MKVELEMENT_ID_e myType; uint32_t numChildren; ObjectWriter_fp objectWriter; uint64_t sizeOffset; MKVWC_Dirty_t*  dirtyMap;


/*!
    used to store information/data for laced audio block
*/
struct _MKVWC_LaceInfo_t_
{
    void*       laceBuffer;     /*! ring buffer used to store blocks until lace boundry is met */
    void*       flushBuffer;    /*! holds completed laced block prior to writing to cache */
    uint32_t    frameSize;      /*! size of frame in a laced block */
    uint32_t    framesPerBlock; /*! number of frames composing a laced block */
    uint32_t    bufferSize;     /*! size of the ring buffer holding block data */
    MKV_FLAGS_e laceType;       /*! indicates type of lacing */
    int16_t     wFormatTag;     /*! format tag for audio -- needed to parse audio header */

    DivXBool    initialized;
    uint16_t    numBlocksRead;
    uint32_t    numBytesRead;
    uint64_t    absStartTime;  /* absolute start time in divxtime units */
    uint64_t    duration;      /* duration of 1 frame in divxtime units */

    uint8_t      blocksInBuffer; /* number of blocks in the ring buffer (used for Xiph)*/
    uint32_t*    blockSizes;     /* array of the sizes of each block in the ringbuffer (used for Xiph)*/
    uint32_t*    blockSizesRead; /* array of the sizes of each block read TODO could keep these in block sizes and use different pointer to read back */
};

/* used as dirty store in each element */
struct _MKVWC_Dirty_t_
{
    MKVELEMENT_ID_e elemId;
    DivXBool      dirty;
};

/* Video (Layer 4 element -- child of TrackEntry */
struct _MKVWC_Video_t_
{
    ELEMENT_BASE
    uint64_t    flagInterlaced;
    uint64_t    pixelWidth;
    uint64_t    pixelHeight;
    uint64_t    pixelCropBottom;
    uint64_t    pixelCropTop;
    uint64_t    pixelCropLeft;
    uint64_t    pixelCropRight;
    uint64_t    displayWidth;
    uint64_t    displayHeight;
    uint64_t    displayUnit;
};

/* Audio (Layer 4 element -- child of TrackEntry */
struct _MKVWC_Audio_t_
{
    ELEMENT_BASE
    float samplingFrequency;
    float outputSamplingFrequency;
    uint64_t    channels;
    uint64_t    bitDepth;
};

/*! Seek (Level 3 Elemet -- Child of SeekHead) */
struct _MKVWC_Seek_t_
{
    ELEMENT_BASE
    uint64_t seekID;        /* embl id of element at "seekPosition" */
    uint64_t seekPosition;  /* offset relative to segments data at which "seekID" id'd element exists */
};

struct _MKVWC_H264PrivateData_t_
{
    MKVVideoH264PrivateData_t*  privateData;
    uint32_t                    dataSize;
};

/*! ContentCompression (Level 6 Element - child of ContentEncoding) */
struct _MKVWC_ContentCompression_t_
{
    ELEMENT_BASE
    uint64_t contentCompAlgo;
    void*    contentCompSettings;
    uint32_t contentCompSettingsSize; // shadow to hold size of the contentCompSettings
};

/*! ContentEncoding (Level 5 element -- child of ContentEncodings) */
struct _MKVWC_ContentEncoding_t_
{
    ELEMENT_BASE
    uint64_t contentEncodingOrder;
    uint64_t contentEncodingScope;
    uint64_t contentEncodingType;
    MKVWC_ContentCompression_t* contentCompression;
    MKVWC_ContentEncryption_t* contentEncryption;
};


/*! ContentEncryption (Level 6 Element - child of ContentEncoding) */
struct _MKVWC_ContentEncryption_t_
{
    void* somthingHere; //TODO
};

/* ContentEncodings (Level 4 element -- child of TrackEntry) */
typedef struct _MKV_ContentEncodings_t_
{
    MKVWC_ContentEncoding_t* contentEncoding;
}MKVWC_ContentEncodings_t;

/*! TrackEntry (Level 3 Element -- Child of Tracks */

typedef struct _MKVWC_TrackEntry_t_
{
    ELEMENT_BASE
    uint64_t trackNumber;   /* id for the track must != 0 */
    uint64_t trackUID;      /* unique id for the track */
    uint64_t trackType;     /* video/audio/sub etc -- @TODO what are the defines for these */
    DivXBool    flagEnabled; /* == 1 when track is used */
    DivXBool    flagDefault; /* == 1 player should play track by default */
    DivXBool    flagForced;  /* == 1 player must play track */
    DivXBool    flagLacing; /* == 1 track may contain laced blocks */
    uint64_t    minCache;   /* number of frames that must be cached for playback */
    uint64_t    maxCache;   /* maximum cache size player will need to cache frames */
    uint64_t    defaultDuration; /* number of nanoseconds a frame lasts 0 => inconsistant duration */
    float       trackTimecodeScale; /* all timecodes are scaled by this value to obtain actual timecode of a block */
    uint8_t*    name;               /* name of the track */ //TODO UTF8
    int8_t* language;           /* language of the track */
    int8_t* codecID;            /* codec used to decode track */
    void*       codecPrivate;       /* information needed by codec */
    uint8_t* codecName;          /* name of the codec */ //TODO UTF8
    uint64_t    attachmentLink;     /* UID of the attachment used by this track */
    MKVWC_Video_t*    video;
    MKVWC_Audio_t*    audio;
    MKVWC_ContentEncodings_t*  contentEncodings;

    /* internal data used for keeping track of time */
    uint64_t rate;
    uint64_t scale;

    /* internal lacing info */
    MKVWC_LaceInfo_t*           laceInfo;
    MKVWC_H264PrivateData_t*    h264PrivateData;

}MKVWC_TrackEntry_t;

/*! TracksData */
typedef struct _MKVWC_TracksData_t_
{
    ELEMENT_BASE
    uint32_t    uiVersion;
    uint32_t    uiPayloadSize;
    uint8_t*    pPayload;
}MKVWC_TracksData_t;


/*! SimpleBlock TODO define this -- not in spec */
typedef struct _MKVWC_SimpleBlock_t_
{
    ELEMENT_BASE
    void* putSomthingHere; /* TODO */
}MKVWC_SimpleBlock_t;


/*! BlockGroup (Level 3 Element -- child of cluster) */

struct _MKVWC_BlockGroup_t_
{
    ELEMENT_BASE
    uint32_t size;                  /* internaly used to store size of void* data */
    void*   block;  /* data */
    int64_t refrenceBlock;  /* timecode offrame releative to block timecode of a frame that must be decoded before this frame can be decoded */
    int64_t blockDuration;  /* scaled duration of the block */
    void*   writeCache;
    uint8_t*     drmBuffer;
    uint32_t     drmBufferSize;
    DivXMem  hMem;
    DivXBool keyframe;      /* internal keyframe indicator */
    DMFBlockType_t blockType;
};


typedef struct _MKVWC_SegmentInfo_t_
{
    ELEMENT_BASE
    uint64_t    durationOffset;
    uint8_t     segmentUID[16];
    int8_t*     segmentFilename;    //utf
    uint8_t     prevUID[16];
    int8_t*     prevFilename;//utf
    uint8_t     nextUID[16];
    int8_t*     nextFilename;//utf
    uint64_t    timecodeScale;
    float      duration;
    int8_t*     title;
    int8_t*     muxingApp;
    int8_t*     writingApp;//utf
    int64_t     dateUTC;

    /* "private" reference to top level cache */
    void* writeCache;
} MKVWC_SegmentInfo_t;



/* SeekHead(Level 2 element) */
typedef struct _MKVWC_SeekHead_t_
{
    ELEMENT_BASE
    LinkedList_t*   seek; /* list of MKVWC_Seek_t elements */

    /* offsets for each level 3 (level 2 in mkv spec) element
       -- required because clusters are not cached */
    /* actually offsets into the seekhead where position is to be written*/
    uint64_t    segInfoOffset;
    uint64_t    seekHeadOffset;
    uint64_t    tracksOffset;
    uint64_t    clusterOffset;
    uint64_t    cueOffset;
    uint64_t    chapOffset;
    uint64_t    attachOffset;
    uint64_t    tagOffset;

    /* seek head requires a reference to top level cache because presence of seekhead elems
       indicated at top level cache */
    void* writeCache;                /* MKVWriteCacheInst_t* */
} MKVWC_SeekHead_t;

/* Tracks (Level 2 element) */
typedef struct _MKVWC_Tracks_t_
{
    ELEMENT_BASE
    LinkedList_t*         trackEntry;     /* list of tracks (MKVWC_TrackEntry_t*)*/
    LinkedList_t*         tracksData;     /* list of tracks (MKVWC_TrackEntry_t*)*/

    /* "private" reference to top level cache */
    void* writeCache;
} MKVWC_Tracks_t;


/* Cluster (Level 2 element) */
typedef struct _MKVWC_Cluster_t_
{
    ELEMENT_BASE
    uint64_t    timeCode;                   /* timecode all child blockgroup's timecodes are relative to */
    uint64_t    position;                   /* position of the cluster relative to segment */
    uint64_t    prevSize;                   /* size of the preceding cluster (bytes) (use to seek backwards) */
    LinkedList_t* blockGroup;               /* one blockgroup element 1-1 with cluster (MKVWC_BlockGroup_t*)*/
    LinkedList_t* simpleBlock;              /* block without overhead of blockGroup -- mutually exclusive with blockgroup(MKVWC_SimpleBlock_t*) */

    /* "private" reference to top level cache */
    void* writeCache;
    uint32_t curBlock;                      /* index of next block to be written */
    uint64_t curTimeOffset;                 /* next blocks time offset relative to cluster time (nanoseconds/timecodescale)*/
}MKVWC_Cluster_t;


struct _MKVWC_EBMLHeader_t_
{
    ELEMENT_BASE
    uint64_t    ebmlVersion;        /* version of EBML writer used to create file.          Default = 1 */
    uint64_t    ebmlReadVersion;    /* minimum version of parser required to read file.     Default = 1 */
    uint64_t    ebmlMaxIDLength;    /* longest EBML-ID contained in file.  Default = 4 */
    uint64_t    ebmlMaxSizeLength;  /* max "s_size" value in file.  Default = 8 */
    int8_t*     docType;            /* type of document.  Default = "matroska" */
    uint64_t    docTypeVersion;     /* verson of "doctype" writer used to create file.  Default = 1 */
    uint64_t    docTypeReadVersion; /* minimum version of "doctype" parser required to read file.Default = 1 */
};


/*! Segment (Level 1 element) 
*/
typedef struct _MKVWC_Segment_t_
{
    ELEMENT_BASE
    uint64_t        size;               /* current size of the segment */
    MKVWC_SegmentInfo_t*  segmentInfo;  /* info about this segment */
    LinkedList_t*         seekHead;     /* list of seek heads (MKVWC_SeekHead_t*)*/
    LinkedList_t*         cluster;      /* list of clusters (MKVWC_Cluster_t*)*/
    MKVWC_Tracks_t*       tracks;
    LinkedList_t*         cues;         /* list of cues (MKVWC_Cues_t*)*/
    LinkedList_t*         attachments;  /* list of attachment (MKVWC_Attachments_t*) */
    LinkedList_t*         chapters;     /* list of chapters (MKVWC_Chapters_t*)*/
    LinkedList_t*         tags;         /* list of tags (MKVWC_Tags_t*)*/
    void*                 writeCache; /* reference to top level cache */
}MKVWC_Segment_t;


#endif //_MKVSTRUCTURES_H_
