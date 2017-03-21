#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>

#include "HepMC/GenEvent.h"
#include "HepMC/IO_GenEvent.h"

using namespace std;
using namespace HepMC;

/*
 * Simple parser that convertes ampt events to the HepMC format
 * Redmer Alexander Bertens (rbertens @ cern.ch)
 * CERN, University of Knoxville, Tennessee, 2017
 */

// #define VERBOSE_AMPTTOHEPMCPARSER

class AmptToHepMCParser {
  public:

    // constructor and destructor  
    AmptToHepMCParser() : _HepMC_writer(0x0) {;}
    ~AmptToHepMCParser();

    // setter functions
    void        set_input_file(string input_file)       {_input_file = input_file; }
    void        set_output_file(string output_file)     {_output_file = output_file; }

    
    // getter functions
    string      get_input_file()  const       { return _input_file; }
    string      get_output_file() const      { return _output_file; }

    // main public function
    bool run();

    // miscellaneous
    bool is_equal_to_zero(double number, double tolerance = 1e-5) {
         return (fabs(number) < tolerance);
    }
        

  private:
    // general functions
    bool prepare_environment();
    bool prepare_HepMC_writer();
    bool is_ampt_header(bool fetchline = false);
    bool is_ampt_particle(bool fetchline = false);
    bool parse_ampt_event();
    double convert_fm_to_mm(double input) {return 1e-12*input; }

    // manipulation of event buffer struct
    bool reset_buffered_event() {
        buffered_event.weight = 0.;
        buffered_event.counts = 0;
    }
    void fill_header();
    void fill_particle();
#ifdef VERBOSE_AMPTTOHEPMCPARSER
    void print_header();
    void print_particle();
#endif
    double get_particle_energy() {
        double energy_squared = 
            buffered_event.p_px*buffered_event.p_px +
            buffered_event.p_py*buffered_event.p_py +
            buffered_event.p_pz*buffered_event.p_pz +
            buffered_event.p_mass*buffered_event.p_mass;
        if(energy_squared > 0.) return sqrt(energy_squared);
        return 0.;
    }
    double get_particle_energy(double p_px, double p_py, double p_pz, double p_mass) {
        double energy_squared = 
            p_px*p_px +
            p_py*p_py +
            p_pz*p_pz +
            p_mass*p_mass;
        if(energy_squared > 0.) return sqrt(energy_squared);
        return 0.;
    }
    bool is_beam_particle();

    // class members for input and output
    string      _input_file;            // name of input file (ampt)
    string      _output_file;           // name of output file (hepmc)
    ifstream    _input_stream;          // input stream
    string      _current_line;          // current line

    // class members hepmc
    IO_GenEvent*         _HepMC_writer;          // HepMC writer

    // simple struct for the event buffer
    struct ampt_buffer {
        double weight;
        int counts;
        // header, members are prefixed with 'e' to denote event
        int e_event_number;     // event number
        int e_test_number;      // test number
        int e_particles_in_event;      // particles in event
        double e_impact_param;  // impact parameter
        int e_projectile_part;  // total number of participant nucleons in projectile
        int e_target_part;      // total number of participant nucleons in target
        int e_elastic_part;     // number of participant nucleons in projectile due to elastic collisions
        int e_inelastic_part;   // number of participant nucleons in projectile due to inelastic collisions
        int e_elastic_target;   // corresponding number in target
        int e_inelastic_target; // corresponding number in target
        double ek;              // the mystery parameter
        // particle, members are prefixed with 'p' to denote particle
        int p_pid;      // pythia particle ID
        double p_px;    // px
        double p_py;    // py
        double p_pz;    // pz
        double p_mass;  // mass
        double p_x_fo;  // x spacetime vector at freeze out
        double p_y_fo;  // y
        double p_z_fo;  // z
        double p_t_fo;  // t 
    };
    ampt_buffer buffered_event;         // buffered event
    int         _event_number;          // current event
};

