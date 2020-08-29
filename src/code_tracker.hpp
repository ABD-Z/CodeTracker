//
// Created by Abdulmajid NASSER on 23/08/2020.
//

#ifndef CODETRACKER_CODE_TRACKER_HPP
#define CODETRACKER_CODE_TRACKER_HPP

#include <cmath>
#include <iostream>
#include <functional>


namespace CodeTracker {
#define TWOPI 6.283185307
#define MASTER_VOLUME 1.f


    struct Key{Key(char n, char o); char note, octave;};
    namespace Notes {
        enum {
            C, C_S, D, D_S, E, F, F_S, G, G_S, A, A_S, B, PITCHES_PER_OCTAVE, OCTAVE_PITCH_OFFSET = 4, NOTE_PITCH_OFFSET = A
        };

        float pitch(float p);
        float key2freq(char note , char octave);
        float key2freq(Key key);
    }

    struct ADSR{
        ADSR(float A, float D, float S, float R);
        float attack, decay, sustain, release;};

    enum{
        SINUS, SQUARE, TRIANGLE, SAW, WHITENOISE, WAVETYPES
    };

    class Oscillator{
    public :
        explicit Oscillator(char wavetype);
        explicit Oscillator(char wavetype, float dc);
        explicit Oscillator(char wavetype, float dc, float p);
        virtual ~Oscillator();
        void setWavetype(char wavetype); char getWavetype();
        void setDutycycle(float dc); float getDutycycle();
        void setPhase(float p); float getPhase();
        virtual float oscillate(float a, float f, float t, float dc);
        virtual float oscillate(float a, float f, float t, float dc, float p);
        virtual ADSR* getAmpEnvelope() = 0;
    private:
        char wavetype = SINUS; float dutycycle = 0.5f; float phase = 0.0f;
        float sinus(float a, float f, float t, float dc, float FMfeed);
        float square(float a, float f, float t, float dc, float FMfeed);
        float triangle(float a, float f, float t, float dc, float FMfeed);
        float saw(float a, float f, float t, float dc, float FMfeed);
        float whitenoise(float a, float f, float t, float dc, float FMfeed);
        //function table
        std::vector<std::function<float(Oscillator&, float, float, float, float, float)>> wavefunc_table =
            {&Oscillator::sinus, &Oscillator::square, &Oscillator::triangle, &Oscillator::saw, &Oscillator::whitenoise};

        virtual float handleAmpEnvelope(float t) = 0;
        virtual void setRelease(bool r) = 0;

    };

    class PSG : public Oscillator{
    public:
        PSG(char wavetype);
        PSG(char wavetype, ADSR amp_enveloppe);
        PSG(char wavetype, float dc, ADSR amp_enveloppe);
        PSG(char wavetype, float dc, float p, ADSR amp_enveloppe);
        ~PSG() override;
        float oscillate(float a, float f, float t, float dc) override;
        float oscillate(float a, float f, float t, float dc, float p) override;
        ADSR* getAmpEnvelope() override;
    private:
        ADSR amp_envelope = ADSR(100.f, 0.0f, 1.0f, 1.0f);
        float handleAmpEnvelope(float t) override;
        void setRelease(bool r) override;
    protected:
        bool release = false;
    };

    class Instrument{
    public:
        explicit Instrument(Oscillator* osc);
        Instrument(Oscillator* osc, float global_volume);
        ~Instrument();
        Oscillator* get_oscillator();
        float play(float a, Key k,float t);
        float play(float a, char note, char octave, float t);
    private:
        float global_volume = 1.0f;
        Oscillator* osc;
    };

    struct Instruction{
        uint8_t instrument_index; Key key; float volume; uint32_t* effects;//efect tab
        Instruction(uint8_t instrument, Key k, float vol);
        Instruction(uint8_t instrument, char note, char octave, float vol);
        Instruction(uint8_t instrument, Key k, float vol, uint32_t* effects);
        Instruction(uint8_t instrument, char note, char octave, float vol, uint32_t *effects);
        ~Instruction();
    };

    struct Pattern{
        Instruction* instructions;
        ~Pattern();
    };

    class Channel;

    class Track{
    public:
        Track(float clk, float basetime, float speed, uint8_t rows, uint8_t frames, uint8_t channels,
              Instrument* instruments_bank, uint8_t numb_of_instruments, Pattern** track_patterns, uint8_t * pattern_indices);
        ~Track();
        float play(float t, Channel* chan);
    private:
        float clk , basetime, speed, step;
        uint8_t  rows, frames;
        uint8_t channels;
        float volume = 1.0f, pitch = 1.0f;
        Instrument* instruments_bank;
        uint8_t instruments;
        Pattern** track_patterns;
        uint8_t* pattern_indices;//new uint_8[channels*frames]
        float duration;
    };

    class Channel{
    public:
        explicit Channel(uint8_t number);
        [[nodiscard]] uint8_t getNumber() const;
        [[nodiscard]] bool isEnable() const; void enable(); void disable();
        [[nodiscard]] float getSpeed() const; void setSpeed(float speed);
        [[nodiscard]] float getPitch() const; void setPitch(float pitch);
        [[nodiscard]] float getVolume() const; void setVolume(float volume);
        [[nodiscard]] Instruction *getLastInstruction() const; void setLastInstruction(Instruction *lastInstruction);
        [[nodiscard]] Track *getTrack() const; void setTrack(Track *track);
        [[nodiscard]] float getTime() const; void setTime(float time);
    private:
        bool enable_sound = true;
        float volume = 1.0f, pitch = 1.0f, speed = 1.0f;
        Instruction* last_instruction = nullptr;
        Track* track = nullptr;
        float time = 0.0;
        uint8_t number;
    };


}

#endif //CODETRACKER_CODE_TRACKER_HPP
