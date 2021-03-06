//
// Created by Abdulmajid, Olivier NASSER on 23/08/2020.
//

/* C0deTracker - Copyright (c) Abdulmajid, Olivier NASSER
 * C0deTracker header file containing everything you need yo create your music :
 * Keys, ADSR, Oscillator classes, Instrument, Pattern, Track and Channel.
 * See the example.hpp for more details about how to use C0deTracker.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim
 * that you wrote the original software. If you use this software in a product,
 * an acknowledgment in the product documentation would be appreciated
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file code_tracker.hpp
 * @brief C0deTracker header. Include it in your project to use all functionalities.
 * @author Abdulmajid, Olivier NASSER
 * @version 0.1
 * @date 23/08/2020
 */

#ifndef CODETRACKER_C0DE_TRACKER_HPP
#define CODETRACKER_C0DE_TRACKER_HPP

#include <cmath>
#include <cstdio>
#include <cstdint>
#include <vector>


namespace C0deTracker {
#define TWOPI 6.283185307
#define MASTER_VOLUME 1.f


    struct Key;
    struct ADSR;
    class Oscillator;
    class PSG;
    class Instrument;
    struct Instruction;
    struct Pattern;
    class Track;
    class Channel;
    class Editor;



    /**
     * @brief This structure represents a piano key which is represented by its note (C, C#, D, D#, E, F, F#, G, G#, A, A#, B ; see Notes enumeration)
     * and its octave (from 0 to 8).
     * @see Notes
     */
    struct Key{
        /**
         * @brief Default constructor of Key. Create an empty key of value 255 (CONTINUE) for the note and the octave.
         */
        Key();
        /**
         * @brief Constructor used to provide the corresponding note and octave of the key instantly.
         * @param n note of the key. See enumeration in Notes namespace.
         * @param o octave of the key, from 0 to 8 (above 8 should not be hearable).
         */
        Key(float n, float o); float note, octave;
    };

    namespace Notes {
        /** @brief Notes enumeration in the American way.
         *  @details Values of semitones go from 0 (C, Do) to 11 (B, Si)\n
         *  C = Do = 0\n
         *  C_S (C#) = Do# = 1\n
         *  D = Ré = 2\n
         *  D_S (D#) = Ré# = 3\n
         *  E = Mi = 4\n
         *  F = Fa = 5\n
         *  F_S (F#) = Fa# = 6\n
         *  G = Sol = 7\n
         *  G_S (G#) = Sol# = 8\n
         *  A = La = 9\n
         *  A_S (A#) = La# = 10\n
         *  B = Si = 11\n
         *  RELEASE, with 244 as value, is used to trigger release state of the instrument.\n
         *  CONTINUE, with 255 as value, is used to fill empty values, such as empty instruments, empty volume and empty key.\n
         *  The other three values in the enumeration are used in function key2freq(uint8_t note, uint8_t octave). You don't have to use them.
         *  @note For each instrument bank, you can create in it (RELEASE -1) different instruments, or 243 maximum.
         */
        enum KeysUtilities{C, C_S, D, D_S, E, F, F_S, G, G_S, A, A_S, B, PITCHES_PER_OCTAVE, OCTAVE_PITCH_OFFSET = 4, NOTE_PITCH_OFFSET = A, RELEASE = 244, CONTINUE};

        /**
         * @param p pitch2freq
         * @return frequency of the pitch2freq in float
         * @note Pitch 0 corresponds to 440 Hz.
         */
        float pitch2freq(float p);

        /**
         * @brief converts a key in float
         * @param k Key (note)
         * @return pitch of the key in float
         */
        float key2pitch(Key k);

        /**
         * @brief converts a key (note and octave separatly) in float
         * @param note
         * @param octave
         * @return pitch in float
         */
        float key2pitch(float note, float octave);

        /**
         * @brief This function gives the frequency of a given note from a given octave
         * @param note
         * @see KeysUtilities
         * @param octave
         * @return frequency of the note and octave in float
         * @note This function calls pitch(float p). A4 (440 Hz) corresponds to pitch2freq 0.
         */
        float key2freq(float note , float octave);
        /**
         * @brief This function gives the frequency of a given key
         * @param key (see structure Key)
         * @return frequency of the key in float
         * @note This function calls key2freq(uint8_t note , uint8_t octave)
         */
        float key2freq(Key key);
    }

