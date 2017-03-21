/*
 * Simple parser that reads in ampt events and writes out HepMC
 * events. 
 *  Redmer Alexander Bertens (rbertens @ cern.ch)
 * CERN, University of Knoxville, Tennessee, 2017
 */


#include <cstdio>
#include <string.h>
#include <stdio.h>

#include "AmptToHepMCParser.h"

using namespace std;
using namespace HepMC;

AmptToHepMCParser::~AmptToHepMCParser() {
    // class destructor
    delete _HepMC_writer;
}

bool AmptToHepMCParser::run() {

    // global run function

    // check if the program can run
    if(!prepare_environment()) return false;

    // open the HepMC writer
    if(!prepare_HepMC_writer()) return false;

    // if all is ok, start the event loop
    while (parse_ampt_event()) {
        if(_input_stream.eof()) break;
        reset_buffered_event();
    }

    // check if the expected number of events was converted
    if(buffered_event.e_event_number == _event_number) {
#ifdef VERBOSE_AMPTTOHEPMCPARSER
        printf(" - Succesfully parsed %i events \n", _event_number);
#endif 
    } else {
#ifdef VERBOSE_AMPTTOHEPMCPARSER
        printf(" - Only %i out of %i events parsed \n", buffered_event.e_event_number, _event_number);
#endif
        return false;
    }
    // return success status to main function
    return true;
}

bool AmptToHepMCParser::prepare_environment() {
    
    // start parsing
#ifdef VERBOSE_AMPTTOHEPMCPARSER
    printf("File for reading [%s] \n", _input_file.c_str());
    printf("File for writing [%s] \n", _output_file.c_str());
#endif
    // attach the instream for the input file
    _input_stream.open(_input_file.c_str());

    if(!_input_stream.good()) {
#ifdef VERBOSE_AMPTTOHEPMCPARSER
        printf(" - Fatal error, could not open [%s] for reading \n", _input_file.c_str());
#endif
        return false;
    }
   
    // prepare ampt event buffer
    buffered_event.weight = 0.;
    buffered_event.counts = 0;

    // start with event number 0
    _event_number = 0;

    return true;
}

bool AmptToHepMCParser::prepare_HepMC_writer() {
     // prepare the HepMC writer
    delete _HepMC_writer;
     _HepMC_writer = new IO_GenEvent(_output_file);
     if(!_HepMC_writer) {
#ifdef VERBOSE_AMPTTOHEPMCPARSER
         printf(" - Fatal error, could not open [%s] for writing \n", _output_file.c_str());
#endif
         return false;
     }
     return true;
}

bool AmptToHepMCParser::is_ampt_header(bool fetchline) {
    // check if the current line of the ifstream is an AMPT header
    // if fetchline is true, a new line will be read from the input stream
    // otherwise operation will be performed on the current line in memory
    if(fetchline) getline(_input_stream, _current_line);
    stringstream stream(_current_line.c_str());
    return (std::distance(std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>()) == 11);
}

bool AmptToHepMCParser::is_ampt_particle(bool fetchline) {
    // check if the current line of the ifstream is an AMPT particle
    // if fetchline is true, a new line will be read from the input stream
    // otherwise the operation will be performed on the current line in memory
    if(fetchline) getline(_input_stream, _current_line);
    stringstream stream(_current_line.c_str());
    return (std::distance(std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>()) == 9);
}

void AmptToHepMCParser::fill_header() {
    // fill the header with ampt variables - see header for explanation
    stringstream stream(_current_line.c_str());
    stream >> buffered_event.e_event_number >> buffered_event.e_test_number >> buffered_event.e_particles_in_event >> buffered_event.e_impact_param >> buffered_event.e_projectile_part >> buffered_event.e_target_part >> buffered_event.e_elastic_part >> buffered_event.e_inelastic_part >> buffered_event.e_elastic_target >> buffered_event.e_inelastic_target >> buffered_event.ek;
}

void AmptToHepMCParser::fill_particle() {
    // fill the track with ampt variables - see header for explanation
    stringstream stream(_current_line.c_str());
    stream >> buffered_event.p_pid >> buffered_event.p_px >> buffered_event.p_py >> buffered_event.p_pz >> buffered_event.p_mass >> buffered_event.p_x_fo >> buffered_event.p_y_fo >> buffered_event.p_z_fo >> buffered_event.p_t_fo;
}

bool AmptToHepMCParser::is_beam_particle() {
    // function to check if the particle is a beam particle. 
    // the check is performed by determining if x and y components of the momentum are 0 ,
    // and we have particle with infinite rapidity
    if (is_equal_to_zero(buffered_event.p_px)) return (is_equal_to_zero(buffered_event.p_py));
    return false;
}

#ifdef VERBOSE_AMPTTOHEPMCPARSER
void AmptToHepMCParser::print_header() {
    // print the buffered event header to the screen
    printf("%s \n", _current_line.c_str());           
    printf("event number %i \n", buffered_event.e_event_number);
    printf("test number %i \n", buffered_event.e_test_number);
    printf("particles in event %i \n", buffered_event.e_particles_in_event); 
    printf("impact parameter %.4f \n", buffered_event.e_impact_param);
    printf("total number of participant nucleons in projectile %i \n", buffered_event.e_projectile_part);
    printf("total number of participant nucleons in target %i \n", buffered_event.e_target_part);
    printf("number of participant nucleons in projectile due to elastic collisions %i \n", buffered_event.e_elastic_part);
    printf("number of participant nucleons in projectile due to inelastic collisions %i \n", buffered_event.e_inelastic_part);
    printf("corresponding number in target %i \n", buffered_event.e_elastic_target);
    printf("corresponding number in target %i \n", buffered_event.e_inelastic_target);
    printf("the mystery parameter %i \n", buffered_event.ek);
}

