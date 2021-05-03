#include <stdio.h>
#include <stdlib.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <opus/opusfile.h>

static ALuint
    bgm_source,
    bgm_intro_buffer,
    bgm_loop_buffer,
    bge_source,
    bge_buffer;

#define SFX_SOURCES 16
static ALuint sfx_sources[SFX_SOURCES], sfx_available;

struct track {
    ALenum format;
    ALvoid *data;
    ALsizei size;
};

static struct track bgm_tracklist[1];
static struct track bge_tracklist[1];
static struct track sfx_tracklist[1];

static const int loopstart[1] = { 0 };


void sound_init(void)
{
    ALCdevice *device = alcOpenDevice(NULL);
    ALCcontext *context = alcCreateContext(device, NULL);

    alcMakeContextCurrent(context);

    if (context == NULL) {
        fprintf(stderr, "sound.c:26: alcMakeContextCurrent failed\n");
        // todo: error handling
    }

    bgm_intro_buffer = AL_NONE;
    bgm_loop_buffer = AL_NONE;

    alGenSources(1, &bgm_source);
    alSource3f(bgm_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSourcef(bgm_source, AL_GAIN, 0.6f);


    bge_buffer = AL_NONE;

    alGenSources(1, &bge_source);
    alSource3f(bge_source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSourcef(bge_source, AL_GAIN, 0.4f);
    alSourcei(bge_source, AL_LOOPING, AL_TRUE);
    

    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);


    alGenSources(SFX_SOURCES, sfx_sources);
    for (int i = 0; i < SFX_SOURCES; i++) {
        alSource3f(sfx_sources[i], AL_POSITION, 0.0f, 0.0f, 0.0f);
        alSourcei(sfx_sources[i], AL_LOOPING, AL_FALSE);
    }
    sfx_available = 0;
}

void sound_proc(void)
{
    // bgm

    ALuint bproc, bqueu;
    alGetSourcei(bgm_source, AL_BUFFERS_PROCESSED, &bproc);
    alGetSourcei(bgm_source, AL_BUFFERS_QUEUED, &bqueu);
    
    if (bproc == 1) {
        printf("1 %d %d\n", bproc, bqueu);
        int t0;
        alSourceUnqueueBuffers(bgm_source, 1, &t0);
        printf("unq %d %d\n", bgm_intro_buffer, t0);
        alDeleteBuffers(1, &t0);
        alSourcei(bgm_source, AL_LOOPING, AL_TRUE);
    }

    if (bproc == 2) {
        printf("2 %d %d\n", bproc, bqueu);
        int t0;
        alSourceUnqueueBuffers(bgm_source, 1, &t0);
        printf("unq %d %d\n", bgm_loop_buffer, t0);
        alDeleteBuffers(1, &t0);
    }


    // sfx

    // deallocate source and buffer
    for (int i = 0; i < SFX_SOURCES; i++) {
        ALint state;
        alGetSourcei(sfx_sources[(sfx_available + i) % SFX_SOURCES], AL_SOURCE_STATE, &state);
        if (state == AL_STOPPED) {
            ALuint buf;
            alGetSourcei(sfx_sources[(sfx_available + i) % SFX_SOURCES], AL_BUFFER, &buf);
            alDeleteBuffers(1, &buf);
            if ((sfx_available + i) % SFX_SOURCES == 0)
                sfx_available = 0;
        } else {
            break;
        }
    }
}


static
void sound_load_track(int id, const char *file, struct track *tracklist)
{
    int error = 0;
    OggOpusFile *of = op_open_file(file, &error);

    if (error) {
        fprintf(stderr, "sound.c:50: op_open_file failed\n");
        // todo: error handling
    }

    int num_channels = op_channel_count(of, -1);
    int pcm_size = op_pcm_total(of, -1);

    ALenum format;
    if (num_channels == 1) {
        format = AL_FORMAT_MONO16;
    } else if (num_channels == 2) {
        format = AL_FORMAT_STEREO16;
    } else {
        fprintf(stderr, "sound.c:sound_load_track(): unhandled format\n");
        // todo: error handling
    }

    int16_t *buf = malloc(sizeof(int16_t) * pcm_size * num_channels);

    int samples_read = 0;
    while (samples_read < pcm_size) {
        int ns = op_read(of, buf + samples_read * num_channels,
            pcm_size * num_channels, 0);
        samples_read += ns;
    }

    op_free(of);

    tracklist[id].format = format;
    tracklist[id].data = buf;
    tracklist[id].size = samples_read * num_channels * 2;
}

void sound_load_bgm(int id, const char *file)
{
    sound_load_track(id, file, bgm_tracklist);
}

void sound_load_bge(int id, const char *file)
{
    sound_load_track(id, file, bge_tracklist);
}

void sound_load_sfx(int id, const char *file)
{
    sound_load_track(id, file, sfx_tracklist);
}


void sound_stop_bgm(void)
{
    int state;
    alGetSourcei(bgm_source, AL_SOURCE_STATE, &state);
    if (state != AL_STOPPED)
        alSourceStop(bgm_source);
}

void sound_play_bgm(int id)
{
    sound_stop_bgm();

    if (bgm_intro_buffer != AL_NONE)
        alDeleteBuffers(1, &bgm_intro_buffer);
    alGenBuffers(1, &bgm_intro_buffer);
    alBufferData(bgm_intro_buffer, bgm_tracklist[id].format,
        bgm_tracklist[id].data, // offset
        loopstart[id] * 2, // size
        48000);

    if (bgm_loop_buffer != AL_NONE)
        alDeleteBuffers(1, &bgm_loop_buffer);
    alGenBuffers(1, &bgm_loop_buffer);
    alBufferData(bgm_loop_buffer, bgm_tracklist[id].format,
        (int16_t *)(bgm_tracklist[id].data) + loopstart[id], // offset
        bgm_tracklist[id].size - loopstart[id] * 2, // size
        48000);

    alSourceQueueBuffers(bgm_source, 1, &bgm_intro_buffer);
    alSourceQueueBuffers(bgm_source, 1, &bgm_loop_buffer);

    alSourcei(bgm_source, AL_LOOPING, AL_FALSE);
    alSourcePlay(bgm_source);
}


void sound_stop_bge(void)
{
    int state;
    alGetSourcei(bge_source, AL_SOURCE_STATE, &state);
    if (state != AL_STOPPED)
        alSourceStop(bge_source);
}

void sound_play_bge(int id)
{
    //sound_stop_bge();

    if (bge_buffer != AL_NONE)
        alDeleteBuffers(1, &bge_buffer);
    alGenBuffers(1, &bge_buffer);
    alBufferData(bge_buffer, bge_tracklist[id].format,
        bge_tracklist[id].data, // offset
        bge_tracklist[id].size, // size
        48000);
    alSourcei(bge_source, AL_BUFFER, bge_buffer);
    alSourcePlay(bge_source);
}


void sound_play_sfx(int id)
{
    // if no sfx sources available then skip request
    if (sfx_available < SFX_SOURCES) {
        int buf;
        alGenBuffers(1, &buf);
        alBufferData(buf, sfx_tracklist[id].format, sfx_tracklist[id].data,
            sfx_tracklist[id].size, 48000);
        alSourcei(sfx_sources[sfx_available], AL_BUFFER, buf);
        alSourcePlay(sfx_sources[sfx_available]);
        sfx_available++;
    }
}


int sound_request_bgm_to_end(void)
{
    ALint looping;
    alGetSourcei(bgm_source, AL_LOOPING, &looping);
    if (looping)
        alSourcei(bgm_source, AL_LOOPING, AL_FALSE);

    ALint state;
    alGetSourcei(bgm_source, AL_SOURCE_STATE, &state);
    return state == AL_STOPPED;
}