    /**
     * @brief ADSR structure contains attack, decay, sustain and release components (all in float) used to manipulates waveform's
     * envelope (mainly for amplitude).
     *
     * @see C0deTracker::Oscillator
     */
    struct ADSR{
        ADSR(float A, float D, float S, float R);
        float attack, decay, sustain, release;
    };

    /**
     * @brief This enumeration stores the primitive waveforms values. You should provide to your Oscillator one of these
     * values in order to select the corresponding waveform function
     * @see C0deTracker::Oscillator
     */
    enum Waveforms{SINUS, SQUARE, TRIANGLE, SAW, WHITENOISE, WHITENOISE2, WAVETYPES};

    /**
     * @brief Abstract class used to generate simple waveform such as SINUS, SQUARE, TRIANGLE, SAW and WHITENOISE over
     * time. Oscillator handles basic stuff : amplitude (a), frequency (f), phase (p), duty cycle (dc), and even frequency
     * modulation feed (FMfeed) for FM synth support.
     * @note This class should not be instantiated. PSG class is one of its specialization.
     * @see C0deTracker::PSG, C0deTracker::Waveforms
     */
    class Oscillator{
    public :
        /**
         * @brief Basic constructor. By default, duty cycle is equal to 0.5 and phase to 0
         * @param wavetype of the oscillator
         */
        explicit Oscillator(uint_fast8_t wavetype);
        /**
         * @brief Second constructor.
         * @param wavetype of the oscillator
         * @param dc duty cycle
         */
        explicit Oscillator(uint_fast8_t wavetype, float dc);
        /**
         * @brief Second constructor.
         * @param wavetype of the oscillator
         * @param dc duty cycle
         * @param p phase
         */
        explicit Oscillator(uint_fast8_t wavetype, float dc, float p);
        /**
         * @brief copy oscillator. Used for cloning instruments in channel
         * @return Oscillator allocated dynamically
         */
        virtual Oscillator* clone() = 0;
        virtual ~Oscillator();

        /**
         * @brief set the wavetype of the oscillator to generate the corresponding waveform
         * @param wavetype 0, 1, 2, 3, 4, 5 => SINUS, SQUARE, TRIANGLE, SAW, WHITENOISE, WHITENOISE2
         * @see Waveforms
         */
        void setWavetype(uint_fast8_t wavetype);

        /**
         * @brief return the value of the corresponding wavetype
         */
        uint_fast8_t getWavetype();

        /**
         * @brief set the duty cycle of the waveform
         * @param dc duty cycle of the waveform
         */
        void setDutycycle(float dc);
        /**
         *
         * @return get the duty cycle of the waveform
         */
        float getDutycycle();

        /**
         * @brief Set the phase of the waveform. The value set is multiplied by 1/frequency (percentage of waveform period)
         * @param p  phase of the waveform.
         */
        void setPhase(float p);
        /**
         *
         * @return the phase in float
         */
        float getPhase();
        /**
         * @brief Generates corresponding waveform selected.
         * @param a Amplitude
         * @param f Frequency
         * @param t Time
         * @param dc Duty cycle
         * @param p Phase
         * @return Signal amplitude at time t with the given duty cycle dc and phase p.
         */
        virtual float oscillate(float a, float f, double t, float dc, float p);
        /**
         * @brief Same as previous oscillate, but with release time to handle release envelope. This function is fully abstract, it is implemented in PSG.
         * @param a Amplitude
         * @param f Frequency
         * @param rt Release time
         * @param t Time
         * @param dc Duty cycle
         * @param p Phase
         * @return Signal amplitude at time t with the given duty cycle dc and phase p.
         */
        virtual float oscillate(float a, float f, double t, double rt, float dc, float p) = 0;
        /**
         * @brief Get pointer to structure holding ADSR values for envelope
         * @return pointer to ADSR struct
         * @see C0deTracker::ADSR
         */
        virtual ADSR* getAmpEnvelope() = 0;
        /**
         * @brief Set release state of the oscillator.
         * @param r boolean to set the release state
         */
        virtual void setRelease(bool r) = 0;
        /**
         * @brief Check if the oscillator is in release state or not
         * @return release member
         */
        virtual bool isReleased() = 0;
    private:
        uint_fast8_t wavetype = SINUS; float dutycycle = 0.5f; float phase = 0.0f;
        static float sinus(float a, float f, double t, float dc, float FMfeed);
        static float square(float a, float f, double t, float dc, float FMfeed);
        static float triangle(float a, float f, double t, float dc, float FMfeed);
        static float saw(float a, float f, double t, float dc, float FMfeed);
        static float whitenoise(float a, float f, double t, float dc, float FMfeed);
        static float whitenoise2(float a, float f, double t, float dc, float FMfeed);