void AmptToHepMCParser::print_particle() {
    // print the buffered particle to the screen
    printf("%s \n", _current_line.c_str());           
    printf("pythia particle ID %i \n", buffered_event.p_pid);
    printf("px %.4f \n", buffered_event.p_px);
    printf("py %.4f \n", buffered_event.p_py);
    printf("pz %.4f \n", buffered_event.p_pz);
    printf("mass %.4f \n", buffered_event.p_mass);
    printf("x %.4f \n", buffered_event.p_x_fo);
    printf("y %.4f \n", buffered_event.p_y_fo);
    printf("z %.4f \n", buffered_event.p_z_fo);
    printf("t  %.4f \n", buffered_event.p_t_fo);
}
#endif

bool AmptToHepMCParser::parse_ampt_event() {
    // search for an event in the input file, read it, and convert it to hepmc

    // step 1) - searching for the ampt header
    if(is_ampt_header()) {
        fill_header();
    } else if (is_ampt_header(true)) {
        fill_header();
#ifdef VERBOSE_AMPTTOHEPMCPARSER
        print_header();
#endif
    }

    // step 2) - create the event on stack
    GenEvent parsed_event(Units::GEV, Units::MM);
    parsed_event.weights().push_back(1.);

    // step 3) - add heavy ion information
    HeavyIon heavyion(  // add the information to the heavy-ion header. for fields that are
                        // not available in ampt, we provide -1, which is obviously nonsensical
        -1,                                     // Number of hard scatterings
        buffered_event.e_projectile_part,       // Number of projectile participants
        buffered_event.e_target_part,           // Number of target participants
        -1,                                     // Number of NN (nucleon-nucleon) collisions
        -1,                                     // Number of N-Nwounded collisions
        -1,                                     // Number of Nwounded-N collisons
        -1,                                     // Number of Nwounded-Nwounded collisions
        -1,                                     // Number of spectator neutrons
        -1,                                     // Number of spectator protons
        buffered_event.e_impact_param,          // Impact Parameter(fm) of collision
        0.,                                     // Azimuthal angle of event plane
        -1.,                                    // eccentricity of participating nucleons
                                                        //in the transverse plane 
                                                        //(as in phobos nucl-ex/0510031) 
        -1.);                                   // nucleon-nucleon inelastic 
                                                        //(including diffractive) cross-section
    parsed_event.set_heavy_ion(heavyion);
    

    // step 4) add two `artificial' beam particles. the hepmc reader in aliroot expects two beam particles
    // (which in hepmc have a special status) ,so we need to create them. they will not 
    // end up in the analysis, as they have only longitudinal momentum. 
    // the particles are produced at 0,0,0, the same production vertex that we'll later use for all
    // final state particles as well
    GenVertex* v = new GenVertex();
    parsed_event.add_vertex(v);

    FourVector beam1(0,0,.14e4, get_particle_energy(0, 0, .14e4, 940));
    GenParticle* bp1 = new GenParticle(beam1, 2212, 2);
    bp1->set_generated_mass(940);
    v->add_particle_in(bp1);

    FourVector beam2(0,0,.14e4, get_particle_energy(0, 0, .14e4, 940));
    GenParticle* bp2 = new GenParticle(beam2, 2212, 2);
    bp2->set_generated_mass(940);
    v->add_particle_in(bp2);

    // hepmc does not seem to provide for more than 1 beam particle ...
    parsed_event.set_beam_particles(bp1,bp2);

    // step 5) - particle loop
    //
    // this section of the function goes through the ampt file line-by-line,
    // and breaks when a line cannot be identified as a particle (which should be
    // the next header or EOF)
    while(is_ampt_particle(true)) {
        // step 5a) extract info on particle from input stream
        fill_particle();
        buffered_event.counts++;
        
        // check if this isn't a beam particle from AMPT (usually the first 20 or 30 particles in the list)
        // AMPT `beam particles', i.e. participant nucleons, cannot be stored as beam particles in the
        // hepmc events, as hepmc only accomodates 2 beam particles, not multiple
        // we explicitly reject the particles here, as they lead to floating point issues
        // in the AliGenMC::KinematicCuts section which cannot deal very well with rapidities going to
        // infinity
        if(is_beam_particle()) continue;

#ifdef VERBOSE_AMPTTOHEPMCPARSER
        if(buffered_event.counts%1000==0) print_particle();
#endif

        // step 5b) create the particle itself (4 momentum, pid, etc)
        FourVector fourmom(
                buffered_event.p_px,
                buffered_event.p_py, 
                buffered_event.p_pz, 
                get_particle_energy());
        GenParticle* particle = new GenParticle(fourmom, buffered_event.p_pid, 1);
        particle->set_generated_mass(buffered_event.p_mass);

        // step 5c) connect the particle to its production vertex
        // all particles connect to the original beam vertex. this isn't strictly correct (and
        // ampt actually stores the freezeout vertex of particles), we do need however to have a 
        // 'mother-daughter' relationship for all particles .... 
          v->add_particle_out(particle);
    }

    // step 6) parsing is done, test if the expected number of particles was found
    // as a sanity check, so we're sure we parsed the full event
    if(buffered_event.counts != buffered_event.e_particles_in_event) {
#ifdef VERBOSE_AMPTTOHEPMCPARSER
        printf(" - Panic, number of generated tracks %i and read tracks %i do not correspond\n", buffered_event.e_particles_in_event, buffered_event.counts);
#endif
        return false;
    } else if (buffered_event.counts == 0) return false;

    // step 7) write out the event and return a success status
    // the parsed event owns the memory allocated by the production vertices, so 
    // this should be freed once the event goes out of scope
    _event_number++;
    _HepMC_writer->write_event(&parsed_event);
    return true;
}
