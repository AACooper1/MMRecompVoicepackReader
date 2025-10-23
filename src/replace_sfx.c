#include "modding.h"
#include "global.h"
#include "recomputils.h"
#include "recompconfig.h"

#include "audio_api/all.h"

RECOMP_IMPORT("magemods_audio_api", uintptr_t AudioApi_GetResourceDevAddr(u32 resourceId, u32 arg1, u32 arg2));


RECOMP_CALLBACK("magemods_audio_api", AudioApi_Init) void test_replace_thing()
{
    AudioApiFileInfo fileInfo = {0};

    if (!AudioApi_AddAudioFileFromFs(&fileInfo, "samples", "WW_Link_Attack1.wav"))
    {
        recomp_printf("Could not add file :/\n");
        return;
    }
    else
    {        

        if (fileInfo.channelType == AUDIOAPI_CHANNEL_TYPE_DEFAULT) {
            fileInfo.channelType = fileInfo.trackCount & 1
            ? AUDIOAPI_CHANNEL_TYPE_MONO
            : AUDIOAPI_CHANNEL_TYPE_STEREO;
        }
        void* sampleAddr = (void*)AudioApi_GetResourceDevAddr(fileInfo.resourceId, 0, 0);

        AdpcmLoop sampleLoop = (AdpcmLoop){
            { fileInfo.loopStart, fileInfo.loopEnd, fileInfo.loopCount, fileInfo.sampleCount }, {}
        };

        Sample sample = (Sample){
            0, 
            fileInfo.codec, 
            MEDIUM_CART, 
            false, 
            false,
            fileInfo.sampleCount * 2,
            sampleAddr,
            &sampleLoop,
            NULL
        };

        TunedSample tunedSample = (TunedSample){
            &sample,
            fileInfo.sampleRate / 32000.0f
        };

        SoundEffect soundEffect = (SoundEffect){
            tunedSample
        };

        AudioApi_ReplaceSoundEffect(0, 28, &soundEffect);
    }
    

    s32 seqId = AudioApi_CreateStreamedBgm(NULL, "samples", "21TakeCare.mp3");
    AudioApi_ReplaceSequence(NA_BGM_FILE_SELECT, &gAudioCtx.sequenceTable->entries[seqId]);
    AudioApi_ReplaceSequenceFont(NA_BGM_FILE_SELECT, 0, AudioApi_GetSequenceFont(seqId, 0));
}