        virtual float handleAmpEnvelope(double t, double rt) = 0;


    };

    /**
     * @brief PSG class inherit from Oscillator. This class is the specification of simple sound generator, Pulse Sound
     * Generator, used in old systems. FM operators are based on this class.
     *
     * @see Oscillator
     */
    class PSG : public Oscillator{
    public:
        explicit PSG(uint_fast8_t wavetype);
        PSG(uint_fast8_t wavetype, ADSR amp_enveloppe);
        PSG(uint_fast8_t wavetype, float dc, ADSR amp_enveloppe);
        PSG(uint_fast8_t wavetype, float dc, float p, ADSR amp_enveloppe);
        PSG * clone() override;
        ~PSG() override;
        float oscillate(float a, float f, double t, float dc, float p) override;
        float oscillate(float a, float f, double t, double rt, float dc, float p) override;
        ADSR* getAmpEnvelope() override;
        void setRelease(bool r) override;
        bool isReleased() override;
    private:
        ADSR amp_envelope = ADSR(100.f, 0.0f, 1.0f, 1.0f);
        float handleAmpEnvelope(double t, double rt) override;
    protected:
        bool release = false;
        float current_envelope_amplitude = 0.f; /**<Used to calculate envelope notably for release state*/
    };

    /**
     * @brief Instrument class is a wrapper for one Oscillator (PSG, or FM). You will basically create your instruments
     * in a bank (simple array) that you give to your track.
     *
     * @see Track
     */
    class Instrument{
    public:
        Instrument();
        /**
         * @brief Creates an instrument which is based on an Oscillator (PSG, FM...)
         * @param osc Oscillator
         */
        explicit Instrument(Oscillator* osc);
        /**
         * @brief Creates an instrument which is based on an Oscillator (PSG, FM...)
         * @param osc Oscillator
         * @param global_volume specifies instrument global volume
         */
        Instrument(Oscillator* osc, float global_volume);
        /**
         * @brief Destructor
         */
        ~Instrument();

        /**
         * @brief copy Instrument. Used in channel
         * @return Instrument allocated dynamically
         */
        Instrument* clone();

        /**
         * @brief Gets instrument core, which is the Oscillator
         * @return a pointer to Oscillator
         */
        Oscillator* get_oscillator() const;
        /**
         * @brief Plays sounds at t time with a given key and amplitude
         * @param a Amplitude
         * @param k Structure Key (note, octave)
         * @param t Time
         * @return The signal
         */
        float play_key(float a, Key k, double t);
        /**
         * @brief Plays sounds at t time with a given note and octave and amplitude
         * @param a Amplitude
         * @param note Note
         * @param octave Octave
         * @param t Time
         * @return The signal
         */
        float play(float a, float note, double octave, double t);
        /**
         * @brief Plays sounds when released at t time and rt release time with a given key and amplitude
         * @param a Amplitude
         * @param k Structure Key (note, octave)
         * @param t Time
         * @return The signal
         */
        float play_key(float a, Key k, double t, double rt);
        /**
         * @brief Plays sounds when released at t time and rt release time with a given note and octave and amplitude
         * @param a Amplitude
         * @param note Note
         * @param octave Octave
         * @param t Time
         * @param rt Release Time
         * @return The Signal
         */
        float play(float a, float note, float octave, double t, double rt);

        /**
         * @brief Plays sounds at t time with a given pitch and amplitude
         * @param a Amplitude
         * @param p Pitch
         * @param t Time
         * @return The Signal
         */
        float play_pitch(float a, float p, double t);

        /**
         * @brief Plays sounds when released at t and rt release time with a given pitch and amplitude
         * @param a Amplitude
         * @param p Pitch
         * @param t Time
         * @return The Signal
         */
        float play_pitch(float a, float p, double t, double rt);

    private:
        float global_volume = 1.0f;
        Oscillator* osc = nullptr;
    };

    /**
     * @brief Instruction structure represents the instruction you type to make your music (instrument index, volume, note,
     * effects). It should be written in Pattern structure.
     *
     * @see Pattern
     */
    struct Instruction{
        uint_fast8_t instrument_index{}; Key key; float volume{}; uint_fast32_t** effects{};//effect tab
        /**
         * @brief Default constructor to create empty instruction
         */
        Instruction();
        Instruction(uint_fast8_t instrument, Key k, float vol);
        Instruction(uint_fast8_t instrument, float note, float octave, float vol);
        Instruction(uint_fast8_t instrument, Key k, float vol, uint_fast32_t** effects);
        Instruction(uint_fast8_t instrument, float note, float octave, float vol, uint_fast32_t** effects);
        /**
         * @brief Destructor.
         */
        ~Instruction();
    };

    /**
     * @brief This structure contains the array of pointers of Instructions. Patterns are fed to the Track.
     * @see Instruction , Track
     */
    struct Pattern{
        Instruction** instructions; /**<Array of Instruction pointers*/
        uint_fast8_t rows; /**< Size of the row*/
        uint_fast8_t n_fx;
        /**
         * @brief Pattern initializer
         * @param rows size of the patterns
         * @param number_of_fx max fx supported in this pattern of a corresponding channel
         */
        Pattern(uint_fast8_t rows, uint_fast8_t number_of_fx);

        /**
         * @brief Delete instructions dynamic allocation
         */
        ~Pattern();
    };

    /**
     * @brief Main class containing all the data needed to run a music. It should works in parallel with Channel.
     *
     * @see Channel
     */
    class Track{
    public:
        /**
         *@brief Track constructor which recquires all the needed parameters below
         * @param clk Clock frequency in Hz (60 in NTSC, 50 in PAL)
         * @param basetime the base time of the track, it multiplies the speed
         * @param speed the speed of the track, the tempo = speed * basetime / clk
         * @param rows Size of a pattern. Number of instructions in each pattern
         * @param frames Number of patterns
         * @param channels Number of channel related to polyphony
         * @param instruments_bank Pointer to the array containing the pointers to instruments
         * @param numb_of_instruments Size of instruments_bank
         * @param track_patterns Pointer to the array containing the pointers to the patterns which contains the instructions of the music
         * @param pattern_indices Pointer to the array containing the pointers to the indices of the patterns (to avoid writing several time same pattern)
         */
        Track(float clk, float basetime, float speed, uint_fast8_t rows, uint_fast8_t frames, uint_fast8_t channels,
              Instrument** instruments_bank, uint_fast8_t numb_of_instruments, Pattern** track_patterns, uint_fast8_t** pattern_indices,
              const uint_fast8_t* effects_per_chan);
        /**
         * @brief free everything related to the track, patterns, patterns indices, instruments
         */
        ~Track();

        /**
         * @brief main function called at each time to calculate the corresponding sample of the track
         * @param double t time in second
         * @param Channel* chan pointers to the channels allocated dynamically by the user
         * @param uint_fast8_t size_of_chans number of channels created by the user, otherwise the size of the array chan
         * @return pointer to array of float for left and right speaker
         */
        float* play(double t, Channel* chan, uint_fast8_t size_of_chans);

        /**
         * @return global panning if the track
         * @brief 0.5 is centered ; 0 sound is only on left ; 1 only on right
         */
        float getPanning();

        /**
         * @return value of the clock
         */
        float getClock();

        /**
         * @return the speed of the track which could be modified by effect 0x09xxxyyy
         */
        float getSpeed();

        /**
         * @return number of channels dedicated fo the track
         */
        uint_fast8_t getNumberofChannels();
        /**
         * @return duration of the track.
         * @note It is very approximative, especially when the track jumps frames and changes speed during the song processing
         */
        float getDuration();

    private:
        float clk , basetime, speed, step;
        uint_fast8_t  rows, frames;
        uint_fast8_t channels;
        float volume = 1.0f, pitch = 0.0f;
        Instrument** instruments_bank;
        uint_fast8_t instruments;
        Pattern** track_patterns;
        uint_fast8_t** pattern_indices;//new uint_8[channels*frames]
        float duration;
        const uint_fast8_t *fx_per_chan;

        uint_fast8_t row_counter = 0, frame_counter = 0;
        double time_advance = 0.0;
        double time = 0.0;

        bool decode_fx(uint_fast32_t fx, double t);
        bool readFx = true;
        float volume_slide_up = 0.f;
        float volume_slide_down = 0.f;
        double volume_slide_time = 0.0;

        float pitch_slide_up = 0.f;
        float pitch_slide_down = 0.f;
        double pitch_slide_time = 0.0;

        float tremolo_speed = 0.0f;
        float tremolo_depth = 0.0f;
        float tremolo_val = 1.0f;
        double tremolo_time = 0.0;

        float vibrato_speed = 0.0f;
        float vibrato_depth = 0.0f;
        float vibrato_val = 0.0f;
        double vibrato_time = 0.0;
        float panning = 0.5f;
        bool branch = false;
        uint_fast8_t frametojump = 0;
        uint_fast8_t rowtojump = 0;

        bool stop = false;

        float panning_slide_right = 0.f;
        float panning_slide_left = 0.f;
        double panning_slide_time = 0.0;
        void update_fx(double t);
    };

    /**
     * @brief This class "processes" sound. It is independent from the track. Each track pattern has its dedicated channel.
     * A channel can be interrupted to generate sound not from the track.
     *
     * @see Track
     */
    class Channel{
    public:
        /**
         * @brief create a channel. Each channel created has it is own number
         */
        Channel();
        /**
         * @brief create a channel with a specified number
         * @param number the number of the channel
         */
        explicit Channel(uint_fast8_t number);

        ~Channel();

        /**
         * @return number of the channel
         */
        uint_fast8_t getNumber() const;
        /**
         * @return if the channel is enabled to play_single_channel sound
         */
        bool isEnable() const;
        /**
         * @brief enable channel sound processing
         */
        void enable();
        /**
         * @brief disable channel sound processing
         */
        void disable();

        /**
         * @return the pitch of the channel
         */
        float getPitch() const;

        /**
         * @brief set the pitch of the channel
         * @param pitch to set
         */
        void setPitch(float pitch);

        /**
         * @return get the volume of the channel
         */
        float getVolume() const;

        /**
         * @brief set the volume of the channel
         * @param volume to set
         */
        void setVolume(float volume);

        /**
         * @return get the pointer to the last non empty instruction (from the track)
         */
        Instruction *getLastInstructionAddress() const;
        /**
         * @brief store the pointer to the last non empty instruction
         * @param lastInstruction pointer to the instruction in pattern
         */
        void setLastInstructionAddress(Instruction *lastInstructionAddress);

        /**
         *
         * @return address of the track currently working with the channel
         */
        Track *getTrack() const;
        /**
         * @brief store the address of the track
         * @param track currently used to generates sound
         */
        void setTrack(Track *track);

        /**
         *
         * @return time when the channel encounter a new
         */
        double getTime() const;

        /**
         * @brief set time when channel encounter a new note
         * @param time
         */
        void setTime(double time);

        /**
         *
         * @return time when release is triggered
         */
        double getTimeRelease() const;

        /**
         * @brief set the time when release is triggered
         * @param time
         */
        void setTimeRelease(double time);
        /**
         *
         * @return a boolean to know if the channel is released or not.
         */
        bool isReleased() const;

        /**
         * @brief set release state to boolean r
         * @param r a boolean (true release)
         */
        void setRelease(bool r);

        /**
         * @return the instruction state copied of the channel (which could be modified without dynamically in exe)
         */
        const Instruction *getInstructionState() const;

        /**
         * @brief copy the current instruction in Channel
         * @param instruc pointer to Instruction from patterns song
         */
        void setInstructionState(Instruction* instruc);

        /**
         * @brief modify the volume of the copied instruction in Channel
         * @param a volume in float
         */
        void setVolumeInstructionState(float a);

        friend float* Track::play(double t, C0deTracker::Channel *chan, uint_fast8_t size_of_chans);//function play of Track friend of Channel in order to avoid creating a huge amount of getters for each attributes
    private:
        static uint_fast8_t chancount;
        Instruction* last_instruct_address = nullptr;
        Track* track = nullptr;

        /**Channel state**/
        uint_fast8_t number;
        double time = 0.0;
        bool enable_sound = true;
        float volume = 1.0f, pitch = 0.0f, speed = 1.0f;
        bool released = false;
        double time_release = 0.0;
        Instruction instruct_state{};
        Instrument* instrument = nullptr;

        bool decode_fx(uint_fast32_t fx, double t);

        float volume_slide_up = 0.f;
        float volume_slide_down = 0.f;
        double volume_slide_time = 0.0;

        float pitch_slide_up = 0.f;
        float pitch_slide_down = 0.f;
        double pitch_slide_time = 0.0;
        double pitch_slide_val =0.0;

        bool portamento = false;
        float portamento_speed = 0.f;
        float portamento_val = 0.f;
        float porta_pitch_dif = 0.0f;
        double portamento_time_step = 0;

        float tremolo_speed = 0.0f;
        float tremolo_depth = 0.0f;
        float tremolo_val = 1.0f;
        double tremolo_time = 0.0;

        float vibrato_speed = 0.0f;
        float vibrato_depth = 0.0f;
        float vibrato_val = 0.0f;
        double vibrato_time = 0.0;

        float panning = 0.5f;

        float panning_slide_right = 0.f;
        float panning_slide_left = 0.f;
        double panning_slide_time = 0.0;

        bool arpeggio = false;
        double arpeggio_step = 0.0;
        uint_fast8_t  arpeggio_index = 0;
        uint_fast8_t arpeggio_val[6]{};

        void update_fx(double t);

        uint_fast8_t transpose_delay = 0;
        uint_fast8_t n_time_to_transpose = 0;
        uint_fast8_t transpose_semitones = 0;
        uint_fast8_t transpose_semitone_counter = 0;
        double transpose_time_step = 0;

        uint_fast8_t retrieg_delay = 0;
        uint_fast8_t retrieg_number = 0;
        uint_fast8_t n_time_to_retrieg = 0;
        double retrieg_time_step = 0;
        uint_fast8_t retrieg_counter = 0;

        uint_fast8_t delay = 0;
        uint_fast8_t release = 0;
        uint_fast8_t n_time_to_delrel = 0;
        double delrel_time_step = 0;
        uint_fast8_t delay_counter = 0;
        uint_fast8_t release_counter = 0;
    };


    /**
     * @brief Editor class is used to ease the user while writing his song.
     * @note The user can create macros as shortcuts to the Editor's commands
     * @see example.cpp
     */
    class Editor{
    public:
        static void loadTrackProperties(uint_fast8_t number_of_rows, uint_fast8_t number_of_frames, uint_fast8_t number_of_channels, const uint_fast8_t *effects_per_chan);
        static Pattern** loadEmptyPatterns();
        static void prepare(Pattern **p, uint_fast8_t chanindx,  uint_fast8_t patternindx, uint_fast8_t instrumentnindx, float volume);
        static void prepare(uint_fast8_t chanindx, uint_fast8_t patternindx, uint_fast8_t instrumentnindx, float volume);
        static void prepare(uint_fast8_t chanindx, uint_fast8_t patternindx, float volume);
        static void storePatterns(Pattern **p);
        static void storeChannelIndex(uint_fast8_t chanindx);
        static void storePatternIndex(uint_fast8_t patternindx);
        static void storeInstrumentIndex(uint_fast8_t instrumentnindx);
        static void storeVolume(float volume);

        static void enterInstruction(uint_fast8_t instruction_index, C0deTracker::Key key);
        static void enterInstruction(uint_fast8_t instruction_index, uint_fast8_t instrument_index, C0deTracker::Key key);
        static void enterInstruction(uint_fast8_t instruction_index, C0deTracker::Key key, float volume);
        static void enterInstruction(uint_fast8_t instruction_index, uint_fast8_t instrument_index, C0deTracker::Key key, float volume);

        static void enterInstruction(uint_fast8_t instruction_index, C0deTracker::Key key, uint_fast32_t** effects);
        static void enterInstruction(uint_fast8_t instruction_index, C0deTracker::Key key, std::vector<uint_fast32_t> effects);
        static void enterInstruction(uint_fast8_t instruction_index, C0deTracker::Key key, uint_fast32_t effect);

        static void enterInstruction(uint_fast8_t instruction_index, uint_fast8_t instrument_index, C0deTracker::Key key, uint_fast32_t** effects);
        static void enterInstruction(uint_fast8_t instruction_index, uint_fast8_t instrument_index, C0deTracker::Key key, std::vector<uint_fast32_t> effects);
        static void enterInstruction(uint_fast8_t instruction_index, uint_fast8_t instrument_index, C0deTracker::Key key, uint_fast32_t effect);

        static void enterInstruction(uint_fast8_t instruction_index, C0deTracker::Key key, float volume, uint_fast32_t** effects);
        static void enterInstruction(uint_fast8_t instruction_index, C0deTracker::Key key, float volume, std::vector<uint_fast32_t> effects);
        static void enterInstruction(uint_fast8_t instruction_index, C0deTracker::Key key, float volume, uint_fast32_t effect);

        static void enterInstruction(uint_fast8_t instruction_index, uint_fast8_t instrument_index, C0deTracker::Key key, float volume, uint_fast32_t** effects);
        static void enterInstruction(uint_fast8_t instruction_index, uint_fast8_t instrument_index, C0deTracker::Key key, float volume, std::vector<uint_fast32_t> effects);
        static void enterInstruction(uint_fast8_t instruction_index, uint_fast8_t instrument_index, C0deTracker::Key key, float volume, uint_fast32_t effect);

        static void enterInstruction(uint_fast8_t instruction_index, float volume);

        static void enterInstruction(uint_fast8_t instruction_index, uint_fast32_t** effects);
        static void enterInstruction(uint_fast8_t instruction_index, float volume, uint_fast32_t** effects);
        static void enterInstruction(uint_fast8_t instruction_index, std::vector<uint_fast32_t> effects);
        static void enterInstruction(uint_fast8_t instruction_index, float volume, std::vector<uint_fast32_t> effects);
        static void enterInstruction(uint_fast8_t instruction_index, uint_fast32_t effect);
        static void enterInstruction(uint_fast8_t instruction_index, float volume, uint_fast32_t effect);

        static void release(uint_fast8_t instruction_index);
        static void release(uint_fast8_t instruction_index, float volume);
        static void release(uint_fast8_t instruction_index, uint_fast32_t** effects);
        static void release(uint_fast8_t instruction_index, float volume, uint_fast32_t** effects);
        static void release(uint_fast8_t instruction_index, std::vector<uint_fast32_t> effects);
        static void release(uint_fast8_t instruction_index, float volume, std::vector<uint_fast32_t> effects);
        static void release(uint_fast8_t instruction_index, uint_fast32_t effect);
        static void release(uint_fast8_t instruction_index, float volume, uint_fast32_t effect);

        static void storePatternsIndices(uint_fast8_t** pi);
        static uint_fast8_t** loadEmptyPatternsIndices();
        static void enterPatternIndice(uint_fast8_t channel, uint_fast8_t frame, uint_fast8_t pattern_indice);

    private:
        static Pattern **pattern;
        static uint_fast8_t** pattern_indices;
        static uint_fast8_t chan_index, pattern_index, instrument_index, frames;
        static float volume;
        static uint_fast8_t rows;
        static uint_fast8_t channels;
        static const uint_fast8_t *fx_per_chan;
    };


}

#endif //CODETRACKER_C0DE_TRACKER_HPP
